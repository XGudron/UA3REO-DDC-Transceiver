#include "Bus.h"
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "esp_idf_version.h"
#include "driver/gpio.h"
#include "esp_rom_gpio.h"
#include "esp_private/periph_ctrl.h"
#include "soc/gpio_sig_map.h"
#include "soc/interrupts.h"
#include "hal/i2c_ll.h"
#include "soc/i2c_reg.h"
#include "esp_intr_alloc.h"
#if !CONFIG_IDF_TARGET_ESP32S3 || ESP_IDF_VERSION < ESP_IDF_VERSION_VAL(5, 5, 0)
#error Select ESP32S3 Dev Module with the esp32 board package 3.3.13 or 4.0.0.
#endif

// The STM32 software master writes a selector, then clocks the answer.
// Prepare the first FIFO before its read address. No task wakeup or Wi-Fi
// scheduling sits between the selector and the first byte.

static portMUX_TYPE mux = portMUX_INITIALIZER_UNLOCKED;
static QueueHandle_t frames;
static BusStats counts;
static WolfFrame input;
static bool overflow, messageDirty;
static uint8_t mail[64], message[64], output[64];
static uint32_t pendingId, stagedId, messageVersion, stagedMessageVersion;
static uint8_t staged, length, filled;
static const uint32_t TX = I2C_TXFIFO_WM_INT_ST_M;
static const uint32_t RX = I2C_RXFIFO_WM_INT_ST_M;
static const uint32_t END = I2C_TRANS_COMPLETE_INT_ST_M;

static void IRAM_ATTR fill() {
  uint32_t freeBytes;
  i2c_ll_get_txfifo_len(&I2C0, &freeBytes);
  uint8_t n = length - filled;
  if (n > freeBytes) n = freeBytes;
  if (n) i2c_ll_write_txfifo(&I2C0, output + filled, n);
  filled += n;
  if (filled < length) I2C0.int_ena.val |= TX;
  else I2C0.int_ena.val &= ~TX;
}

static void IRAM_ATTR prepare(uint8_t kind) {
  I2C0.int_ena.val &= ~TX;
  i2c_ll_txfifo_rst(&I2C0);
  staged = kind;
  filled = 0;
  length = kind == 0x20 ? 1 : 64;
  if (kind == 0x20) {
    counts.polls++;
    output[0] = (pendingId || messageDirty) ? 1 : 0;
  } else {
    const uint8_t *src = kind == 1 ? message : mail;
    for (unsigned i = 0; i < 64; ++i) output[i] = src[i];
    stagedId = pendingId;
    stagedMessageVersion = messageVersion;
  }
  fill();
}

static void IRAM_ATTR interrupt(void *arg) {
  const uint32_t flags = I2C0.int_status.val;
  I2C0.int_clr.val = flags;
  BaseType_t wake = pdFALSE;
  portENTER_CRITICAL_ISR(&mux);
  if (flags & (RX | END)) {
    uint32_t n;
    i2c_ll_get_rxfifo_cnt(&I2C0, &n);
    while (n--) {
      uint8_t value;
      i2c_ll_read_rxfifo(&I2C0, &value, 1);
      if (input.size < sizeof(input.bytes)) input.bytes[input.size++] = value;
      else overflow = true;
    }
  }
  if (flags & END) {
    if (input.size) {
      if (overflow) ++counts.dropped;
      else if (input.size == 1 && input.bytes[0] == 0x20) prepare(0x20);
      else if (input.size == 2 && input.bytes[0] == 0x30 &&
               (input.bytes[1] == 1 || input.bytes[1] == 2)) prepare(input.bytes[1]);
      else if (xQueueSendFromISR(frames, &input, &wake) != pdTRUE) ++counts.dropped;
      input.size = 0;
      overflow = false;
    } else if (staged) {
      // A short/aborted read must leave the mailbox pending for a retry.
      const bool complete = filled == length && I2C0.sr.txfifo_cnt == 0;
      if (complete && staged == 2) {
        ++counts.reads;
        if (stagedId && pendingId == stagedId) {
          counts.delivery = stagedId;
          pendingId = 0;
          mail[0] = 0;
        }
      } else if (complete && staged == 1) {
        ++counts.messages;
        if (messageVersion == stagedMessageVersion) messageDirty = false;
      } else if (!complete) ++counts.shortReads;
      staged = filled = length = 0;
      I2C0.int_ena.val &= ~TX;
    }
  }
  if ((flags & TX) && filled < length) fill();
  portEXIT_CRITICAL_ISR(&mux);
  if (wake) portYIELD_FROM_ISR();
}

bool startBus(uint8_t sdaPin, uint8_t sclPin) {
  frames = xQueueCreate(32, sizeof(WolfFrame));
  if (!frames) return false;
  const gpio_num_t sda = (gpio_num_t)sdaPin, scl = (gpio_num_t)sclPin;
  gpio_config_t pins = {
    .pin_bit_mask = (1ULL << sda) | (1ULL << scl),
    .mode = GPIO_MODE_INPUT_OUTPUT_OD,
    .pull_up_en = GPIO_PULLUP_ENABLE,
    .pull_down_en = GPIO_PULLDOWN_DISABLE,
    .intr_type = GPIO_INTR_DISABLE
  };
  // Release both open-drain lines before connecting the peripheral.
  gpio_set_level(sda, 1);
  gpio_set_level(scl, 1);
  if (gpio_config(&pins) != ESP_OK) goto fail;
  PERIPH_RCC_ATOMIC() {
    i2c_ll_enable_bus_clock(0, true);
    i2c_ll_reset_register(0);
    i2c_ll_enable_controller_clock(&I2C0, true);
  }
  i2c_ll_set_source_clk(&I2C0, I2C_CLK_SRC_XTAL);
  i2c_ll_set_mode(&I2C0, I2C_BUS_MODE_SLAVE);
  i2c_ll_enable_pins_open_drain(&I2C0, true);
  i2c_ll_enable_fifo_mode(&I2C0, true);
  i2c_ll_slave_enable_auto_start(&I2C0, true);
  i2c_ll_set_slave_addr(&I2C0, 0x76, false);
  i2c_ll_set_sda_timing(&I2C0, 10, 10);
  i2c_ll_set_tout(&I2C0, I2C_LL_MAX_TIMEOUT);
  I2C0.int_ena.val = 0;
  I2C0.scl_stretch_conf.val = 0;
  i2c_ll_rxfifo_rst(&I2C0);
  i2c_ll_txfifo_rst(&I2C0);
  i2c_ll_set_rxfifo_full_thr(&I2C0, 12);
  i2c_ll_set_txfifo_empty_thr(&I2C0, 12);
  i2c_ll_update(&I2C0);
  I2C0.int_clr.val = 0xffffffff;
  if (esp_intr_alloc(ETS_I2C_EXT0_INTR_SOURCE, ESP_INTR_FLAG_IRAM | ESP_INTR_FLAG_LEVEL2,
                     interrupt, NULL, NULL) != ESP_OK) goto fail;
  esp_rom_gpio_connect_in_signal(sda, I2CEXT0_SDA_IN_IDX, false);
  esp_rom_gpio_connect_in_signal(scl, I2CEXT0_SCL_IN_IDX, false);
  esp_rom_gpio_connect_out_signal(sda, I2CEXT0_SDA_OUT_IDX, false, false);
  esp_rom_gpio_connect_out_signal(scl, I2CEXT0_SCL_OUT_IDX, false, false);
  I2C0.int_ena.val = RX | END;
  return true;
fail:
  vQueueDelete(frames);
  frames = NULL;
  return false;
}

bool nextFrame(WolfFrame *f) { return frames && xQueueReceive(frames, f, 0) == pdTRUE; }

BusStats busStats() {
  portENTER_CRITICAL(&mux);
  BusStats copy = counts;
  portEXIT_CRITICAL(&mux);
  return copy;
}

void publishCat(const char *s, uint32_t serial) {
  uint8_t buffer[64] = {0};
  strlcpy((char *)buffer, s, sizeof(buffer));
  portENTER_CRITICAL(&mux);
  memcpy(mail, buffer, 64);
  pendingId = serial;
  portEXIT_CRITICAL(&mux);
}

void publishMessage(const char *s) {
  uint8_t buffer[64] = {0};
  strlcpy((char *)buffer, s, sizeof(buffer));
  portENTER_CRITICAL(&mux);
  memcpy(message, buffer, 64);
  messageDirty = true;
  ++messageVersion;
  portEXIT_CRITICAL(&mux);
}
