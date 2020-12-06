#ifndef TRX_MANAGER_H
#define TRX_MANAGER_H

#include "stm32h7xx_hal.h"
#include <stdbool.h>
#include "settings.h"

extern void TRX_Init(void);
extern void TRX_setFrequency(uint32_t _freq, VFO *vfo);
extern void TRX_setMode(uint_fast8_t _mode, VFO *vfo);
extern void TRX_ptt_change(void);
extern void TRX_key_change(void);
extern bool TRX_on_TX(void);
extern void TRX_DoAutoGain(void);
extern void TRX_Restart_Mode(void);
extern void TRX_DBMCalculate(void);
extern float32_t TRX_GenerateCWSignal(float32_t power);
extern float32_t TRX_getSTM32H743Temperature(void);
extern float32_t TRX_getSTM32H743vref(void);
extern void TRX_TemporaryMute(void);
extern void TRX_ProcessScanMode(void);

volatile extern bool TRX_ptt_hard;
volatile extern bool TRX_ptt_cat;
volatile extern bool TRX_old_ptt_cat;
volatile extern bool TRX_key_serial;
volatile extern bool TRX_old_key_serial;
volatile extern bool TRX_key_dot_hard;
volatile extern bool TRX_key_dash_hard;
volatile extern uint_fast16_t TRX_Key_Timeout_est;
volatile extern bool TRX_RX1_IQ_swap;
volatile extern bool TRX_RX2_IQ_swap;
volatile extern bool TRX_TX_IQ_swap;
volatile extern bool TRX_Tune;
volatile extern bool TRX_Inited;
volatile extern int_fast16_t TRX_RX_dBm;
volatile extern bool TRX_ADC_OTR;
volatile extern bool TRX_DAC_OTR;
volatile extern int16_t TRX_ADC_MINAMPLITUDE;
volatile extern int16_t TRX_ADC_MAXAMPLITUDE;
volatile extern int32_t TRX_VCXO_ERROR;
volatile extern uint32_t TRX_SNTP_Synced;
volatile extern int_fast16_t TRX_SHIFT;
volatile extern float32_t TRX_MAX_TX_Amplitude;
volatile extern float32_t TRX_PWR_Forward;
volatile extern float32_t TRX_PWR_Backward;
volatile extern float32_t TRX_SWR;
volatile extern float32_t TRX_VLT_forward;				//Tisho 
volatile extern float32_t TRX_VLT_backward;				//Tisho
volatile extern float32_t TRX_ALC;
volatile extern bool TRX_DAC_DIV0;
volatile extern bool TRX_DAC_DIV1;
volatile extern bool TRX_DAC_HP1;
volatile extern bool TRX_DAC_HP2;
volatile extern bool TRX_DAC_X4;
volatile extern bool TRX_DCDC_Freq;
volatile extern bool TRX_Mute;
volatile extern uint16_t TRX_Volume;
volatile extern float32_t TRX_STM32_VREF;
volatile extern float32_t TRX_STM32_TEMPERATURE;
volatile extern float32_t TRX_IQ_phase_error;
volatile extern bool TRX_NeedGoToBootloader;
volatile extern bool TRX_Temporary_Stop_BandMap;
volatile extern float32_t TRX_RF_Temperature;
volatile extern uint8_t TRX_AutoGain_Stage;
extern const char *MODE_DESCR[];
extern ADC_HandleTypeDef hadc1;
extern uint32_t TRX_freq_phrase;
extern uint32_t TRX_freq_phrase2;
extern uint32_t TRX_freq_phrase_tx;
volatile extern uint32_t TRX_Temporary_Mute_StartTime;
volatile extern bool TRX_ScanMode;

#endif
