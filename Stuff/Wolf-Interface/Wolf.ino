#include "Wolf.h"
#include "driver/i2c.h"
#include "freertos/FreeRTOS.h"

#define WOLF_I2C_SLAVE_SDA_IO GPIO_NUM_21
#define WOLF_I2C_SLAVE_SCL_IO GPIO_NUM_22
#define WOLF_I2C_SLAVE_NUM I2C_NUM_0
#define WOLF_I2C_ADDR 0b1110110 // without RW bit
#define WOLF_BUF_LEN 256

tWOLF TRX_WOLF;
tWOLFInterface TRX_INTERFACE;

static uint8_t WOLF_inBuff[WOLF_BUF_LEN];
static uint8_t WOLF_outBuff[WOLF_BUF_LEN];

static esp_err_t WOLF_i2c_slave_init() {
	i2c_config_t conf_slave;
	conf_slave.sda_io_num = WOLF_I2C_SLAVE_SDA_IO;
	conf_slave.sda_pullup_en = GPIO_PULLUP_ENABLE;
	conf_slave.scl_io_num = WOLF_I2C_SLAVE_SCL_IO;
	conf_slave.scl_pullup_en = GPIO_PULLUP_ENABLE;
	conf_slave.mode = I2C_MODE_SLAVE;
	conf_slave.slave.addr_10bit_en = 0;
	conf_slave.slave.slave_addr = WOLF_I2C_ADDR;
	conf_slave.slave.maximum_speed = 300000;
	conf_slave.clk_flags = 0;
	i2c_param_config(WOLF_I2C_SLAVE_NUM, &conf_slave);
	return i2c_driver_install(WOLF_I2C_SLAVE_NUM, conf_slave.mode, WOLF_BUF_LEN, WOLF_BUF_LEN, 0);
}

static void WOLF_i2c_background_task(void *arg) {
	while (1) {
		size_t size = i2c_slave_read_buffer(WOLF_I2C_SLAVE_NUM, WOLF_inBuff, 1, 1000 / portTICK_RATE_MS);
		if (size != 1) {
			continue;
		}

		if (WOLF_inBuff[0] == 0x10) // write structure command
		{
			size = i2c_slave_read_buffer(WOLF_I2C_SLAVE_NUM, WOLF_inBuff, 1, 1000 / portTICK_RATE_MS);
			if (size != 1) {
				continue;
			}

			if (WOLF_inBuff[0] == 0x01) {
				i2c_slave_read_buffer(WOLF_I2C_SLAVE_NUM, (uint8_t *)&TRX_WOLF.CurrentVFO, sizeof(TRX_WOLF.CurrentVFO), 1000 / portTICK_RATE_MS);
			}
			if (WOLF_inBuff[0] == 0x02) {
				i2c_slave_read_buffer(WOLF_I2C_SLAVE_NUM, (uint8_t *)&TRX_WOLF.VFO_A_Frequency, sizeof(TRX_WOLF.VFO_A_Frequency), 1000 / portTICK_RATE_MS);
			}
			if (WOLF_inBuff[0] == 0x03) {
				i2c_slave_read_buffer(WOLF_I2C_SLAVE_NUM, (uint8_t *)&TRX_WOLF.VFO_B_Frequency, sizeof(TRX_WOLF.VFO_B_Frequency), 1000 / portTICK_RATE_MS);
			}
			if (WOLF_inBuff[0] == 0x04) {
				i2c_slave_read_buffer(WOLF_I2C_SLAVE_NUM, (uint8_t *)&TRX_WOLF.VFO_A_Mode, sizeof(TRX_WOLF.VFO_A_Mode), 1000 / portTICK_RATE_MS);
			}
			if (WOLF_inBuff[0] == 0x05) {
				i2c_slave_read_buffer(WOLF_I2C_SLAVE_NUM, (uint8_t *)&TRX_WOLF.VFO_B_Mode, sizeof(TRX_WOLF.VFO_B_Mode), 1000 / portTICK_RATE_MS);
			}
			if (WOLF_inBuff[0] == 0x06) {
				i2c_slave_read_buffer(WOLF_I2C_SLAVE_NUM, (uint8_t *)&TRX_WOLF.CAT_Answer, sizeof(TRX_WOLF.CAT_Answer), 1000 / portTICK_RATE_MS);
			}
			if (WOLF_inBuff[0] == 0x07) {
				i2c_slave_read_buffer(WOLF_I2C_SLAVE_NUM, (uint8_t *)&TRX_WOLF.TX, sizeof(TRX_WOLF.TX), 1000 / portTICK_RATE_MS);
			}
			if (WOLF_inBuff[0] == 0x08) {
				i2c_slave_read_buffer(WOLF_I2C_SLAVE_NUM, (uint8_t *)&TRX_WOLF.Tune, sizeof(TRX_WOLF.Tune), 1000 / portTICK_RATE_MS);
			}
		}

		if (WOLF_inBuff[0] == 0x20) // read has changes? command
		{
			WOLF_outBuff[0] = TRX_INTERFACE.hasDataChanges ? 0x01 : 0x00;
			i2c_reset_tx_fifo(WOLF_I2C_SLAVE_NUM);
			i2c_slave_write_buffer(WOLF_I2C_SLAVE_NUM, WOLF_outBuff, 1, 1000 / portTICK_RATE_MS);
		}

		if (WOLF_inBuff[0] == 0x30) // read data from interface command
		{
			size = i2c_slave_read_buffer(WOLF_I2C_SLAVE_NUM, WOLF_inBuff, 1, 1000 / portTICK_RATE_MS);
			if (size != 1) {
				continue;
			}

			i2c_reset_tx_fifo(WOLF_I2C_SLAVE_NUM);

			if (WOLF_inBuff[0] == 0x01) {
				i2c_slave_write_buffer(WOLF_I2C_SLAVE_NUM, (uint8_t *)TRX_INTERFACE.Message, sizeof(TRX_INTERFACE.Message), 1000 / portTICK_RATE_MS);
			}
			if (WOLF_inBuff[0] == 0x02) {
				i2c_slave_write_buffer(WOLF_I2C_SLAVE_NUM, (uint8_t *)TRX_INTERFACE.CAT_Command, sizeof(TRX_INTERFACE.CAT_Command), 1000 / portTICK_RATE_MS);
				memset(TRX_INTERFACE.CAT_Command, 0x00, sizeof(TRX_INTERFACE.CAT_Command));
			}

			TRX_INTERFACE.hasDataChanges = false;
		}
	}
}

void WOLF_init(void) {
	ESP_ERROR_CHECK(WOLF_i2c_slave_init());
	xTaskCreate(WOLF_i2c_background_task, "wolf-task", 1024 * 2, (void *)1, 10, NULL);
}

String WOLF_modeToString(uint8_t mode) {
	const char *MODE_DESCR[] = {"LSB", "USB", "CW", "NFM", "WFM", "AM", "SAM", "AM-L", "AM-U", "DIGL", "DIGU", "IQ", "LOOP", "RTTY", "DSB"};
	return String(MODE_DESCR[mode]);
}

void WOLF_setMessage(String message) {
	message.toCharArray(TRX_INTERFACE.Message, sizeof(TRX_INTERFACE.Message));
	TRX_INTERFACE.hasDataChanges = true;
}

String WOLF_catCommand(String command) {
	memset(TRX_WOLF.CAT_Answer, 0x00, sizeof(TRX_WOLF.CAT_Answer));
	command.toCharArray(TRX_INTERFACE.CAT_Command, sizeof(TRX_INTERFACE.CAT_Command));
	TRX_INTERFACE.hasDataChanges = true;

	uint32_t startMs = esp_timer_get_time() / 1000;
	while ((esp_timer_get_time() / 1000) - startMs < 2000) { // CAT Timeout
		if (strlen(TRX_WOLF.CAT_Answer) > 0) {
			return String(TRX_WOLF.CAT_Answer);
		}
	}

	return "";
}
