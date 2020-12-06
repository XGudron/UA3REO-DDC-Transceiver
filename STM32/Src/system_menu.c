#include "system_menu.h"
#include "lcd.h"
#include "settings.h"
#include "audio_filters.h"
#include "bootloader.h"
#include "functions.h"
#include "wifi.h"
#include "spec_analyzer.h"
#include "swr_analyzer.h"
#include "fonts.h"
#include "agc.h"
#include "screen_layout.h"
#include "noise_blanker.h"
#include "bands.h"
#include "sd.h"

static void SYSMENU_HANDL_TRX_RFPower(int8_t direction);
static void SYSMENU_HANDL_TRX_BandMap(int8_t direction);
static void SYSMENU_HANDL_TRX_AutoGain(int8_t direction);
static void SYSMENU_HANDL_TRX_TWO_SIGNAL_TUNE(int8_t direction);
static void SYSMENU_HANDL_TRX_RFFilters(int8_t direction);
static void SYSMENU_HANDL_TRX_MICIN(int8_t direction);
static void SYSMENU_HANDL_TRX_LINEIN(int8_t direction);
static void SYSMENU_HANDL_TRX_USBIN(int8_t direction);
static void SYSMENU_HANDL_TRX_SHIFT_INTERVAL(int8_t direction);
static void SYSMENU_HANDL_TRX_FRQ_STEP(int8_t direction);
static void SYSMENU_HANDL_TRX_FRQ_FAST_STEP(int8_t direction);
static void SYSMENU_HANDL_TRX_FRQ_ENC_STEP(int8_t direction);
static void SYSMENU_HANDL_TRX_FRQ_ENC_FAST_STEP(int8_t direction);
static void SYSMENU_HANDL_TRX_ENC_ACCELERATE(int8_t direction);
static void SYSMENU_HANDL_TRX_ATT_STEP(int8_t direction);
static void SYSMENU_HANDL_TRX_DEBUG_CONSOLE(int8_t direction);
static void SYSMENU_HANDL_TRX_SetCallsign(int8_t direction);
static void SYSMENU_HANDL_TRX_TRANSV_ENABLE(int8_t direction);
static void SYSMENU_HANDL_TRX_TRANSV_OFFSET(int8_t direction);

static void SYSMENU_HANDL_AUDIO_IFGain(int8_t direction);
static void SYSMENU_HANDL_AUDIO_AGC_GAIN_TARGET(int8_t direction);
static void SYSMENU_HANDL_AUDIO_MIC_Gain(int8_t direction);
static void SYSMENU_HANDL_AUDIO_DNR_THRES(int8_t direction);
static void SYSMENU_HANDL_AUDIO_DNR_AVERAGE(int8_t direction);
static void SYSMENU_HANDL_AUDIO_DNR_MINMAL(int8_t direction);
static void SYSMENU_HANDL_AUDIO_SSB_HPF_pass(int8_t direction);
static void SYSMENU_HANDL_AUDIO_SSB_LPF_pass(int8_t direction);
static void SYSMENU_HANDL_AUDIO_CW_LPF_pass(int8_t direction);
static void SYSMENU_HANDL_AUDIO_CW_HPF_pass(int8_t direction);
static void SYSMENU_HANDL_AUDIO_AM_LPF_pass(int8_t direction);
static void SYSMENU_HANDL_AUDIO_FM_LPF_pass(int8_t direction);
static void SYSMENU_HANDL_AUDIO_MIC_EQ_LOW(int8_t direction);
static void SYSMENU_HANDL_AUDIO_MIC_EQ_MID(int8_t direction);
static void SYSMENU_HANDL_AUDIO_MIC_EQ_HIG(int8_t direction);
static void SYSMENU_HANDL_AUDIO_MIC_REVERBER(int8_t direction);
static void SYSMENU_HANDL_AUDIO_RX_EQ_LOW(int8_t direction);
static void SYSMENU_HANDL_AUDIO_RX_EQ_MID(int8_t direction);
static void SYSMENU_HANDL_AUDIO_RX_EQ_HIG(int8_t direction);
static void SYSMENU_HANDL_AUDIO_RX_AGC_SSB_Speed(int8_t direction);
static void SYSMENU_HANDL_AUDIO_RX_AGC_CW_Speed(int8_t direction);
static void SYSMENU_HANDL_AUDIO_TX_AGCSpeed(int8_t direction);
static void SYSMENU_HANDL_AUDIO_FMSquelch(int8_t direction);
static void SYSMENU_HANDL_AUDIO_VAD_Squelch(int8_t direction);
static void SYSMENU_HANDL_AUDIO_Beeper(int8_t direction);

static void SYSMENU_HANDL_CW_GENERATOR_SHIFT_HZ(int8_t direction);
static void SYSMENU_HANDL_CW_Decoder(int8_t direction);
static void SYSMENU_HANDL_CW_SelfHear(int8_t direction);
static void SYSMENU_HANDL_CW_Keyer(int8_t direction);
static void SYSMENU_HANDL_CW_Keyer_WPM(int8_t direction);
static void SYSMENU_HANDL_CW_Key_timeout(int8_t direction);

static void SYSMENU_HANDL_SCREEN_FFT_Enabled(int8_t direction);
static void SYSMENU_HANDL_SCREEN_COLOR_THEME(int8_t direction);
static void SYSMENU_HANDL_SCREEN_LAYOUT_THEME(int8_t direction);
static void SYSMENU_HANDL_SCREEN_FFT_Averaging(int8_t direction);
static void SYSMENU_HANDL_SCREEN_FFT_Window(int8_t direction);
static void SYSMENU_HANDL_SCREEN_FFT_Zoom(int8_t direction);
static void SYSMENU_HANDL_SCREEN_FFT_Height(int8_t direction);
static void SYSMENU_HANDL_SCREEN_FFT_Color(int8_t direction);
static void SYSMENU_HANDL_SCREEN_FFT_Grid(int8_t direction);
static void SYSMENU_HANDL_SCREEN_FFT_Background(int8_t direction);
static void SYSMENU_HANDL_SCREEN_FFT_Speed(int8_t direction);
static void SYSMENU_HANDL_SCREEN_FFT_Compressor(int8_t direction);

static void SYSMENU_HANDL_ADC_PGA(int8_t direction);
static void SYSMENU_HANDL_ADC_RAND(int8_t direction);
static void SYSMENU_HANDL_ADC_SHDN(int8_t direction);
static void SYSMENU_HANDL_ADC_DITH(int8_t direction);
static void SYSMENU_HANDL_ADC_DRIVER(int8_t direction);

static void SYSMENU_HANDL_WIFI_Enabled(int8_t direction);
static void SYSMENU_HANDL_WIFI_SelectAP(int8_t direction);
static void SYSMENU_HANDL_WIFI_SetAPpassword(int8_t direction);
static void SYSMENU_HANDL_WIFI_Timezone(int8_t direction);
static void SYSMENU_HANDL_WIFI_CAT_Server(int8_t direction);
//static void SYSMENU_HANDL_WIFI_UpdateFW(int8_t direction);

static void SYSMENU_HANDL_SD_Format(int8_t direction);
static void SYSMENU_HANDL_SD_ExportSettings(int8_t direction);
static void SYSMENU_HANDL_SD_ImportSettings(int8_t direction);

static void SYSMENU_HANDL_SETTIME(int8_t direction);
static void SYSMENU_HANDL_Bootloader(int8_t direction);

static void SYSMENU_HANDL_SPECTRUM_Begin(int8_t direction);
static void SYSMENU_HANDL_SPECTRUM_Start(int8_t direction);
static void SYSMENU_HANDL_SPECTRUM_End(int8_t direction);
static void SYSMENU_HANDL_SPECTRUM_TopDBM(int8_t direction);
static void SYSMENU_HANDL_SPECTRUM_BottomDBM(int8_t direction);

static void SYSMENU_HANDL_CALIB_ENCODER_SLOW_RATE(int8_t direction);
static void SYSMENU_HANDL_CALIB_ENCODER_INVERT(int8_t direction);
static void SYSMENU_HANDL_CALIB_ENCODER2_INVERT(int8_t direction);
static void SYSMENU_HANDL_CALIB_ENCODER_DEBOUNCE(int8_t direction);
static void SYSMENU_HANDL_CALIB_ENCODER2_DEBOUNCE(int8_t direction);
static void SYSMENU_HANDL_CALIB_ENCODER_ON_FALLING(int8_t direction);
static void SYSMENU_HANDL_CALIB_ENCODER_ACCELERATION(int8_t direction);
static void SYSMENU_HANDL_CALIB_CICCOMP_SHIFT(int8_t direction);
static void SYSMENU_HANDL_CALIB_TXCICCOMP_SHIFT(int8_t direction);
static void SYSMENU_HANDL_CALIB_DAC_SHIFT(int8_t direction);
static void SYSMENU_HANDL_CALIB_RF_GAIN_LF(int8_t direction);
static void SYSMENU_HANDL_CALIB_RF_GAIN_HF_LOW(int8_t direction);
static void SYSMENU_HANDL_CALIB_RF_GAIN_HF1(int8_t direction);
static void SYSMENU_HANDL_CALIB_RF_GAIN_HF2(int8_t direction);
static void SYSMENU_HANDL_CALIB_RF_GAIN_HF_HIGH(int8_t direction);
static void SYSMENU_HANDL_CALIB_RF_GAIN_VHF(int8_t direction);
static void SYSMENU_HANDL_CALIB_S_METER(int8_t direction);
static void SYSMENU_HANDL_CALIB_ADC_OFFSET(int8_t direction);
static void SYSMENU_HANDL_CALIB_LPF_END(int8_t direction);
static void SYSMENU_HANDL_CALIB_BPF_0_START(int8_t direction);
static void SYSMENU_HANDL_CALIB_BPF_0_END(int8_t direction);
static void SYSMENU_HANDL_CALIB_BPF_1_START(int8_t direction);
static void SYSMENU_HANDL_CALIB_BPF_1_END(int8_t direction);
static void SYSMENU_HANDL_CALIB_BPF_2_START(int8_t direction);
static void SYSMENU_HANDL_CALIB_BPF_2_END(int8_t direction);
static void SYSMENU_HANDL_CALIB_BPF_3_START(int8_t direction);
static void SYSMENU_HANDL_CALIB_BPF_3_END(int8_t direction);
static void SYSMENU_HANDL_CALIB_BPF_4_START(int8_t direction);
static void SYSMENU_HANDL_CALIB_BPF_4_END(int8_t direction);
static void SYSMENU_HANDL_CALIB_BPF_5_START(int8_t direction);
static void SYSMENU_HANDL_CALIB_BPF_5_END(int8_t direction);
static void SYSMENU_HANDL_CALIB_BPF_6_START(int8_t direction);
static void SYSMENU_HANDL_CALIB_BPF_6_END(int8_t direction);
static void SYSMENU_HANDL_CALIB_HPF_START(int8_t direction);
static void SYSMENU_HANDL_CALIB_SWR_TRANS_RATE(int8_t direction);
static void SYSMENU_HANDL_CALIB_VCXO(int8_t direction);
static void SYSMENU_HANDL_CALIB_FW_AD8307_SLP(int8_t direction);	//Tisho	  
static void SYSMENU_HANDL_CALIB_FW_AD8307_OFFS(int8_t direction);	//Tisho
static void SYSMENU_HANDL_CALIB_BW_AD8307_SLP(int8_t direction);	//Tisho
static void SYSMENU_HANDL_CALIB_BW_AD8307_OFFS(int8_t direction);	//Tisho

static void SYSMENU_HANDL_TRXMENU(int8_t direction);
static void SYSMENU_HANDL_AUDIOMENU(int8_t direction);
static void SYSMENU_HANDL_CWMENU(int8_t direction);
static void SYSMENU_HANDL_LCDMENU(int8_t direction);
static void SYSMENU_HANDL_ADCMENU(int8_t direction);
static void SYSMENU_HANDL_WIFIMENU(int8_t direction);
static void SYSMENU_HANDL_SDMENU(int8_t direction);
static void SYSMENU_HANDL_CALIBRATIONMENU(int8_t direction);

static void SYSMENU_HANDL_SPECTRUMMENU(int8_t direction);
static void SYSMENU_HANDL_SWR_BAND_START(int8_t direction);
static void SYSMENU_HANDL_SWR_HF_START(int8_t direction);
static void SYSMENU_HANDL_RDA_STATS(int8_t direction);
static void SYSMENU_HANDL_PROPAGINATION(int8_t direction);
static void SYSMENU_HANDL_SWR_Tandem_Ctrl(int8_t direction);		//Tisho

IRAM2 static struct sysmenu_item_handler sysmenu_handlers[] =
	{
		{"TRX Settings", SYSMENU_MENU, 0, SYSMENU_HANDL_TRXMENU},
		{"AUDIO Settings", SYSMENU_MENU, 0, SYSMENU_HANDL_AUDIOMENU},
		{"CW Settings", SYSMENU_MENU, 0, SYSMENU_HANDL_CWMENU},
		{"SCREEN Settings", SYSMENU_MENU, 0, SYSMENU_HANDL_LCDMENU},
		{"ADC/DAC Settings", SYSMENU_MENU, 0, SYSMENU_HANDL_ADCMENU},
		{"WIFI Settings", SYSMENU_MENU, 0, SYSMENU_HANDL_WIFIMENU},
		{"SD Card", SYSMENU_MENU, 0, SYSMENU_HANDL_SDMENU},
		{"Set Clock Time", SYSMENU_RUN, 0, SYSMENU_HANDL_SETTIME},
		{"Flash update", SYSMENU_RUN, 0, SYSMENU_HANDL_Bootloader},
		{"Calibration", SYSMENU_HIDDEN_MENU, 0, SYSMENU_HANDL_CALIBRATIONMENU},
};
static uint8_t sysmenu_item_count = sizeof(sysmenu_handlers) / sizeof(sysmenu_handlers[0]);

IRAM2 static struct sysmenu_item_handler sysmenu_trx_handlers[] =
	{
		{"RF Power", SYSMENU_UINT8, (uint32_t *)&TRX.RF_Power, SYSMENU_HANDL_TRX_RFPower},
		{"Band Map", SYSMENU_BOOLEAN, (uint32_t *)&TRX.BandMapEnabled, SYSMENU_HANDL_TRX_BandMap},
		{"AutoGainer", SYSMENU_BOOLEAN, (uint32_t *)&TRX.AutoGain, SYSMENU_HANDL_TRX_AutoGain},
		{"RF_Filters", SYSMENU_BOOLEAN, (uint32_t *)&TRX.RF_Filters, SYSMENU_HANDL_TRX_RFFilters},
		{"Two Signal TUNE", SYSMENU_BOOLEAN, (uint32_t *)&TRX.TWO_SIGNAL_TUNE, SYSMENU_HANDL_TRX_TWO_SIGNAL_TUNE},
		{"Shift Interval", SYSMENU_UINT16, (uint32_t *)&TRX.SHIFT_INTERVAL, SYSMENU_HANDL_TRX_SHIFT_INTERVAL},
		{"Freq Step", SYSMENU_UINT16, (uint32_t *)&TRX.FRQ_STEP, SYSMENU_HANDL_TRX_FRQ_STEP},
		{"Freq Step FAST", SYSMENU_UINT16, (uint32_t *)&TRX.FRQ_FAST_STEP, SYSMENU_HANDL_TRX_FRQ_FAST_STEP},
		{"Freq Step ENC2", SYSMENU_UINT16, (uint32_t *)&TRX.FRQ_ENC_STEP, SYSMENU_HANDL_TRX_FRQ_ENC_STEP},
		{"Freq Step ENC2 FAST", SYSMENU_UINT32R, (uint32_t *)&TRX.FRQ_ENC_FAST_STEP, SYSMENU_HANDL_TRX_FRQ_ENC_FAST_STEP},
		{"Encoder Accelerate", SYSMENU_BOOLEAN, (uint32_t *)&TRX.Encoder_Accelerate, SYSMENU_HANDL_TRX_ENC_ACCELERATE},
		{"Att step, dB", SYSMENU_UINT8, (uint32_t *)&TRX.ATT_STEP, SYSMENU_HANDL_TRX_ATT_STEP},
		{"DEBUG Console", SYSMENU_BOOLEAN, (uint32_t *)&TRX.Debug_Console, SYSMENU_HANDL_TRX_DEBUG_CONSOLE},
		{"MIC IN", SYSMENU_BOOLEAN, (uint32_t *)&TRX.InputType_MIC, SYSMENU_HANDL_TRX_MICIN},
		{"LINE IN", SYSMENU_BOOLEAN, (uint32_t *)&TRX.InputType_LINE, SYSMENU_HANDL_TRX_LINEIN},
		{"USB IN", SYSMENU_BOOLEAN, (uint32_t *)&TRX.InputType_USB, SYSMENU_HANDL_TRX_USBIN},
		{"Callsign", SYSMENU_RUN, 0, SYSMENU_HANDL_TRX_SetCallsign},
		{"Transverter Enable", SYSMENU_BOOLEAN, (uint32_t *)&TRX.Transverter_Enabled, SYSMENU_HANDL_TRX_TRANSV_ENABLE},
		{"Transverter Offset, mHz", SYSMENU_UINT16, (uint32_t *)&TRX.Transverter_Offset_Mhz, SYSMENU_HANDL_TRX_TRANSV_OFFSET},
};
static uint8_t sysmenu_trx_item_count = sizeof(sysmenu_trx_handlers) / sizeof(sysmenu_trx_handlers[0]);

IRAM2 static struct sysmenu_item_handler sysmenu_audio_handlers[] =
	{
		{"IF Gain, dB", SYSMENU_UINT8, (uint32_t *)&TRX.IF_Gain, SYSMENU_HANDL_AUDIO_IFGain},
		{"AGC Gain target, LKFS", SYSMENU_INT8, (uint32_t *)&TRX.AGC_GAIN_TARGET, SYSMENU_HANDL_AUDIO_AGC_GAIN_TARGET},
		{"Mic Gain", SYSMENU_UINT8, (uint32_t *)&TRX.MIC_GAIN, SYSMENU_HANDL_AUDIO_MIC_Gain},
		{"DNR Threshold", SYSMENU_UINT8, (uint32_t *)&TRX.DNR_SNR_THRESHOLD, SYSMENU_HANDL_AUDIO_DNR_THRES},
		{"DNR Average", SYSMENU_UINT8, (uint32_t *)&TRX.DNR_AVERAGE, SYSMENU_HANDL_AUDIO_DNR_AVERAGE},
		{"DNR Minimal", SYSMENU_UINT8, (uint32_t *)&TRX.DNR_MINIMAL, SYSMENU_HANDL_AUDIO_DNR_MINMAL},
		{"SSB HPF Pass", SYSMENU_UINT16, (uint32_t *)&TRX.SSB_HPF_Filter, SYSMENU_HANDL_AUDIO_SSB_HPF_pass},
		{"SSB LPF Pass", SYSMENU_UINT16, (uint32_t *)&TRX.SSB_LPF_Filter, SYSMENU_HANDL_AUDIO_SSB_LPF_pass},
		{"CW HPF Pass", SYSMENU_UINT16, (uint32_t *)&TRX.CW_HPF_Filter, SYSMENU_HANDL_AUDIO_CW_HPF_pass},
		{"CW LPF Pass", SYSMENU_UINT16, (uint32_t *)&TRX.CW_LPF_Filter, SYSMENU_HANDL_AUDIO_CW_LPF_pass},
		{"AM LPF Pass", SYSMENU_UINT16, (uint32_t *)&TRX.AM_LPF_Filter, SYSMENU_HANDL_AUDIO_AM_LPF_pass},
		{"FM LPF Pass", SYSMENU_UINT16, (uint32_t *)&TRX.FM_LPF_Filter, SYSMENU_HANDL_AUDIO_FM_LPF_pass},
		{"FM Squelch", SYSMENU_UINT8, (uint32_t *)&TRX.FM_SQL_threshold, SYSMENU_HANDL_AUDIO_FMSquelch},
		{"VAD Squelch", SYSMENU_BOOLEAN, (uint32_t *)&TRX.VAD_Squelch, SYSMENU_HANDL_AUDIO_VAD_Squelch},
		{"MIC EQ Low", SYSMENU_INT8, (uint32_t *)&TRX.MIC_EQ_LOW, SYSMENU_HANDL_AUDIO_MIC_EQ_LOW},
		{"MIC EQ Mid", SYSMENU_INT8, (uint32_t *)&TRX.MIC_EQ_MID, SYSMENU_HANDL_AUDIO_MIC_EQ_MID},
		{"MIC EQ High", SYSMENU_INT8, (uint32_t *)&TRX.MIC_EQ_HIG, SYSMENU_HANDL_AUDIO_MIC_EQ_HIG},
		{"MIC Reverber", SYSMENU_UINT8, (uint32_t *)&TRX.MIC_REVERBER, SYSMENU_HANDL_AUDIO_MIC_REVERBER},
		{"RX EQ Low", SYSMENU_INT8, (uint32_t *)&TRX.RX_EQ_LOW, SYSMENU_HANDL_AUDIO_RX_EQ_LOW},
		{"RX EQ Mid", SYSMENU_INT8, (uint32_t *)&TRX.RX_EQ_MID, SYSMENU_HANDL_AUDIO_RX_EQ_MID},
		{"RX EQ High", SYSMENU_INT8, (uint32_t *)&TRX.RX_EQ_HIG, SYSMENU_HANDL_AUDIO_RX_EQ_HIG},
		{"RX AGC SSB Speed", SYSMENU_UINT8, (uint32_t *)&TRX.RX_AGC_SSB_speed, SYSMENU_HANDL_AUDIO_RX_AGC_SSB_Speed},
		{"RX AGC CW Speed", SYSMENU_UINT8, (uint32_t *)&TRX.RX_AGC_CW_speed, SYSMENU_HANDL_AUDIO_RX_AGC_CW_Speed},
		{"TX AGC Speed", SYSMENU_UINT8, (uint32_t *)&TRX.TX_AGC_speed, SYSMENU_HANDL_AUDIO_TX_AGCSpeed},
		{"Beeper", SYSMENU_UINT8, (uint32_t *)&TRX.Beeper, SYSMENU_HANDL_AUDIO_Beeper},
};
static uint8_t sysmenu_audio_item_count = sizeof(sysmenu_audio_handlers) / sizeof(sysmenu_audio_handlers[0]);

IRAM2 static struct sysmenu_item_handler sysmenu_cw_handlers[] =
	{
		{"CW Key timeout", SYSMENU_UINT16, (uint32_t *)&TRX.CW_Key_timeout, SYSMENU_HANDL_CW_Key_timeout},
		{"CW Generator shift", SYSMENU_UINT16, (uint32_t *)&TRX.CW_GENERATOR_SHIFT_HZ, SYSMENU_HANDL_CW_GENERATOR_SHIFT_HZ},
		{"CW Self Hear", SYSMENU_BOOLEAN, (uint32_t *)&TRX.CW_SelfHear, SYSMENU_HANDL_CW_SelfHear},
		{"CW Keyer", SYSMENU_BOOLEAN, (uint32_t *)&TRX.CW_KEYER, SYSMENU_HANDL_CW_Keyer},
		{"CW Keyer WPM", SYSMENU_UINT8, (uint32_t *)&TRX.CW_KEYER_WPM, SYSMENU_HANDL_CW_Keyer_WPM},
		{"CW Decoder", SYSMENU_BOOLEAN, (uint32_t *)&TRX.CWDecoder, SYSMENU_HANDL_CW_Decoder},
};
static uint8_t sysmenu_cw_item_count = sizeof(sysmenu_cw_handlers) / sizeof(sysmenu_cw_handlers[0]);

IRAM2 static struct sysmenu_item_handler sysmenu_screen_handlers[] =
	{
		{"FFT Zoom", SYSMENU_UINT8, (uint32_t *)&TRX.FFT_Zoom, SYSMENU_HANDL_SCREEN_FFT_Zoom},
		{"Color Theme", SYSMENU_UINT8, (uint32_t *)&TRX.ColorThemeId, SYSMENU_HANDL_SCREEN_COLOR_THEME},
		{"Layout Theme", SYSMENU_UINT8, (uint32_t *)&TRX.LayoutThemeId, SYSMENU_HANDL_SCREEN_LAYOUT_THEME},
		{"FFT Speed", SYSMENU_UINT8, (uint32_t *)&TRX.FFT_Speed, SYSMENU_HANDL_SCREEN_FFT_Speed},
		{"FFT Height", SYSMENU_UINT8, (uint32_t *)&TRX.FFT_Height, SYSMENU_HANDL_SCREEN_FFT_Height},
		{"FFT Color", SYSMENU_UINT8, (uint32_t *)&TRX.FFT_Color, SYSMENU_HANDL_SCREEN_FFT_Color},
		{"FFT Grid", SYSMENU_UINT8, (uint32_t *)&TRX.FFT_Grid, SYSMENU_HANDL_SCREEN_FFT_Grid},
		{"FFT Background", SYSMENU_BOOLEAN, (uint32_t *)&TRX.FFT_Background, SYSMENU_HANDL_SCREEN_FFT_Background},
		{"FFT Enabled", SYSMENU_BOOLEAN, (uint32_t *)&TRX.FFT_Enabled, SYSMENU_HANDL_SCREEN_FFT_Enabled},
		{"FFT Compressor", SYSMENU_BOOLEAN, (uint32_t *)&TRX.FFT_Compressor, SYSMENU_HANDL_SCREEN_FFT_Compressor},
		{"FFT Averaging", SYSMENU_UINT8, (uint32_t *)&TRX.FFT_Averaging, SYSMENU_HANDL_SCREEN_FFT_Averaging},
		{"FFT Window", SYSMENU_UINT8, (uint32_t *)&TRX.FFT_Window, SYSMENU_HANDL_SCREEN_FFT_Window},
};
static uint8_t sysmenu_screen_item_count = sizeof(sysmenu_screen_handlers) / sizeof(sysmenu_screen_handlers[0]);

IRAM2 static struct sysmenu_item_handler sysmenu_adc_handlers[] =
	{
		{"ADC Driver", SYSMENU_BOOLEAN, (uint32_t *)&TRX.ADC_Driver, SYSMENU_HANDL_ADC_DRIVER},
		{"ADC Preamp", SYSMENU_BOOLEAN, (uint32_t *)&TRX.ADC_PGA, SYSMENU_HANDL_ADC_PGA},
		{"ADC Dither", SYSMENU_BOOLEAN, (uint32_t *)&TRX.ADC_DITH, SYSMENU_HANDL_ADC_DITH},
		{"ADC Randomizer", SYSMENU_BOOLEAN, (uint32_t *)&TRX.ADC_RAND, SYSMENU_HANDL_ADC_RAND},
		{"ADC Shutdown", SYSMENU_BOOLEAN, (uint32_t *)&TRX.ADC_SHDN, SYSMENU_HANDL_ADC_SHDN},
};
static uint8_t sysmenu_adc_item_count = sizeof(sysmenu_adc_handlers) / sizeof(sysmenu_adc_handlers[0]);

IRAM2 static struct sysmenu_item_handler sysmenu_wifi_handlers[] =
	{
		{"WIFI Enabled", SYSMENU_BOOLEAN, (uint32_t *)&TRX.WIFI_Enabled, SYSMENU_HANDL_WIFI_Enabled},
		{"WIFI Select AP", SYSMENU_RUN, 0, SYSMENU_HANDL_WIFI_SelectAP},
		{"WIFI Set AP Pass", SYSMENU_RUN, 0, SYSMENU_HANDL_WIFI_SetAPpassword},
		{"WIFI Timezone", SYSMENU_INT8, (uint32_t *)&TRX.WIFI_TIMEZONE, SYSMENU_HANDL_WIFI_Timezone},
		{"WIFI CAT Server", SYSMENU_BOOLEAN, (uint32_t *)&TRX.WIFI_CAT_SERVER, SYSMENU_HANDL_WIFI_CAT_Server},
		//{"WIFI Update firmware", SYSMENU_RUN, 0, SYSMENU_HANDL_WIFI_UpdateFW},
		{"", SYSMENU_INFOLINE, 0, 0},
		{"IP:", SYSMENU_INFOLINE, 0, 0},
		{WIFI_IP, SYSMENU_INFOLINE, 0, 0},
};
static uint8_t sysmenu_wifi_item_count = sizeof(sysmenu_wifi_handlers) / sizeof(sysmenu_wifi_handlers[0]);

IRAM2 static struct sysmenu_item_handler sysmenu_sd_handlers[] =
	{
		{"Export Settings", SYSMENU_RUN, 0, SYSMENU_HANDL_SD_ExportSettings},
		{"Import Settings", SYSMENU_RUN, 0, SYSMENU_HANDL_SD_ImportSettings},
		{"Format SD card", SYSMENU_RUN, 0, SYSMENU_HANDL_SD_Format},
};
static uint8_t sysmenu_sd_item_count = sizeof(sysmenu_sd_handlers) / sizeof(sysmenu_sd_handlers[0]);

IRAM2 static struct sysmenu_item_handler sysmenu_spectrum_handlers[] =
	{
		{"Spectrum START", SYSMENU_RUN, 0, SYSMENU_HANDL_SPECTRUM_Start},
		{"Begin, kHz", SYSMENU_UINT32, (uint32_t *)&TRX.SPEC_Begin, SYSMENU_HANDL_SPECTRUM_Begin},
		{"End, kHz", SYSMENU_UINT32, (uint32_t *)&TRX.SPEC_End, SYSMENU_HANDL_SPECTRUM_End},
		{"Top, dBm", SYSMENU_INT16, (uint32_t *)&TRX.SPEC_TopDBM, SYSMENU_HANDL_SPECTRUM_TopDBM},
		{"Bottom, dBm", SYSMENU_INT16, (uint32_t *)&TRX.SPEC_BottomDBM, SYSMENU_HANDL_SPECTRUM_BottomDBM},
};
static uint8_t sysmenu_spectrum_item_count = sizeof(sysmenu_spectrum_handlers) / sizeof(sysmenu_spectrum_handlers[0]);

IRAM2 static struct sysmenu_item_handler sysmenu_calibration_handlers[] =
	{
		{"Encoder invert", SYSMENU_BOOLEAN, (uint32_t *)&CALIBRATE.ENCODER_INVERT, SYSMENU_HANDL_CALIB_ENCODER_INVERT},
		{"Encoder2 invert", SYSMENU_BOOLEAN, (uint32_t *)&CALIBRATE.ENCODER2_INVERT, SYSMENU_HANDL_CALIB_ENCODER2_INVERT},
		{"Encoder debounce", SYSMENU_UINT8, (uint32_t *)&CALIBRATE.ENCODER_DEBOUNCE, SYSMENU_HANDL_CALIB_ENCODER_DEBOUNCE},
		{"Encoder2 debounce", SYSMENU_UINT8, (uint32_t *)&CALIBRATE.ENCODER2_DEBOUNCE, SYSMENU_HANDL_CALIB_ENCODER2_DEBOUNCE},
		{"Encoder slow rate", SYSMENU_UINT8, (uint32_t *)&CALIBRATE.ENCODER_SLOW_RATE, SYSMENU_HANDL_CALIB_ENCODER_SLOW_RATE},
		{"Encoder on falling", SYSMENU_BOOLEAN, (uint32_t *)&CALIBRATE.ENCODER_ON_FALLING, SYSMENU_HANDL_CALIB_ENCODER_ON_FALLING},
		{"Encoder acceleration", SYSMENU_UINT8, (uint32_t *)&CALIBRATE.ENCODER_ACCELERATION, SYSMENU_HANDL_CALIB_ENCODER_ACCELERATION},
		{"CICCOMP Shift", SYSMENU_UINT8, (uint32_t *)&CALIBRATE.CICFIR_GAINER_val, SYSMENU_HANDL_CALIB_CICCOMP_SHIFT},
		{"TX CICCOMP Shift", SYSMENU_UINT8, (uint32_t *)&CALIBRATE.TXCICFIR_GAINER_val, SYSMENU_HANDL_CALIB_TXCICCOMP_SHIFT},
		{"DAC Shift", SYSMENU_UINT8, (uint32_t *)&CALIBRATE.DAC_GAINER_val, SYSMENU_HANDL_CALIB_DAC_SHIFT},
		{"RF GAIN <2mhz", SYSMENU_UINT8, (uint32_t *)&CALIBRATE.rf_out_power_up2mhz, SYSMENU_HANDL_CALIB_RF_GAIN_LF},	//-V641
		{"RF GAIN <5mhz", SYSMENU_UINT8, (uint32_t *)&CALIBRATE.rf_out_power_up5mhz, SYSMENU_HANDL_CALIB_RF_GAIN_HF_LOW},	//-V641
		{"RF GAIN <15mhz", SYSMENU_UINT8, (uint32_t *)&CALIBRATE.rf_out_power_up15mhz, SYSMENU_HANDL_CALIB_RF_GAIN_HF2},	//-V641
		{"RF GAIN <30mhz", SYSMENU_UINT8, (uint32_t *)&CALIBRATE.rf_out_power_up30mhz, SYSMENU_HANDL_CALIB_RF_GAIN_HF2},	//-V641
		{"RF GAIN <60mhz", SYSMENU_UINT8, (uint32_t *)&CALIBRATE.rf_out_power_up60mhz, SYSMENU_HANDL_CALIB_RF_GAIN_HF_HIGH},	//-V641
		{"RF GAIN VHF", SYSMENU_UINT8, (uint32_t *)&CALIBRATE.rf_out_power_vhf, SYSMENU_HANDL_CALIB_RF_GAIN_VHF},	//-V641
		{"S METER", SYSMENU_INT16, (uint32_t *)&CALIBRATE.smeter_calibration, SYSMENU_HANDL_CALIB_S_METER},
		{"ADC OFFSET", SYSMENU_INT16, (uint32_t *)&CALIBRATE.adc_offset, SYSMENU_HANDL_CALIB_ADC_OFFSET},
		{"LPF END", SYSMENU_UINT32, (uint32_t *)&CALIBRATE.LPF_END, SYSMENU_HANDL_CALIB_LPF_END},
		{"BPF 0 START", SYSMENU_UINT32, (uint32_t *)&CALIBRATE.BPF_0_START, SYSMENU_HANDL_CALIB_BPF_0_START},
		{"BPF 0 END", SYSMENU_UINT32, (uint32_t *)&CALIBRATE.BPF_0_END, SYSMENU_HANDL_CALIB_BPF_0_END},
		{"BPF 1 START", SYSMENU_UINT32, (uint32_t *)&CALIBRATE.BPF_1_START, SYSMENU_HANDL_CALIB_BPF_1_START},
		{"BPF 1 END", SYSMENU_UINT32, (uint32_t *)&CALIBRATE.BPF_1_END, SYSMENU_HANDL_CALIB_BPF_1_END},
		{"BPF 2 START", SYSMENU_UINT32, (uint32_t *)&CALIBRATE.BPF_2_START, SYSMENU_HANDL_CALIB_BPF_2_START},
		{"BPF 2 END", SYSMENU_UINT32, (uint32_t *)&CALIBRATE.BPF_2_END, SYSMENU_HANDL_CALIB_BPF_2_END},
		{"BPF 3 START", SYSMENU_UINT32, (uint32_t *)&CALIBRATE.BPF_3_START, SYSMENU_HANDL_CALIB_BPF_3_START},
		{"BPF 3 END", SYSMENU_UINT32, (uint32_t *)&CALIBRATE.BPF_3_END, SYSMENU_HANDL_CALIB_BPF_3_END},
		{"BPF 4 START", SYSMENU_UINT32, (uint32_t *)&CALIBRATE.BPF_4_START, SYSMENU_HANDL_CALIB_BPF_4_START},
		{"BPF 4 END", SYSMENU_UINT32, (uint32_t *)&CALIBRATE.BPF_4_END, SYSMENU_HANDL_CALIB_BPF_4_END},
		{"BPF 5 START", SYSMENU_UINT32, (uint32_t *)&CALIBRATE.BPF_5_START, SYSMENU_HANDL_CALIB_BPF_5_START},
		{"BPF 5 END", SYSMENU_UINT32, (uint32_t *)&CALIBRATE.BPF_5_END, SYSMENU_HANDL_CALIB_BPF_5_END},
		{"BPF 6 START", SYSMENU_UINT32, (uint32_t *)&CALIBRATE.BPF_6_START, SYSMENU_HANDL_CALIB_BPF_6_START},
		{"BPF 6 END", SYSMENU_UINT32, (uint32_t *)&CALIBRATE.BPF_6_END, SYSMENU_HANDL_CALIB_BPF_6_END},
		{"HPF START", SYSMENU_UINT32, (uint32_t *)&CALIBRATE.BPF_HPF, SYSMENU_HANDL_CALIB_HPF_START},
		{"SWR TRANS RATE", SYSMENU_FLOAT32, (uint32_t *)&CALIBRATE.swr_trans_rate, SYSMENU_HANDL_CALIB_SWR_TRANS_RATE},
		{"VCXO Freq", SYSMENU_INT8, (uint32_t *)&CALIBRATE.VCXO_correction, SYSMENU_HANDL_CALIB_VCXO},
		{"FW_AD8307_Slope (mv/dB)", SYSMENU_FLOAT32, (uint32_t *)&CALIBRATE.FW_AD8307_SLP, SYSMENU_HANDL_CALIB_FW_AD8307_SLP},
		{"FW_AD8307_Offset (mV)", SYSMENU_FLOAT32, (uint32_t *)&CALIBRATE.FW_AD8307_OFFS, SYSMENU_HANDL_CALIB_FW_AD8307_OFFS},
		{"BW_AD8307_Slope (mv/dB)", SYSMENU_FLOAT32, (uint32_t *)&CALIBRATE.BW_AD8307_SLP, SYSMENU_HANDL_CALIB_BW_AD8307_SLP},
		{"BW_AD8307_Offset (mV)", SYSMENU_FLOAT32, (uint32_t *)&CALIBRATE.BW_AD8307_OFFS, SYSMENU_HANDL_CALIB_BW_AD8307_OFFS},
};
static uint8_t sysmenu_calibration_item_count = sizeof(sysmenu_calibration_handlers) / sizeof(sysmenu_calibration_handlers[0]);

IRAM2 static struct sysmenu_item_handler sysmenu_services_handlers[] =
{
	{"Band SWR", SYSMENU_RUN, 0, SYSMENU_HANDL_SWR_BAND_START},
	{"HF SWR", SYSMENU_RUN, 0, SYSMENU_HANDL_SWR_HF_START},
	{"Spectrum Analyzer", SYSMENU_MENU, 0, SYSMENU_HANDL_SPECTRUMMENU},
	{"RDA Statistics", SYSMENU_RUN, 0, SYSMENU_HANDL_RDA_STATS},
	{"Propagination", SYSMENU_RUN, 0, SYSMENU_HANDL_PROPAGINATION},
	{"SWR Tandem Match Contr.", SYSMENU_RUN, 0, SYSMENU_HANDL_SWR_Tandem_Ctrl},		//Tisho
};
static uint8_t sysmenu_services_item_count = sizeof(sysmenu_services_handlers) / sizeof(sysmenu_services_handlers[0]); 

//COMMON MENU
static void drawSystemMenuElement(char *title, SystemMenuType type, uint32_t *value, bool onlyVal);
static void redrawCurrentItem(void);
static void SYSMENU_WIFI_DrawSelectAPMenu(bool full_redraw);
static void SYSMENU_WIFI_SelectAPMenuMove(int8_t dir);
static void SYSMENU_WIFI_DrawAPpasswordMenu(bool full_redraw);
static void SYSMENU_WIFI_RotatePasswordChar(int8_t dir);
static void SYSMENU_TRX_DrawCallsignMenu(bool full_redraw);
static void SYSMENU_TRX_RotateCallsignChar(int8_t dir);

static struct sysmenu_item_handler *sysmenu_handlers_selected = &sysmenu_handlers[0];
static uint8_t *sysmenu_item_count_selected = &sysmenu_item_count;
static uint8_t systemMenuIndex = 0;
static uint8_t systemMenuRootIndex = 0;
static uint16_t sysmenu_y = 5;
static uint8_t sysmenu_i = 0;
static bool sysmenu_onroot = true;
bool sysmenu_hiddenmenu_enabled = false;
static bool sysmenu_services_opened = false;
static bool sysmenu_infowindow_opened = false;

//WIFI
static bool sysmenu_wifi_selectap_menu_opened = false;
static bool sysmenu_wifi_setAPpassword_menu_opened = false;
static bool sysmenu_trx_setCallsign_menu_opened = false;
static uint16_t sysmenu_wifi_rescan_interval = 0;
static uint8_t sysmenu_wifi_selected_ap_index = 0;
static uint8_t sysmenu_wifi_selected_ap_password_char_index = 0;
static uint8_t sysmenu_trx_selected_callsign_char_index = 0;

//Time menu
static bool sysmenu_timeMenuOpened = false;
static uint8_t TimeMenuSelection = 0;

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"

//TRX MENU

static void SYSMENU_HANDL_TRXMENU(int8_t direction)
{
	sysmenu_handlers_selected = &sysmenu_trx_handlers[0];
	sysmenu_item_count_selected = &sysmenu_trx_item_count;
	sysmenu_onroot = false;
	systemMenuIndex = 0;
	drawSystemMenu(true);
}

void SYSMENU_TRX_RFPOWER_HOTKEY(void)
{
	sysmenu_handlers_selected = &sysmenu_trx_handlers[0];
	sysmenu_item_count_selected = &sysmenu_trx_item_count;
	sysmenu_onroot = false;
	systemMenuIndex = 0;
	drawSystemMenu(true);
}

void SYSMENU_TRX_STEP_HOTKEY(void)
{
	sysmenu_handlers_selected = &sysmenu_trx_handlers[0];
	sysmenu_item_count_selected = &sysmenu_trx_item_count;
	sysmenu_onroot = false;
	systemMenuIndex = 7;
	if (TRX.Fast)
		systemMenuIndex = 8;
	drawSystemMenu(true);
}

static void SYSMENU_HANDL_TRX_BandMap(int8_t direction)
{
	if (direction > 0)
		TRX.BandMapEnabled = true;
	if (direction < 0)
		TRX.BandMapEnabled = false;
}

static void SYSMENU_HANDL_TRX_AutoGain(int8_t direction)
{
	if (direction > 0)
		TRX.AutoGain = true;
	if (direction < 0)
		TRX.AutoGain = false;
	FPGA_NeedSendParams = true;
}

static void SYSMENU_HANDL_TRX_RFFilters(int8_t direction)
{
	if (direction > 0)
		TRX.RF_Filters = true;
	if (direction < 0)
		TRX.RF_Filters = false;
}

static void SYSMENU_HANDL_TRX_TWO_SIGNAL_TUNE(int8_t direction)
{
	if (direction > 0)
		TRX.TWO_SIGNAL_TUNE = true;
	if (direction < 0)
		TRX.TWO_SIGNAL_TUNE = false;
}

static void SYSMENU_HANDL_TRX_RFPower(int8_t direction)
{
	TRX.RF_Power += direction;
	if (TRX.RF_Power < 1)
		TRX.RF_Power = 1;
	if (TRX.RF_Power > 100)
		TRX.RF_Power = 100;
}

static void SYSMENU_HANDL_TRX_MICIN(int8_t direction)
{
	if (direction > 0)
		TRX.InputType_MIC = true;
	if (direction < 0)
		TRX.InputType_MIC = false;
	TRX.InputType_LINE = false;
	TRX.InputType_USB = false;
	drawSystemMenu(false);
	TRX_Restart_Mode();
}

static void SYSMENU_HANDL_TRX_LINEIN(int8_t direction)
{
	if (direction > 0)
		TRX.InputType_LINE = true;
	if (direction < 0)
		TRX.InputType_LINE = false;
	TRX.InputType_MIC = false;
	TRX.InputType_USB = false;
	drawSystemMenu(false);
	TRX_Restart_Mode();
}

static void SYSMENU_HANDL_TRX_USBIN(int8_t direction)
{
	if (direction > 0)
		TRX.InputType_USB = true;
	if (direction < 0)
		TRX.InputType_USB = false;
	TRX.InputType_MIC = false;
	TRX.InputType_LINE = false;
	drawSystemMenu(false);
	TRX_Restart_Mode();
}

static void SYSMENU_HANDL_TRX_DEBUG_CONSOLE(int8_t direction)
{
	if (direction > 0)
		TRX.Debug_Console = true;
	if (direction < 0)
		TRX.Debug_Console = false;
}

static void SYSMENU_HANDL_TRX_SHIFT_INTERVAL(int8_t direction)
{
	TRX.SHIFT_INTERVAL += direction * 100;
	if (TRX.SHIFT_INTERVAL < 100)
		TRX.SHIFT_INTERVAL = 100;
	if (TRX.SHIFT_INTERVAL > 10000)
		TRX.SHIFT_INTERVAL = 10000;
}

static void SYSMENU_HANDL_TRX_FRQ_STEP(int8_t direction)
{
	const uint16_t freq_steps[] = {1, 10, 25, 50, 100};
	for (uint8_t i = 0; i < ARRLENTH(freq_steps); i++)
		if (TRX.FRQ_STEP == freq_steps[i])
		{
			if (direction < 0)
			{
				if (i > 0)
					TRX.FRQ_STEP = freq_steps[i - 1];
				else
					TRX.FRQ_STEP = freq_steps[0];
				return;
			}
			else
			{
				if (i < (ARRLENTH(freq_steps) - 1))
					TRX.FRQ_STEP = freq_steps[i + 1];
				else
					TRX.FRQ_STEP = freq_steps[ARRLENTH(freq_steps) - 1];
				return;
			}
		}
	TRX.FRQ_STEP = freq_steps[0];
}

static void SYSMENU_HANDL_TRX_FRQ_FAST_STEP(int8_t direction)
{
	const uint16_t freq_steps[] = {10, 25, 50, 100, 500, 1000};
	for (uint8_t i = 0; i < ARRLENTH(freq_steps); i++)
		if (TRX.FRQ_FAST_STEP == freq_steps[i])
		{
			if (direction < 0)
			{
				if (i > 0)
					TRX.FRQ_FAST_STEP = freq_steps[i - 1];
				else
					TRX.FRQ_FAST_STEP = freq_steps[0];
				return;
			}
			else
			{
				if (i < (ARRLENTH(freq_steps) - 1))
					TRX.FRQ_FAST_STEP = freq_steps[i + 1];
				else
					TRX.FRQ_FAST_STEP = freq_steps[ARRLENTH(freq_steps) - 1];
				return;
			}
		}
	TRX.FRQ_FAST_STEP = freq_steps[0];
}

static void SYSMENU_HANDL_TRX_FRQ_ENC_STEP(int8_t direction)
{
	const uint16_t freq_steps[] = {1000, 5000, 25000, 50000};
	for (uint8_t i = 0; i < ARRLENTH(freq_steps); i++)
		if (TRX.FRQ_ENC_STEP == freq_steps[i])
		{
			if (direction < 0)
			{
				if (i > 0)
					TRX.FRQ_ENC_STEP = freq_steps[i - 1];
				else
					TRX.FRQ_ENC_STEP = freq_steps[0];
				return;
			}
			else
			{
				if (i < (ARRLENTH(freq_steps) - 1))
					TRX.FRQ_ENC_STEP = freq_steps[i + 1];
				else
					TRX.FRQ_ENC_STEP = freq_steps[ARRLENTH(freq_steps) - 1];
				return;
			}
		}
	TRX.FRQ_ENC_STEP = freq_steps[0];
}

static void SYSMENU_HANDL_TRX_FRQ_ENC_FAST_STEP(int8_t direction)
{
	const uint32_t freq_steps[] = {5000, 25000, 50000, 100000, 500000};
	for (uint8_t i = 0; i < ARRLENTH(freq_steps); i++)
		if (TRX.FRQ_ENC_FAST_STEP == freq_steps[i])
		{
			if (direction < 0)
			{
				if (i > 0)
					TRX.FRQ_ENC_FAST_STEP = freq_steps[i - 1];
				else
					TRX.FRQ_ENC_FAST_STEP = freq_steps[0];
				return;
			}
			else
			{
				if (i < (ARRLENTH(freq_steps) - 1))
					TRX.FRQ_ENC_FAST_STEP = freq_steps[i + 1];
				else
					TRX.FRQ_ENC_FAST_STEP = freq_steps[ARRLENTH(freq_steps) - 1];
				return;
			}
		}
	TRX.FRQ_ENC_FAST_STEP = freq_steps[0];
}

static void SYSMENU_HANDL_TRX_ENC_ACCELERATE(int8_t direction)
{
	if (direction > 0)
		TRX.Encoder_Accelerate = true;
	if (direction < 0)
		TRX.Encoder_Accelerate = false;
}

static void SYSMENU_HANDL_TRX_ATT_STEP(int8_t direction)
{
	TRX.ATT_STEP += direction;
	if (TRX.ATT_STEP < 1)
		TRX.ATT_STEP = 1;
	if (TRX.ATT_STEP > 15)
		TRX.ATT_STEP = 15;
}

static void SYSMENU_TRX_DrawCallsignMenu(bool full_redraw)
{
	if (full_redraw)
	{
		LCDDriver_Fill(BG_COLOR);
		LCDDriver_printText("CALLSIGN:", 5, 5, FG_COLOR, BG_COLOR, 2);
	}

	LCDDriver_printText(TRX.CALLSIGN, 10, 37, COLOR_GREEN, BG_COLOR, 2);
	LCDDriver_drawFastHLine(8 + sysmenu_trx_selected_callsign_char_index * 12, 54, 12, COLOR_RED);
}

static void SYSMENU_TRX_RotateCallsignChar(int8_t dir)
{
	bool full_redraw = false;
	if (TRX.CALLSIGN[sysmenu_trx_selected_callsign_char_index] == 0)
		full_redraw = true;
	TRX.CALLSIGN[sysmenu_trx_selected_callsign_char_index] += dir;

	// do not show special characters
	if (TRX.CALLSIGN[sysmenu_trx_selected_callsign_char_index] >= 1 && TRX.CALLSIGN[sysmenu_trx_selected_callsign_char_index] <= 32 && dir > 0)
		TRX.CALLSIGN[sysmenu_trx_selected_callsign_char_index] = 33;
	if (TRX.CALLSIGN[sysmenu_trx_selected_callsign_char_index] >= 1 && TRX.CALLSIGN[sysmenu_trx_selected_callsign_char_index] <= 32 && dir < 0)
		TRX.CALLSIGN[sysmenu_trx_selected_callsign_char_index] = 0;
	if (TRX.CALLSIGN[sysmenu_trx_selected_callsign_char_index] >= 127)
		TRX.CALLSIGN[sysmenu_trx_selected_callsign_char_index] = 0;
	if (TRX.CALLSIGN[sysmenu_trx_selected_callsign_char_index] == 0)
		full_redraw = true;

	if (full_redraw)
		SYSMENU_TRX_DrawCallsignMenu(true);
	else
		SYSMENU_TRX_DrawCallsignMenu(false);
}

static void SYSMENU_HANDL_TRX_SetCallsign(int8_t direction)
{
	sysmenu_trx_setCallsign_menu_opened = true;
	SYSMENU_TRX_DrawCallsignMenu(true);
	drawSystemMenu(true);
}

static void SYSMENU_HANDL_TRX_TRANSV_ENABLE(int8_t direction)
{
	if (direction > 0)
		TRX.Transverter_Enabled = true;
	if (direction < 0)
		TRX.Transverter_Enabled = false;
}

static void SYSMENU_HANDL_TRX_TRANSV_OFFSET(int8_t direction)
{
	TRX.Transverter_Offset_Mhz += direction;
	if (TRX.Transverter_Offset_Mhz < 1)
		TRX.Transverter_Offset_Mhz = 1;
	if (TRX.Transverter_Offset_Mhz > 500)
		TRX.Transverter_Offset_Mhz = 500;
}

//AUDIO MENU

static void SYSMENU_HANDL_AUDIOMENU(int8_t direction)
{
	sysmenu_handlers_selected = &sysmenu_audio_handlers[0];
	sysmenu_item_count_selected = &sysmenu_audio_item_count;
	sysmenu_onroot = false;
	systemMenuIndex = 0;
	drawSystemMenu(true);
}

void SYSMENU_AUDIO_BW_SSB_HOTKEY(void)
{
	sysmenu_handlers_selected = &sysmenu_audio_handlers[0];
	sysmenu_item_count_selected = &sysmenu_audio_item_count;
	sysmenu_onroot = false;
	systemMenuIndex = 7;
	drawSystemMenu(true);
}

void SYSMENU_AUDIO_BW_CW_HOTKEY(void)
{
	sysmenu_handlers_selected = &sysmenu_audio_handlers[0];
	sysmenu_item_count_selected = &sysmenu_audio_item_count;
	sysmenu_onroot = false;
	systemMenuIndex = 9;
	drawSystemMenu(true);
}

void SYSMENU_AUDIO_BW_AM_HOTKEY(void)
{
	sysmenu_handlers_selected = &sysmenu_audio_handlers[0];
	sysmenu_item_count_selected = &sysmenu_audio_item_count;
	sysmenu_onroot = false;
	systemMenuIndex = 10;
	drawSystemMenu(true);
}

void SYSMENU_AUDIO_BW_FM_HOTKEY(void)
{
	sysmenu_handlers_selected = &sysmenu_audio_handlers[0];
	sysmenu_item_count_selected = &sysmenu_audio_item_count;
	sysmenu_onroot = false;
	systemMenuIndex = 11;
	drawSystemMenu(true);
}

void SYSMENU_AUDIO_HPF_SSB_HOTKEY(void)
{
	sysmenu_handlers_selected = &sysmenu_audio_handlers[0];
	sysmenu_item_count_selected = &sysmenu_audio_item_count;
	sysmenu_onroot = false;
	systemMenuIndex = 6;
	drawSystemMenu(true);
}

void SYSMENU_AUDIO_HPF_CW_HOTKEY(void)
{
	sysmenu_handlers_selected = &sysmenu_audio_handlers[0];
	sysmenu_item_count_selected = &sysmenu_audio_item_count;
	systemMenuIndex = 8;
	drawSystemMenu(true);
}

void SYSMENU_AUDIO_SQUELCH_HOTKEY(void)
{
	sysmenu_handlers_selected = &sysmenu_audio_handlers[0];
	sysmenu_item_count_selected = &sysmenu_audio_item_count;
	sysmenu_onroot = false;
	systemMenuIndex = 12;
	drawSystemMenu(true);
}

void SYSMENU_AUDIO_AGC_HOTKEY(void)
{
	sysmenu_handlers_selected = &sysmenu_audio_handlers[0];
	sysmenu_item_count_selected = &sysmenu_audio_item_count;
	sysmenu_onroot = false;
	systemMenuIndex = 20;
	drawSystemMenu(true);
}

static void SYSMENU_HANDL_AUDIO_IFGain(int8_t direction)
{
	TRX.IF_Gain += direction;
	if (TRX.IF_Gain < 1)
		TRX.IF_Gain = 1;
	if (TRX.IF_Gain > 80)
		TRX.IF_Gain = 80;
}

static void SYSMENU_HANDL_AUDIO_AGC_GAIN_TARGET(int8_t direction)
{
	TRX.AGC_GAIN_TARGET += direction;
	if (TRX.AGC_GAIN_TARGET < -80)
		TRX.AGC_GAIN_TARGET = -80;
	if (TRX.AGC_GAIN_TARGET > -10)
		TRX.AGC_GAIN_TARGET = -10;
}

static void SYSMENU_HANDL_AUDIO_MIC_Gain(int8_t direction)
{
	TRX.MIC_GAIN += direction;
	if (TRX.MIC_GAIN < 1)
		TRX.MIC_GAIN = 1;
	if (TRX.MIC_GAIN > 20)
		TRX.MIC_GAIN = 20;
}

static void SYSMENU_HANDL_AUDIO_DNR_THRES(int8_t direction)
{
	TRX.DNR_SNR_THRESHOLD += direction;
	if (TRX.DNR_SNR_THRESHOLD < 1)
		TRX.DNR_SNR_THRESHOLD = 1;
	if (TRX.DNR_SNR_THRESHOLD > 100)
		TRX.DNR_SNR_THRESHOLD = 100;
}

static void SYSMENU_HANDL_AUDIO_DNR_AVERAGE(int8_t direction)
{
	TRX.DNR_AVERAGE += direction;
	if (TRX.DNR_AVERAGE < 1)
		TRX.DNR_AVERAGE = 1;
	if (TRX.DNR_AVERAGE > 100)
		TRX.DNR_AVERAGE = 100;
}

static void SYSMENU_HANDL_AUDIO_DNR_MINMAL(int8_t direction)
{
	TRX.DNR_MINIMAL += direction;
	if (TRX.DNR_MINIMAL < 1)
		TRX.DNR_MINIMAL = 1;
	if (TRX.DNR_MINIMAL > 100)
		TRX.DNR_MINIMAL = 100;
}

static void SYSMENU_HANDL_AUDIO_MIC_EQ_LOW(int8_t direction)
{
	TRX.MIC_EQ_LOW += direction;
	if (TRX.MIC_EQ_LOW < -10)
		TRX.MIC_EQ_LOW = -10;
	if (TRX.MIC_EQ_LOW > 10)
		TRX.MIC_EQ_LOW = 10;
	NeedReinitAudioFilters = true;
}

static void SYSMENU_HANDL_AUDIO_MIC_EQ_MID(int8_t direction)
{
	TRX.MIC_EQ_MID += direction;
	if (TRX.MIC_EQ_MID < -10)
		TRX.MIC_EQ_MID = -10;
	if (TRX.MIC_EQ_MID > 10)
		TRX.MIC_EQ_MID = 10;
	NeedReinitAudioFilters = true;
}

static void SYSMENU_HANDL_AUDIO_MIC_EQ_HIG(int8_t direction)
{
	TRX.MIC_EQ_HIG += direction;
	if (TRX.MIC_EQ_HIG < -10)
		TRX.MIC_EQ_HIG = -10;
	if (TRX.MIC_EQ_HIG > 10)
		TRX.MIC_EQ_HIG = 10;
	NeedReinitAudioFilters = true;
}

static void SYSMENU_HANDL_AUDIO_MIC_REVERBER(int8_t direction)
{
	if(direction > 0 || TRX.MIC_REVERBER > 0)
		TRX.MIC_REVERBER += direction;
	if (TRX.MIC_REVERBER > (AUDIO_MAX_REVERBER_TAPS - 1))
		TRX.MIC_REVERBER = (AUDIO_MAX_REVERBER_TAPS - 1);
	NeedReinitReverber = true;
}

static void SYSMENU_HANDL_AUDIO_RX_EQ_LOW(int8_t direction)
{
	TRX.RX_EQ_LOW += direction;
	if (TRX.RX_EQ_LOW < -10)
		TRX.RX_EQ_LOW = -10;
	if (TRX.RX_EQ_LOW > 10)
		TRX.RX_EQ_LOW = 10;
	NeedReinitAudioFilters = true;
}

static void SYSMENU_HANDL_AUDIO_RX_EQ_MID(int8_t direction)
{
	TRX.RX_EQ_MID += direction;
	if (TRX.RX_EQ_MID < -10)
		TRX.RX_EQ_MID = -10;
	if (TRX.RX_EQ_MID > 10)
		TRX.RX_EQ_MID = 10;
	NeedReinitAudioFilters = true;
}

static void SYSMENU_HANDL_AUDIO_RX_EQ_HIG(int8_t direction)
{
	TRX.RX_EQ_HIG += direction;
	if (TRX.RX_EQ_HIG < -10)
		TRX.RX_EQ_HIG = -10;
	if (TRX.RX_EQ_HIG > 10)
		TRX.RX_EQ_HIG = 10;
	NeedReinitAudioFilters = true;
}

static void SYSMENU_HANDL_AUDIO_RX_AGC_SSB_Speed(int8_t direction)
{
	TRX.RX_AGC_SSB_speed += direction;
	if (TRX.RX_AGC_SSB_speed < 1)
		TRX.RX_AGC_SSB_speed = 1;
	if (TRX.RX_AGC_SSB_speed > 20)
		TRX.RX_AGC_SSB_speed = 20;
}

static void SYSMENU_HANDL_AUDIO_RX_AGC_CW_Speed(int8_t direction)
{
	TRX.RX_AGC_CW_speed += direction;
	if (TRX.RX_AGC_CW_speed < 1)
		TRX.RX_AGC_CW_speed = 1;
	if (TRX.RX_AGC_CW_speed > 20)
		TRX.RX_AGC_CW_speed = 20;
}

static void SYSMENU_HANDL_AUDIO_TX_AGCSpeed(int8_t direction)
{
	TRX.TX_AGC_speed += direction;
	if (TRX.TX_AGC_speed < 1)
		TRX.TX_AGC_speed = 1;
	if (TRX.TX_AGC_speed > 10)
		TRX.TX_AGC_speed = 10;
}

static void SYSMENU_HANDL_AUDIO_FMSquelch(int8_t direction)
{
	if (direction < 0 && TRX.FM_SQL_threshold == 0)
		return;
	TRX.FM_SQL_threshold += direction;
	if (TRX.FM_SQL_threshold > 10)
		TRX.FM_SQL_threshold = 10;

	int8_t band = getBandFromFreq(CurrentVFO()->Freq, true);
	if (band > 0)
		TRX.BANDS_SAVED_SETTINGS[band].FM_SQL_threshold = TRX.FM_SQL_threshold;
}

static void SYSMENU_HANDL_AUDIO_VAD_Squelch(int8_t direction)
{
	if (direction > 0)
		TRX.VAD_Squelch = true;
	if (direction < 0)
		TRX.VAD_Squelch = false;
}

static void SYSMENU_HANDL_AUDIO_SSB_HPF_pass(int8_t direction)
{
	if (direction > 0)
	{
		if (TRX.SSB_HPF_Filter == 0)
			TRX.SSB_HPF_Filter = 60;
		else if (TRX.SSB_HPF_Filter == 60)
			TRX.SSB_HPF_Filter = 100;
		else if (TRX.SSB_HPF_Filter == 100)
			TRX.SSB_HPF_Filter = 200;
		else if (TRX.SSB_HPF_Filter == 200)
			TRX.SSB_HPF_Filter = 300;
		else if (TRX.SSB_HPF_Filter == 300)
			TRX.SSB_HPF_Filter = 400;
		else if (TRX.SSB_HPF_Filter == 400)
			TRX.SSB_HPF_Filter = 500;
	}
	else
	{
		if (TRX.SSB_HPF_Filter == 60)
			TRX.SSB_HPF_Filter = 0;
		else if (TRX.SSB_HPF_Filter == 100)
			TRX.SSB_HPF_Filter = 60;
		else if (TRX.SSB_HPF_Filter == 200)
			TRX.SSB_HPF_Filter = 100;
		else if (TRX.SSB_HPF_Filter == 300)
			TRX.SSB_HPF_Filter = 200;
		else if (TRX.SSB_HPF_Filter == 400)
			TRX.SSB_HPF_Filter = 300;
		else if (TRX.SSB_HPF_Filter == 500)
			TRX.SSB_HPF_Filter = 400;
	}
	NeedReinitAudioFilters = true;
}

static void SYSMENU_HANDL_AUDIO_CW_HPF_pass(int8_t direction)
{
	if (direction > 0)
	{
		if (TRX.CW_HPF_Filter == 0)
			TRX.CW_HPF_Filter = 60;
		else if (TRX.CW_HPF_Filter == 60)
			TRX.CW_HPF_Filter = 100;
	}
	else
	{
		if (TRX.CW_HPF_Filter == 60)
			TRX.CW_HPF_Filter = 0;
		else if (TRX.CW_HPF_Filter == 100)
			TRX.CW_HPF_Filter = 60;
	}
	NeedReinitAudioFilters = true;
}

static void SYSMENU_HANDL_AUDIO_CW_LPF_pass(int8_t direction)
{
	if (direction > 0)
	{
		if (TRX.CW_LPF_Filter == 300)
			TRX.CW_LPF_Filter = 500;
		else if (TRX.CW_LPF_Filter == 500)
			TRX.CW_LPF_Filter = 700;
		else if (TRX.CW_LPF_Filter == 700)
			TRX.CW_LPF_Filter = 1000;
		else if (TRX.CW_LPF_Filter == 1400)
			TRX.CW_LPF_Filter = 1600;
		else if (TRX.CW_LPF_Filter == 1600)
			TRX.CW_LPF_Filter = 1800;
		else if (TRX.CW_LPF_Filter == 1800)
			TRX.CW_LPF_Filter = 2100;
	}
	else
	{
		if (TRX.CW_LPF_Filter == 500)
			TRX.CW_LPF_Filter = 300;
		else if (TRX.CW_LPF_Filter == 700)
			TRX.CW_LPF_Filter = 500;
		else if (TRX.CW_LPF_Filter == 1000)
			TRX.CW_LPF_Filter = 700;
		else if (TRX.CW_LPF_Filter == 1400)
			TRX.CW_LPF_Filter = 1000;
		else if (TRX.CW_LPF_Filter == 1600)
			TRX.CW_LPF_Filter = 1400;
		else if (TRX.CW_LPF_Filter == 1800)
			TRX.CW_LPF_Filter = 1600;
		else if (TRX.CW_LPF_Filter == 2100)
			TRX.CW_LPF_Filter = 1800;
	}

	TRX_setMode(SecondaryVFO()->Mode, SecondaryVFO());
	TRX_setMode(CurrentVFO()->Mode, CurrentVFO());
}

static void SYSMENU_HANDL_AUDIO_SSB_LPF_pass(int8_t direction)
{
	if (direction > 0)
	{
		if (TRX.SSB_LPF_Filter == 0)
			TRX.SSB_LPF_Filter = 1400;

		if (TRX.SSB_LPF_Filter == 1400)
			TRX.SSB_LPF_Filter = 1600;
		else if (TRX.SSB_LPF_Filter == 1600)
			TRX.SSB_LPF_Filter = 1800;
		else if (TRX.SSB_LPF_Filter == 1800)
			TRX.SSB_LPF_Filter = 2100;
		else if (TRX.SSB_LPF_Filter == 2100)
			TRX.SSB_LPF_Filter = 2300;
		else if (TRX.SSB_LPF_Filter == 2300)
			TRX.SSB_LPF_Filter = 2500;
		else if (TRX.SSB_LPF_Filter == 2500)
			TRX.SSB_LPF_Filter = 2700;
		else if (TRX.SSB_LPF_Filter == 2700)
			TRX.SSB_LPF_Filter = 2900;
		else if (TRX.SSB_LPF_Filter == 2900)
			TRX.SSB_LPF_Filter = 3000;
		else if (TRX.SSB_LPF_Filter == 3000)
			TRX.SSB_LPF_Filter = 3200;
		else if (TRX.SSB_LPF_Filter == 3200)
			TRX.SSB_LPF_Filter = 3400;
	}
	else
	{
		if (TRX.SSB_LPF_Filter == 1600)
			TRX.SSB_LPF_Filter = 1400;
		else if (TRX.SSB_LPF_Filter == 1800)
			TRX.SSB_LPF_Filter = 1600;
		else if (TRX.SSB_LPF_Filter == 2100)
			TRX.SSB_LPF_Filter = 1800;
		else if (TRX.SSB_LPF_Filter == 2300)
			TRX.SSB_LPF_Filter = 2100;
		else if (TRX.SSB_LPF_Filter == 2500)
			TRX.SSB_LPF_Filter = 2300;
		else if (TRX.SSB_LPF_Filter == 2700)
			TRX.SSB_LPF_Filter = 2500;
		else if (TRX.SSB_LPF_Filter == 2900)
			TRX.SSB_LPF_Filter = 2700;
		else if (TRX.SSB_LPF_Filter == 3000)
			TRX.SSB_LPF_Filter = 2900;
		else if (TRX.SSB_LPF_Filter == 3200)
			TRX.SSB_LPF_Filter = 3000;
		else if (TRX.SSB_LPF_Filter == 3400)
			TRX.SSB_LPF_Filter = 3200;
	}

	TRX_setMode(SecondaryVFO()->Mode, SecondaryVFO());
	TRX_setMode(CurrentVFO()->Mode, CurrentVFO());
}

static void SYSMENU_HANDL_AUDIO_AM_LPF_pass(int8_t direction)
{
	if (direction > 0)
	{
		if (TRX.AM_LPF_Filter == 2100)
			TRX.AM_LPF_Filter = 2300;
		else if (TRX.AM_LPF_Filter == 2300)
			TRX.AM_LPF_Filter = 2500;
		else if (TRX.AM_LPF_Filter == 2500)
			TRX.AM_LPF_Filter = 2700;
		else if (TRX.AM_LPF_Filter == 2700)
			TRX.AM_LPF_Filter = 2900;
		else if (TRX.AM_LPF_Filter == 2900)
			TRX.AM_LPF_Filter = 3000;
		else if (TRX.AM_LPF_Filter == 3000)
			TRX.AM_LPF_Filter = 3200;
		else if (TRX.AM_LPF_Filter == 3200)
			TRX.AM_LPF_Filter = 3400;
		else if (TRX.AM_LPF_Filter == 3400)
			TRX.AM_LPF_Filter = 3600;
		else if (TRX.AM_LPF_Filter == 3600)
			TRX.AM_LPF_Filter = 3800;
		else if (TRX.AM_LPF_Filter == 3800)
			TRX.AM_LPF_Filter = 4000;
		else if (TRX.AM_LPF_Filter == 4000)
			TRX.AM_LPF_Filter = 4500;
		else if (TRX.AM_LPF_Filter == 4500)
			TRX.AM_LPF_Filter = 5000;
		else if (TRX.AM_LPF_Filter == 5000)
			TRX.AM_LPF_Filter = 6000;
		else if (TRX.AM_LPF_Filter == 6000)
			TRX.AM_LPF_Filter = 7000;
		else if (TRX.AM_LPF_Filter == 7000)
			TRX.AM_LPF_Filter = 8000;
		else if (TRX.AM_LPF_Filter == 8000)
			TRX.AM_LPF_Filter = 9000;
		else if (TRX.AM_LPF_Filter == 9000)
			TRX.AM_LPF_Filter = 10000;
	}
	else
	{
		if (TRX.AM_LPF_Filter == 2300)
			TRX.AM_LPF_Filter = 2100;
		else if (TRX.AM_LPF_Filter == 2500)
			TRX.AM_LPF_Filter = 2300;
		else if (TRX.AM_LPF_Filter == 2700)
			TRX.AM_LPF_Filter = 2500;
		else if (TRX.AM_LPF_Filter == 2900)
			TRX.AM_LPF_Filter = 2700;
		else if (TRX.AM_LPF_Filter == 3000)
			TRX.AM_LPF_Filter = 2900;
		else if (TRX.AM_LPF_Filter == 3200)
			TRX.AM_LPF_Filter = 3000;
		else if (TRX.AM_LPF_Filter == 3400)
			TRX.AM_LPF_Filter = 3200;
		else if (TRX.AM_LPF_Filter == 3600)
			TRX.AM_LPF_Filter = 3400;
		else if (TRX.AM_LPF_Filter == 3800)
			TRX.AM_LPF_Filter = 3400;
		else if (TRX.AM_LPF_Filter == 4000)
			TRX.AM_LPF_Filter = 3800;
		else if (TRX.AM_LPF_Filter == 4500)
			TRX.AM_LPF_Filter = 3800;
		else if (TRX.AM_LPF_Filter == 5000)
			TRX.AM_LPF_Filter = 4500;
		else if (TRX.AM_LPF_Filter == 6000)
			TRX.AM_LPF_Filter = 5000;
		else if (TRX.AM_LPF_Filter == 7000)
			TRX.AM_LPF_Filter = 6000;
		else if (TRX.AM_LPF_Filter == 8000)
			TRX.AM_LPF_Filter = 7000;
		else if (TRX.AM_LPF_Filter == 9000)
			TRX.AM_LPF_Filter = 8000;
		else if (TRX.AM_LPF_Filter == 10000)
			TRX.AM_LPF_Filter = 9000;
	}

	TRX_setMode(SecondaryVFO()->Mode, SecondaryVFO());
	TRX_setMode(CurrentVFO()->Mode, CurrentVFO());
}

static void SYSMENU_HANDL_AUDIO_FM_LPF_pass(int8_t direction)
{
	if (direction > 0)
	{
		if (TRX.FM_LPF_Filter == 5000)
			TRX.FM_LPF_Filter = 6000;
		else if (TRX.FM_LPF_Filter == 6000)
			TRX.FM_LPF_Filter = 7000;
		else if (TRX.FM_LPF_Filter == 7000)
			TRX.FM_LPF_Filter = 8000;
		else if (TRX.FM_LPF_Filter == 8000)
			TRX.FM_LPF_Filter = 9000;
		else if (TRX.FM_LPF_Filter == 9000)
			TRX.FM_LPF_Filter = 10000;
		else if (TRX.FM_LPF_Filter == 10000)
			TRX.FM_LPF_Filter = 15000;
		else if (TRX.FM_LPF_Filter == 15000)
			TRX.FM_LPF_Filter = 20000;
	}
	else
	{
		if (TRX.FM_LPF_Filter == 6000)
			TRX.FM_LPF_Filter = 5000;
		else if (TRX.FM_LPF_Filter == 7000)
			TRX.FM_LPF_Filter = 6000;
		else if (TRX.FM_LPF_Filter == 8000)
			TRX.FM_LPF_Filter = 7000;
		else if (TRX.FM_LPF_Filter == 9000)
			TRX.FM_LPF_Filter = 8000;
		else if (TRX.FM_LPF_Filter == 10000)
			TRX.FM_LPF_Filter = 9000;
		else if (TRX.FM_LPF_Filter == 15000)
			TRX.FM_LPF_Filter = 10000;
		else if (TRX.FM_LPF_Filter == 20000)
			TRX.FM_LPF_Filter = 15000;
	}

	TRX_setMode(SecondaryVFO()->Mode, SecondaryVFO());
	TRX_setMode(CurrentVFO()->Mode, CurrentVFO());
}

static void SYSMENU_HANDL_AUDIO_Beeper(int8_t direction)
{
	if (direction > 0)
		TRX.Beeper = true;
	if (direction < 0)
		TRX.Beeper = false;
}

//CW MENU

static void SYSMENU_HANDL_CWMENU(int8_t direction)
{
	sysmenu_handlers_selected = &sysmenu_cw_handlers[0];
	sysmenu_item_count_selected = &sysmenu_cw_item_count;
	sysmenu_onroot = false;
	systemMenuIndex = 0;
	drawSystemMenu(true);
}

void SYSMENU_CW_WPM_HOTKEY(void)
{
	sysmenu_handlers_selected = &sysmenu_cw_handlers[0];
	sysmenu_item_count_selected = &sysmenu_cw_item_count;
	sysmenu_onroot = false;
	systemMenuIndex = 4;
	drawSystemMenu(true);
}

void SYSMENU_CW_KEYER_HOTKEY(void)
{
	sysmenu_handlers_selected = &sysmenu_cw_handlers[0];
	sysmenu_item_count_selected = &sysmenu_cw_item_count;
	sysmenu_onroot = false;
	systemMenuIndex = 3;
	drawSystemMenu(true);
}

static void SYSMENU_HANDL_CW_Decoder(int8_t direction)
{
	if (direction > 0)
		TRX.CWDecoder = true;
	if (direction < 0)
		TRX.CWDecoder = false;
}

static void SYSMENU_HANDL_CW_GENERATOR_SHIFT_HZ(int8_t direction)
{
	TRX.CW_GENERATOR_SHIFT_HZ += direction * 100;
	if (TRX.CW_GENERATOR_SHIFT_HZ < 100)
		TRX.CW_GENERATOR_SHIFT_HZ = 100;
	if (TRX.CW_GENERATOR_SHIFT_HZ > 10000)
		TRX.CW_GENERATOR_SHIFT_HZ = 10000;
}

static void SYSMENU_HANDL_CW_Key_timeout(int8_t direction)
{
	if (TRX.CW_Key_timeout > 0 || direction > 0)
		TRX.CW_Key_timeout += direction * 50;
	if (TRX.CW_Key_timeout > 5000)
		TRX.CW_Key_timeout = 5000;
}

static void SYSMENU_HANDL_CW_Keyer(int8_t direction)
{
	if (direction > 0)
		TRX.CW_KEYER = true;
	if (direction < 0)
		TRX.CW_KEYER = false;
}

static void SYSMENU_HANDL_CW_Keyer_WPM(int8_t direction)
{
	TRX.CW_KEYER_WPM += direction;
	if (TRX.CW_KEYER_WPM < 1)
		TRX.CW_KEYER_WPM = 1;
	if (TRX.CW_KEYER_WPM > 200)
		TRX.CW_KEYER_WPM = 200;
}

static void SYSMENU_HANDL_CW_SelfHear(int8_t direction)
{
	if (direction > 0)
		TRX.CW_SelfHear = true;
	if (direction < 0)
		TRX.CW_SelfHear = false;
}

//SCREEN MENU

static void SYSMENU_HANDL_LCDMENU(int8_t direction)
{
	sysmenu_handlers_selected = &sysmenu_screen_handlers[0];
	sysmenu_item_count_selected = &sysmenu_screen_item_count;
	sysmenu_onroot = false;
	systemMenuIndex = 0;
	drawSystemMenu(true);
}

static void SYSMENU_HANDL_SCREEN_FFT_Enabled(int8_t direction)
{
	if (direction > 0)
		TRX.FFT_Enabled = true;
	if (direction < 0)
		TRX.FFT_Enabled = false;
}

static void SYSMENU_HANDL_SCREEN_COLOR_THEME(int8_t direction)
{
	if(direction > 0 || TRX.ColorThemeId > 0)
		TRX.ColorThemeId += direction;
	if (TRX.ColorThemeId > (COLOR_THEMES_COUNT - 1))
		TRX.ColorThemeId = (COLOR_THEMES_COUNT - 1);
	
	COLOR = &COLOR_THEMES[TRX.ColorThemeId];
	FFT_Init();
	LCD_redraw();
}

static void SYSMENU_HANDL_SCREEN_LAYOUT_THEME(int8_t direction)
{
	if(direction > 0 || TRX.LayoutThemeId > 0)
		TRX.LayoutThemeId += direction;
	if (TRX.LayoutThemeId > (LAYOUT_THEMES_COUNT - 1))
		TRX.LayoutThemeId = (LAYOUT_THEMES_COUNT - 1);
	
	LAYOUT = &LAYOUT_THEMES[TRX.LayoutThemeId];
	FFT_Init();
	LCD_redraw();
}

static void SYSMENU_HANDL_SCREEN_FFT_Compressor(int8_t direction)
{
	if (direction > 0)
		TRX.FFT_Compressor = true;
	if (direction < 0)
		TRX.FFT_Compressor = false;
}

static void SYSMENU_HANDL_SCREEN_FFT_Averaging(int8_t direction)
{
	TRX.FFT_Averaging += direction;
	if (TRX.FFT_Averaging < 1)
		TRX.FFT_Averaging = 1;
	if (TRX.FFT_Averaging > 10)
		TRX.FFT_Averaging = 10;
}

static void SYSMENU_HANDL_SCREEN_FFT_Window(int8_t direction)
{
	TRX.FFT_Window += direction;
	if (TRX.FFT_Window < 1)
		TRX.FFT_Window = 1;
	if (TRX.FFT_Window > 3)
		TRX.FFT_Window = 3;
	FFT_Init();
}

static void SYSMENU_HANDL_SCREEN_FFT_Zoom(int8_t direction)
{
	if (direction > 0)
	{
		if (TRX.FFT_Zoom == 1)
			TRX.FFT_Zoom = 2;
		else if (TRX.FFT_Zoom == 2)
			TRX.FFT_Zoom = 4;
		else if (TRX.FFT_Zoom == 4)
			TRX.FFT_Zoom = 8;
		else if (TRX.FFT_Zoom == 8)
			TRX.FFT_Zoom = 16;
	}
	else
	{
		if (TRX.FFT_Zoom == 2)
			TRX.FFT_Zoom = 1;
		else if (TRX.FFT_Zoom == 4)
			TRX.FFT_Zoom = 2;
		else if (TRX.FFT_Zoom == 8)
			TRX.FFT_Zoom = 4;
		else if (TRX.FFT_Zoom == 16)
			TRX.FFT_Zoom = 8;
	}
	FFT_Init();
}

static void SYSMENU_HANDL_SCREEN_FFT_Height(int8_t direction)
{
	TRX.FFT_Height += direction;
	if (TRX.FFT_Height < 1)
		TRX.FFT_Height = 1;
	if (TRX.FFT_Height > 3)
		TRX.FFT_Height = 3;
	FFT_Init();
}

static void SYSMENU_HANDL_SCREEN_FFT_Color(int8_t direction)
{
	TRX.FFT_Color += direction;
	if (TRX.FFT_Color < 1)
		TRX.FFT_Color = 1;
	if (TRX.FFT_Color > 7)
		TRX.FFT_Color = 7;
	FFT_Init();
}

static void SYSMENU_HANDL_SCREEN_FFT_Grid(int8_t direction)
{
	TRX.FFT_Grid += direction;
	if (TRX.FFT_Grid < 0)
		TRX.FFT_Grid = 0;
	if (TRX.FFT_Grid > 3)
		TRX.FFT_Grid = 3;
	FFT_Init();
}

static void SYSMENU_HANDL_SCREEN_FFT_Background(int8_t direction)
{
	if (direction > 0)
		TRX.FFT_Background = true;
	if (direction < 0)
		TRX.FFT_Background = false;
}

static void SYSMENU_HANDL_SCREEN_FFT_Speed(int8_t direction)
{
	TRX.FFT_Speed += direction;
	if (TRX.FFT_Speed < 1)
		TRX.FFT_Speed = 1;
	if (TRX.FFT_Speed > 5)
		TRX.FFT_Speed = 5;
}

//ADC/DAC MENU

static void SYSMENU_HANDL_ADCMENU(int8_t direction)
{
	sysmenu_handlers_selected = &sysmenu_adc_handlers[0];
	sysmenu_item_count_selected = &sysmenu_adc_item_count;
	sysmenu_onroot = false;
	systemMenuIndex = 0;
	drawSystemMenu(true);
}

static void SYSMENU_HANDL_ADC_DRIVER(int8_t direction)
{
	if (direction > 0)
		TRX.ADC_Driver = true;
	if (direction < 0)
		TRX.ADC_Driver = false;
	int8_t band = getBandFromFreq(CurrentVFO()->Freq, true);
	if (band > 0)
		TRX.BANDS_SAVED_SETTINGS[band].ADC_Driver = TRX.ADC_Driver;
	FPGA_NeedSendParams = true;
}

static void SYSMENU_HANDL_ADC_PGA(int8_t direction)
{
	if (direction > 0)
		TRX.ADC_PGA = true;
	if (direction < 0)
		TRX.ADC_PGA = false;
	int8_t band = getBandFromFreq(CurrentVFO()->Freq, true);
	if (band > 0)
		TRX.BANDS_SAVED_SETTINGS[band].ADC_PGA = TRX.ADC_PGA;
	FPGA_NeedSendParams = true;
}

static void SYSMENU_HANDL_ADC_RAND(int8_t direction)
{
	if (direction > 0)
		TRX.ADC_RAND = true;
	if (direction < 0)
		TRX.ADC_RAND = false;
	FPGA_NeedSendParams = true;
}

static void SYSMENU_HANDL_ADC_SHDN(int8_t direction)
{
	if (direction > 0)
		TRX.ADC_SHDN = true;
	if (direction < 0)
		TRX.ADC_SHDN = false;
	FPGA_NeedSendParams = true;
}

static void SYSMENU_HANDL_ADC_DITH(int8_t direction)
{
	if (direction > 0)
		TRX.ADC_DITH = true;
	if (direction < 0)
		TRX.ADC_DITH = false;
	FPGA_NeedSendParams = true;
}

//WIFI MENU

static void SYSMENU_HANDL_WIFIMENU(int8_t direction)
{
	sysmenu_handlers_selected = &sysmenu_wifi_handlers[0];
	sysmenu_item_count_selected = &sysmenu_wifi_item_count;
	sysmenu_onroot = false;
	systemMenuIndex = 0;
	drawSystemMenu(true);
}

static void SYSMENU_WIFI_DrawSelectAPMenuCallback(void)
{
	sysmenu_wifi_rescan_interval = 0;
}

static void SYSMENU_WIFI_DrawSelectAPMenu(bool full_redraw)
{
	if (full_redraw || sysmenu_wifi_rescan_interval == 0)
	{
		LCDDriver_Fill(BG_COLOR);
		LCDDriver_printText("AP Found:", 5, 5, FG_COLOR, BG_COLOR, 2);
		for (uint8_t i = 0; i < WIFI_FOUNDED_AP_MAXCOUNT; i++)
			LCDDriver_printText((char *)WIFI_FoundedAP[i], 10, 33 + i * 24, COLOR_GREEN, BG_COLOR, 2);
		LCDDriver_drawFastHLine(0, 49 + sysmenu_wifi_selected_ap_index * 24, LAYOUT->SYSMENU_W, FG_COLOR);
		WIFI_ListAP(SYSMENU_WIFI_DrawSelectAPMenuCallback);
	}
	sysmenu_wifi_rescan_interval++;
	if (sysmenu_wifi_rescan_interval > 30)
		sysmenu_wifi_rescan_interval = 0;
	LCD_UpdateQuery.SystemMenu = true;
}

static void SYSMENU_WIFI_SelectAPMenuMove(int8_t dir)
{
	if (dir < 0 && sysmenu_wifi_selected_ap_index > 0)
		sysmenu_wifi_selected_ap_index--;
	if (dir > 0 && sysmenu_wifi_selected_ap_index < WIFI_FOUNDED_AP_MAXCOUNT)
		sysmenu_wifi_selected_ap_index++;
	SYSMENU_WIFI_DrawSelectAPMenu(true);
	if (dir == 0)
	{
		strcpy(TRX.WIFI_AP, (char *)&WIFI_FoundedAP[sysmenu_wifi_selected_ap_index]);
		WIFI_InitStateIndex = 0;
		WIFI_State = WIFI_INITED;
		sysmenu_wifi_selectap_menu_opened = false;
		systemMenuIndex = 0;
		drawSystemMenu(true);
	}
}

static void SYSMENU_WIFI_DrawAPpasswordMenu(bool full_redraw)
{
	if (full_redraw)
	{
		LCDDriver_Fill(BG_COLOR);
		LCDDriver_printText("AP Password:", 5, 5, FG_COLOR, BG_COLOR, 2);
	}

	LCDDriver_printText(TRX.WIFI_PASSWORD, 10, 37, COLOR_GREEN, BG_COLOR, 2);
	LCDDriver_drawFastHLine(8 + sysmenu_wifi_selected_ap_password_char_index * 12, 54, 12, COLOR_RED);
}

static void SYSMENU_WIFI_RotatePasswordChar(int8_t dir)
{
	bool full_redraw = false;
	if (TRX.WIFI_PASSWORD[sysmenu_wifi_selected_ap_password_char_index] == 0)
		full_redraw = true;
	TRX.WIFI_PASSWORD[sysmenu_wifi_selected_ap_password_char_index] += dir;

	// do not show special characters
	if (TRX.WIFI_PASSWORD[sysmenu_wifi_selected_ap_password_char_index] >= 1 && TRX.WIFI_PASSWORD[sysmenu_wifi_selected_ap_password_char_index] <= 32 && dir > 0)
		TRX.WIFI_PASSWORD[sysmenu_wifi_selected_ap_password_char_index] = 33;
	if (TRX.WIFI_PASSWORD[sysmenu_wifi_selected_ap_password_char_index] >= 1 && TRX.WIFI_PASSWORD[sysmenu_wifi_selected_ap_password_char_index] <= 32 && dir < 0)
		TRX.WIFI_PASSWORD[sysmenu_wifi_selected_ap_password_char_index] = 0;
	if (TRX.WIFI_PASSWORD[sysmenu_wifi_selected_ap_password_char_index] >= 127)
		TRX.WIFI_PASSWORD[sysmenu_wifi_selected_ap_password_char_index] = 0;
	if (TRX.WIFI_PASSWORD[sysmenu_wifi_selected_ap_password_char_index] == 0)
		full_redraw = true;

	if (full_redraw)
		SYSMENU_WIFI_DrawAPpasswordMenu(true);
	else
		SYSMENU_WIFI_DrawAPpasswordMenu(false);
}

static void SYSMENU_HANDL_WIFI_Enabled(int8_t direction)
{
	if (direction > 0)
		TRX.WIFI_Enabled = true;
	if (direction < 0)
		TRX.WIFI_Enabled = false;
}

static void SYSMENU_HANDL_WIFI_SelectAP(int8_t direction)
{
	sysmenu_wifi_selectap_menu_opened = true;
	SYSMENU_WIFI_DrawSelectAPMenu(true);
	drawSystemMenu(true);
}

static void SYSMENU_HANDL_WIFI_SetAPpassword(int8_t direction)
{
	sysmenu_wifi_setAPpassword_menu_opened = true;
	SYSMENU_WIFI_DrawAPpasswordMenu(true);
	drawSystemMenu(true);
}

static void SYSMENU_HANDL_WIFI_Timezone(int8_t direction)
{
	TRX.WIFI_TIMEZONE += direction;
	if (TRX.WIFI_TIMEZONE < -12)
		TRX.WIFI_TIMEZONE = -12;
	if (TRX.WIFI_TIMEZONE > 12)
		TRX.WIFI_TIMEZONE = 12;
	WIFI_State = WIFI_INITED;
}

static void SYSMENU_HANDL_WIFI_CAT_Server(int8_t direction)
{
	if (direction > 0)
		TRX.WIFI_CAT_SERVER = true;
	if (direction < 0)
		TRX.WIFI_CAT_SERVER = false;
}

/*static void SYSMENU_HANDL_WIFI_UpdateFW(int8_t direction)
{
	WIFI_UpdateFW(NULL);
}*/

//SD MENU

static void SYSMENU_HANDL_SDMENU(int8_t direction)
{
	sysmenu_handlers_selected = &sysmenu_sd_handlers[0];
	sysmenu_item_count_selected = &sysmenu_sd_item_count;
	sysmenu_onroot = false;
	systemMenuIndex = 0;
	drawSystemMenu(true);
}

static void SYSMENU_HANDL_SD_ExportSettings(int8_t direction)
{
	if (direction > 0 && SD_isIdle() && !LCD_busy)
	{
		SD_doCommand(SDCOMM_EXPORT_SETTINGS);
	}
}

static void SYSMENU_HANDL_SD_ImportSettings(int8_t direction)
{
	if (direction > 0 && SD_isIdle() && !LCD_busy)
	{
		SD_doCommand(SDCOMM_IMPORT_SETTINGS);
	}
}

static void SYSMENU_HANDL_SD_Format(int8_t direction)
{
	if (direction > 0 && SD_isIdle() && !LCD_busy)
	{
		SD_doCommand(SDCOMM_FORMAT);
	}
}

//SET TIME MENU

static void SYSMENU_HANDL_SETTIME(int8_t direction)
{
	if (!sysmenu_timeMenuOpened)
		LCDDriver_Fill(BG_COLOR);
	sysmenu_timeMenuOpened = true;
	static uint8_t Hours;
	static uint8_t Minutes;
	static uint8_t Seconds;
	static uint32_t Time;
	char ctmp[50];
	Time = RTC->TR;
	Hours = ((Time >> 20) & 0x03) * 10 + ((Time >> 16) & 0x0f);
	Minutes = ((Time >> 12) & 0x07) * 10 + ((Time >> 8) & 0x0f);
	Seconds = ((Time >> 4) & 0x07) * 10 + ((Time >> 0) & 0x0f);
	if (direction != 0)
	{
		if (TimeMenuSelection == 0)
		{
			if (Hours == 0 && direction < 0)
				return;
			Hours = (uint8_t)(Hours + direction);
		}
		if (TimeMenuSelection == 1)
		{
			if (Minutes == 0 && direction < 0)
				return;
			Minutes = (uint8_t)(Minutes + direction);
		}
		if (TimeMenuSelection == 2)
		{
			if (Seconds == 0 && direction < 0)
				return;
			Seconds = (uint8_t)(Seconds + direction);
		}
		if (Hours >= 24)
			Hours = 0;
		if (Minutes >= 60)
			Minutes = 0;
		if (Seconds >= 60)
			Seconds = 0;
		RTC_TimeTypeDef sTime;
		sTime.TimeFormat = RTC_HOURFORMAT12_PM;
		sTime.SubSeconds = 0;
		sTime.SecondFraction = 0;
		sTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
		sTime.StoreOperation = RTC_STOREOPERATION_SET;
		sTime.Hours = Hours;
		sTime.Minutes = Minutes;
		sTime.Seconds = Seconds;
		BKPSRAM_Enable();
		HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
	}
	sprintf(ctmp, "%d", Hours);
	addSymbols(ctmp, ctmp, 2, "0", false);
	LCDDriver_printText(ctmp, 76, 100, COLOR->BUTTON_TEXT, TimeMenuSelection == 0 ? FG_COLOR : BG_COLOR, 3);
	LCDDriver_printText(":", 124, 100, COLOR->BUTTON_TEXT, BG_COLOR, 3);
	sprintf(ctmp, "%d", Minutes);
	addSymbols(ctmp, ctmp, 2, "0", false);
	LCDDriver_printText(ctmp, 148, 100, COLOR->BUTTON_TEXT, TimeMenuSelection == 1 ? FG_COLOR : BG_COLOR, 3);
	LCDDriver_printText(":", 194, 100, COLOR->BUTTON_TEXT, BG_COLOR, 3);
	sprintf(ctmp, "%d", Seconds);
	addSymbols(ctmp, ctmp, 2, "0", false);
	LCDDriver_printText(ctmp, 220, 100, COLOR->BUTTON_TEXT, TimeMenuSelection == 2 ? FG_COLOR : BG_COLOR, 3);
}

//FLASH MENU

static void SYSMENU_HANDL_Bootloader(int8_t direction)
{
	WM8731_CleanBuffer();
	TRX_NeedGoToBootloader = true;
	TRX_Inited = false;
	LCD_busy = true;
}

#pragma GCC diagnostic ignored "-Wsign-conversion"

//CALIBRATION MENU

static void SYSMENU_HANDL_CALIB_ENCODER_INVERT(int8_t direction)
{
	if (direction > 0)
		CALIBRATE.ENCODER_INVERT = true;
	if (direction < 0)
		CALIBRATE.ENCODER_INVERT = false;
}

static void SYSMENU_HANDL_CALIB_ENCODER2_INVERT(int8_t direction)
{
	if (direction > 0)
		CALIBRATE.ENCODER2_INVERT = true;
	if (direction < 0)
		CALIBRATE.ENCODER2_INVERT = false;
}

static void SYSMENU_HANDL_CALIB_ENCODER_DEBOUNCE(int8_t direction)
{
	if (CALIBRATE.ENCODER_DEBOUNCE > 0 || direction > 0)
		CALIBRATE.ENCODER_DEBOUNCE += direction;
	if (CALIBRATE.ENCODER_DEBOUNCE > 250)
		CALIBRATE.ENCODER_DEBOUNCE = 250;
}

static void SYSMENU_HANDL_CALIB_ENCODER2_DEBOUNCE(int8_t direction)
{
	if (CALIBRATE.ENCODER2_DEBOUNCE > 0 || direction > 0)
		CALIBRATE.ENCODER2_DEBOUNCE += direction;
	if (CALIBRATE.ENCODER2_DEBOUNCE > 250)
		CALIBRATE.ENCODER2_DEBOUNCE = 250;
}

static void SYSMENU_HANDL_CALIB_ENCODER_SLOW_RATE(int8_t direction)
{
	CALIBRATE.ENCODER_SLOW_RATE += direction;
	if (CALIBRATE.ENCODER_SLOW_RATE < 1)
		CALIBRATE.ENCODER_SLOW_RATE = 1;
	if (CALIBRATE.ENCODER_SLOW_RATE > 100)
		CALIBRATE.ENCODER_SLOW_RATE = 100;
}

static void SYSMENU_HANDL_CALIB_ENCODER_ON_FALLING(int8_t direction)
{
	if (direction > 0)
		CALIBRATE.ENCODER_ON_FALLING = true;
	if (direction < 0)
		CALIBRATE.ENCODER_ON_FALLING = false;
}

static void SYSMENU_HANDL_CALIB_ENCODER_ACCELERATION(int8_t direction)
{
	CALIBRATE.ENCODER_ACCELERATION += direction;
	if (CALIBRATE.ENCODER_ACCELERATION < 1)
		CALIBRATE.ENCODER_ACCELERATION = 1;
	if (CALIBRATE.ENCODER_ACCELERATION > 250)
		CALIBRATE.ENCODER_ACCELERATION = 250;
}

static void SYSMENU_HANDL_CALIBRATIONMENU(int8_t direction)
{
	if (!sysmenu_hiddenmenu_enabled)
		return;
	sysmenu_handlers_selected = &sysmenu_calibration_handlers[0];
	sysmenu_item_count_selected = &sysmenu_calibration_item_count;
	sysmenu_onroot = false;
	systemMenuIndex = 0;
	drawSystemMenu(true);
}

static void SYSMENU_HANDL_SWR_Tandem_Ctrl(int8_t direction)			//Tisho
{
	
	if (sysmenu_TDM_CTRL_opened)
	{
		
	}
	else
	{
		sysmenu_TDM_CTRL_opened = true;
		TDM_Voltages_Start();
//		drawSystemMenu(true);
	}
}
static void SYSMENU_HANDL_CALIB_CICCOMP_SHIFT(int8_t direction)
{
	CALIBRATE.CICFIR_GAINER_val += direction;
	if (CALIBRATE.CICFIR_GAINER_val < 32)
		CALIBRATE.CICFIR_GAINER_val = 32;
	if (CALIBRATE.CICFIR_GAINER_val > 64)
		CALIBRATE.CICFIR_GAINER_val = 64;
}

static void SYSMENU_HANDL_CALIB_TXCICCOMP_SHIFT(int8_t direction)
{
	CALIBRATE.TXCICFIR_GAINER_val += direction;
	if (CALIBRATE.TXCICFIR_GAINER_val < 16)
		CALIBRATE.TXCICFIR_GAINER_val = 16;
	if (CALIBRATE.TXCICFIR_GAINER_val > 64)
		CALIBRATE.TXCICFIR_GAINER_val = 64;
}

static void SYSMENU_HANDL_CALIB_DAC_SHIFT(int8_t direction)
{
	CALIBRATE.DAC_GAINER_val += direction;
	if (CALIBRATE.DAC_GAINER_val < 14)
		CALIBRATE.DAC_GAINER_val = 14;
	if (CALIBRATE.DAC_GAINER_val > 28)
		CALIBRATE.DAC_GAINER_val = 28;
}

static void SYSMENU_HANDL_CALIB_RF_GAIN_LF(int8_t direction)
{
	if (CALIBRATE.rf_out_power_up2mhz > 0)
		CALIBRATE.rf_out_power_up2mhz += direction;
	if (CALIBRATE.rf_out_power_up2mhz == 0 && direction > 0)
		CALIBRATE.rf_out_power_up2mhz += direction;
	if (CALIBRATE.rf_out_power_up2mhz > 100)
		CALIBRATE.rf_out_power_up2mhz = 100;
	
	TRX_MAX_TX_Amplitude = getMaxTXAmplitudeOnFreq(CurrentVFO()->Freq);
}

static void SYSMENU_HANDL_CALIB_RF_GAIN_HF_LOW(int8_t direction)
{
	if (CALIBRATE.rf_out_power_up5mhz > 0)
		CALIBRATE.rf_out_power_up5mhz += direction;
	if (CALIBRATE.rf_out_power_up5mhz == 0 && direction > 0)
		CALIBRATE.rf_out_power_up5mhz += direction;
	if (CALIBRATE.rf_out_power_up5mhz > 100)
		CALIBRATE.rf_out_power_up5mhz = 100;
	
	TRX_MAX_TX_Amplitude = getMaxTXAmplitudeOnFreq(CurrentVFO()->Freq);
}

static void SYSMENU_HANDL_CALIB_RF_GAIN_HF1(int8_t direction)
{
	if (CALIBRATE.rf_out_power_up15mhz > 0)
		CALIBRATE.rf_out_power_up15mhz += direction;
	if (CALIBRATE.rf_out_power_up15mhz == 0 && direction > 0)
		CALIBRATE.rf_out_power_up15mhz += direction;
	if (CALIBRATE.rf_out_power_up15mhz > 100)
		CALIBRATE.rf_out_power_up15mhz = 100;
	
	TRX_MAX_TX_Amplitude = getMaxTXAmplitudeOnFreq(CurrentVFO()->Freq);
}

static void SYSMENU_HANDL_CALIB_RF_GAIN_HF2(int8_t direction)
{
	if (CALIBRATE.rf_out_power_up30mhz > 0)
		CALIBRATE.rf_out_power_up30mhz += direction;
	if (CALIBRATE.rf_out_power_up30mhz == 0 && direction > 0)
		CALIBRATE.rf_out_power_up30mhz += direction;
	if (CALIBRATE.rf_out_power_up30mhz > 100)
		CALIBRATE.rf_out_power_up30mhz = 100;
	
	TRX_MAX_TX_Amplitude = getMaxTXAmplitudeOnFreq(CurrentVFO()->Freq);
}

static void SYSMENU_HANDL_CALIB_RF_GAIN_HF_HIGH(int8_t direction)
{
	if (CALIBRATE.rf_out_power_up60mhz > 0)
		CALIBRATE.rf_out_power_up60mhz += direction;
	if (CALIBRATE.rf_out_power_up60mhz == 0 && direction > 0)
		CALIBRATE.rf_out_power_up60mhz += direction;
	if (CALIBRATE.rf_out_power_up60mhz > 100)
		CALIBRATE.rf_out_power_up60mhz = 100;
	
	TRX_MAX_TX_Amplitude = getMaxTXAmplitudeOnFreq(CurrentVFO()->Freq);
}

static void SYSMENU_HANDL_CALIB_RF_GAIN_VHF(int8_t direction)
{
	if (CALIBRATE.rf_out_power_vhf > 0)
		CALIBRATE.rf_out_power_vhf += direction;
	if (CALIBRATE.rf_out_power_vhf == 0 && direction > 0)
		CALIBRATE.rf_out_power_vhf += direction;
	if (CALIBRATE.rf_out_power_vhf > 100)
		CALIBRATE.rf_out_power_vhf = 100;
	
	TRX_MAX_TX_Amplitude = getMaxTXAmplitudeOnFreq(CurrentVFO()->Freq);
}

static void SYSMENU_HANDL_CALIB_S_METER(int8_t direction)
{
	CALIBRATE.smeter_calibration += direction;
	if (CALIBRATE.smeter_calibration < -50)
		CALIBRATE.smeter_calibration = -50;
	if (CALIBRATE.smeter_calibration > 50)
		CALIBRATE.smeter_calibration = 50;
}

static void SYSMENU_HANDL_CALIB_ADC_OFFSET(int8_t direction)
{
	CALIBRATE.adc_offset += direction;
	if (CALIBRATE.adc_offset < -500)
		CALIBRATE.adc_offset = -500;
	if (CALIBRATE.adc_offset > 500)
		CALIBRATE.adc_offset = 500;
}

#pragma GCC diagnostic ignored "-Wsign-conversion"
static void SYSMENU_HANDL_CALIB_LPF_END(int8_t direction)
{
	CALIBRATE.LPF_END += direction * 100000;
	if (CALIBRATE.LPF_END < 1)
		CALIBRATE.LPF_END = 1;
	if (CALIBRATE.LPF_END > 999999999)
		CALIBRATE.LPF_END = 999999999;
}

static void SYSMENU_HANDL_CALIB_BPF_0_START(int8_t direction)
{
	CALIBRATE.BPF_0_START += direction * 100000;
	if (CALIBRATE.BPF_0_START < 1)
		CALIBRATE.BPF_0_START = 1;
	if (CALIBRATE.BPF_0_START > 999999999)
		CALIBRATE.BPF_0_START = 999999999;
}

static void SYSMENU_HANDL_CALIB_BPF_0_END(int8_t direction)
{
	CALIBRATE.BPF_0_END += direction * 100000;
	if (CALIBRATE.BPF_0_END < 1)
		CALIBRATE.BPF_0_END = 1;
	if (CALIBRATE.BPF_0_END > 999999999)
		CALIBRATE.BPF_0_END = 999999999;
}

static void SYSMENU_HANDL_CALIB_BPF_1_START(int8_t direction)
{
	CALIBRATE.BPF_1_START += direction * 100000;
	if (CALIBRATE.BPF_1_START < 1)
		CALIBRATE.BPF_1_START = 1;
	if (CALIBRATE.BPF_1_START > 999999999)
		CALIBRATE.BPF_1_START = 999999999;
}

static void SYSMENU_HANDL_CALIB_BPF_1_END(int8_t direction)
{
	CALIBRATE.BPF_1_END += direction * 100000;
	if (CALIBRATE.BPF_1_END < 1)
		CALIBRATE.BPF_1_END = 1;
	if (CALIBRATE.BPF_1_END > 999999999)
		CALIBRATE.BPF_1_END = 999999999;
}

static void SYSMENU_HANDL_CALIB_BPF_2_START(int8_t direction)
{
	CALIBRATE.BPF_2_START += direction * 100000;
	if (CALIBRATE.BPF_2_START < 1)
		CALIBRATE.BPF_2_START = 1;
	if (CALIBRATE.BPF_2_START > 999999999)
		CALIBRATE.BPF_2_START = 999999999;
}

static void SYSMENU_HANDL_CALIB_BPF_2_END(int8_t direction)
{
	CALIBRATE.BPF_2_END += direction * 100000;
	if (CALIBRATE.BPF_2_END < 1)
		CALIBRATE.BPF_2_END = 1;
	if (CALIBRATE.BPF_2_END > 999999999)
		CALIBRATE.BPF_2_END = 999999999;
}

static void SYSMENU_HANDL_CALIB_BPF_3_START(int8_t direction)
{
	CALIBRATE.BPF_3_START += direction * 100000;
	if (CALIBRATE.BPF_3_START < 1)
		CALIBRATE.BPF_3_START = 1;
	if (CALIBRATE.BPF_3_START > 999999999)
		CALIBRATE.BPF_3_START = 999999999;
}

static void SYSMENU_HANDL_CALIB_BPF_3_END(int8_t direction)
{
	CALIBRATE.BPF_3_END += direction * 100000;
	if (CALIBRATE.BPF_3_END < 1)
		CALIBRATE.BPF_3_END = 1;
	if (CALIBRATE.BPF_3_END > 999999999)
		CALIBRATE.BPF_3_END = 999999999;
}

static void SYSMENU_HANDL_CALIB_BPF_4_START(int8_t direction)
{
	CALIBRATE.BPF_4_START += direction * 100000;
	if (CALIBRATE.BPF_4_START < 1)
		CALIBRATE.BPF_4_START = 1;
	if (CALIBRATE.BPF_4_START > 999999999)
		CALIBRATE.BPF_4_START = 999999999;
}

static void SYSMENU_HANDL_CALIB_BPF_4_END(int8_t direction)
{
	CALIBRATE.BPF_4_END += direction * 100000;
	if (CALIBRATE.BPF_4_END < 1)
		CALIBRATE.BPF_4_END = 1;
	if (CALIBRATE.BPF_4_END > 999999999)
		CALIBRATE.BPF_4_END = 999999999;
}

static void SYSMENU_HANDL_CALIB_BPF_5_START(int8_t direction)
{
	CALIBRATE.BPF_5_START += direction * 100000;
	if (CALIBRATE.BPF_5_START < 1)
		CALIBRATE.BPF_5_START = 1;
	if (CALIBRATE.BPF_5_START > 999999999)
		CALIBRATE.BPF_5_START = 999999999;
}

static void SYSMENU_HANDL_CALIB_BPF_5_END(int8_t direction)
{
	CALIBRATE.BPF_5_END += direction * 100000;
	if (CALIBRATE.BPF_5_END < 1)
		CALIBRATE.BPF_5_END = 1;
	if (CALIBRATE.BPF_5_END > 999999999)
		CALIBRATE.BPF_5_END = 999999999;
}

static void SYSMENU_HANDL_CALIB_BPF_6_START(int8_t direction)
{
	CALIBRATE.BPF_6_START += direction * 100000;
	if (CALIBRATE.BPF_6_START < 1)
		CALIBRATE.BPF_6_START = 1;
	if (CALIBRATE.BPF_6_START > 999999999)
		CALIBRATE.BPF_6_START = 999999999;
}

static void SYSMENU_HANDL_CALIB_BPF_6_END(int8_t direction)
{
	CALIBRATE.BPF_6_END += direction * 100000;
	if (CALIBRATE.BPF_6_END < 1)
		CALIBRATE.BPF_6_END = 1;
	if (CALIBRATE.BPF_6_END > 999999999)
		CALIBRATE.BPF_6_END = 999999999;
}

static void SYSMENU_HANDL_CALIB_HPF_START(int8_t direction)
{
	CALIBRATE.BPF_HPF += direction * 100000;
	if (CALIBRATE.BPF_HPF < 1)
		CALIBRATE.BPF_HPF = 1;
	if (CALIBRATE.BPF_HPF > 999999999)
		CALIBRATE.BPF_HPF = 999999999;
}

static void SYSMENU_HANDL_CALIB_SWR_TRANS_RATE(int8_t direction)
{
	CALIBRATE.swr_trans_rate += (float32_t)direction * 0.1f;
	if (CALIBRATE.swr_trans_rate < 1.0f)
		CALIBRATE.swr_trans_rate = 1.0f;
	if (CALIBRATE.swr_trans_rate > 50.0f)
		CALIBRATE.swr_trans_rate = 50.0f;
}

static void SYSMENU_HANDL_CALIB_VCXO(int8_t direction)
{
	CALIBRATE.VCXO_correction += direction;
	if (CALIBRATE.VCXO_correction < -100)
		CALIBRATE.VCXO_correction = -100;
	if (CALIBRATE.VCXO_correction > 100)
		CALIBRATE.VCXO_correction = 100;
}
//Tisho
static void SYSMENU_HANDL_CALIB_FW_AD8307_SLP(int8_t direction)
{
	CALIBRATE.FW_AD8307_SLP += (float32_t)direction * 0.1f;
	if (CALIBRATE.FW_AD8307_SLP < 20.0f)
		CALIBRATE.FW_AD8307_SLP = 20.0f;
	if (CALIBRATE.FW_AD8307_SLP > 30.0f)
		CALIBRATE.FW_AD8307_SLP = 30.0f;
}

static void SYSMENU_HANDL_CALIB_FW_AD8307_OFFS(int8_t direction)
{
	CALIBRATE.FW_AD8307_OFFS += (float32_t)direction;
	if (CALIBRATE.FW_AD8307_OFFS < 0.1f)
		CALIBRATE.FW_AD8307_OFFS = 0.1f;
	if (CALIBRATE.FW_AD8307_OFFS > 4000.0f)
		CALIBRATE.FW_AD8307_OFFS = 4000.0f;
}

static void SYSMENU_HANDL_CALIB_BW_AD8307_SLP(int8_t direction)
{
	CALIBRATE.BW_AD8307_SLP += (float32_t)direction * 0.1f;
	if (CALIBRATE.BW_AD8307_SLP < 20.0f)
		CALIBRATE.BW_AD8307_SLP = 20.0f;
	if (CALIBRATE.BW_AD8307_SLP > 30.0f)
		CALIBRATE.BW_AD8307_SLP = 30.0f;
}

static void SYSMENU_HANDL_CALIB_BW_AD8307_OFFS(int8_t direction)
{
	CALIBRATE.BW_AD8307_OFFS += (float32_t)direction;
	if (CALIBRATE.BW_AD8307_OFFS < 0.1f)
		CALIBRATE.BW_AD8307_OFFS = 0.1f;
	if (CALIBRATE.BW_AD8307_OFFS > 4000.0f)
		CALIBRATE.BW_AD8307_OFFS = 4000.0f;
}
//end Tisho
//SERVICES
void SYSMENU_HANDL_SERVICESMENU(int8_t direction)
{
	sysmenu_services_opened = true;
	sysmenu_handlers_selected = &sysmenu_services_handlers[0];
	sysmenu_item_count_selected = &sysmenu_services_item_count;
	sysmenu_onroot = false;
	systemMenuIndex = 0;
	drawSystemMenu(true);
}

//SPECTRUM ANALIZER
static void SYSMENU_HANDL_SPECTRUMMENU(int8_t direction)
{
	sysmenu_handlers_selected = &sysmenu_spectrum_handlers[0];
	sysmenu_item_count_selected = &sysmenu_spectrum_item_count;
	sysmenu_onroot = false;
	systemMenuIndex = 0;
	drawSystemMenu(true);
}

static void SYSMENU_HANDL_SPECTRUM_Start(int8_t direction)
{
	if (sysmenu_spectrum_opened)
	{
		SPEC_EncRotate(direction);
	}
	else
	{
		sysmenu_spectrum_opened = true;
		SPEC_Start();
		drawSystemMenu(true);
	}
}

static void SYSMENU_HANDL_SPECTRUM_Begin(int8_t direction)
{
	TRX.SPEC_Begin += direction * 100;
	if (TRX.SPEC_Begin < 100)
		TRX.SPEC_Begin = 100;
}

static void SYSMENU_HANDL_SPECTRUM_End(int8_t direction)
{
	TRX.SPEC_End += direction * 100;
	if (TRX.SPEC_End < 100)
		TRX.SPEC_End = 100;
}

static void SYSMENU_HANDL_SPECTRUM_TopDBM(int8_t direction)
{
	TRX.SPEC_TopDBM += direction;
	if (TRX.SPEC_TopDBM < -140)
		TRX.SPEC_TopDBM = -140;
	if (TRX.SPEC_TopDBM > 40)
		TRX.SPEC_TopDBM = 40;
	if (TRX.SPEC_TopDBM <= TRX.SPEC_BottomDBM)
		TRX.SPEC_TopDBM = TRX.SPEC_BottomDBM + 1;
}

static void SYSMENU_HANDL_SPECTRUM_BottomDBM(int8_t direction)
{
	TRX.SPEC_BottomDBM += direction;
	if (TRX.SPEC_BottomDBM < -140)
		TRX.SPEC_BottomDBM = -140;
	if (TRX.SPEC_BottomDBM > 40)
		TRX.SPEC_BottomDBM = 40;
	if (TRX.SPEC_BottomDBM >= TRX.SPEC_TopDBM)
		TRX.SPEC_BottomDBM = TRX.SPEC_TopDBM - 1;
}

//SWR BAND ANALYZER
static void SYSMENU_HANDL_SWR_BAND_START(int8_t direction)
{
	if (sysmenu_swr_opened)
	{
		SWR_EncRotate(direction);
	}
	else
	{
		sysmenu_swr_opened = true;
		int8_t band = getBandFromFreq(CurrentVFO()->Freq, true);
		SWR_Start(BANDS[band].startFreq - 100000, BANDS[band].endFreq + 100000);
		drawSystemMenu(true);
	}
}

//SWR HF ANALYZER
static void SYSMENU_HANDL_SWR_HF_START(int8_t direction)
{
	if (sysmenu_swr_opened)
	{
		SWR_EncRotate(direction);
	}
	else
	{
		sysmenu_swr_opened = true;
		SWR_Start(1000000, 60000000);
		drawSystemMenu(true);
	}
}

//RDA STATS
static void SYSMENU_HANDL_RDA_STATS(int8_t direction)
{
	sysmenu_infowindow_opened = true;
	drawSystemMenu(true);
	WIFI_getRDA();
}

//PROPAGINATION
static void SYSMENU_HANDL_PROPAGINATION(int8_t direction)
{
	sysmenu_infowindow_opened = true;
	drawSystemMenu(true);
	WIFI_getPropagination();
}

//COMMON MENU FUNCTIONS
void drawSystemMenu(bool draw_background)
{
	if (LCD_busy)
	{
		LCD_UpdateQuery.SystemMenu = true;
		return;
	}
	if (!LCD_systemMenuOpened)
		return;
	if (sysmenu_timeMenuOpened)
	{
		SYSMENU_HANDL_SETTIME(0);
		return;
	}
	if (sysmenu_wifi_selectap_menu_opened)
	{
		SYSMENU_WIFI_DrawSelectAPMenu(false);
		return;
	}
	if (sysmenu_wifi_setAPpassword_menu_opened)
	{
		SYSMENU_WIFI_DrawAPpasswordMenu(false);
		return;
	}
	if (sysmenu_trx_setCallsign_menu_opened)
	{
		SYSMENU_TRX_DrawCallsignMenu(false);
		return;
	}
	if (sysmenu_spectrum_opened)
	{
		SPEC_Draw();
		return;
	}
	if (sysmenu_swr_opened)
	{
		SWR_Draw();
		return;
	}
	if (sysmenu_TDM_CTRL_opened)				//Tisho
	{
		TDM_Voltages();									
		return;
	}
	if(sysmenu_infowindow_opened)
		return;
	LCD_busy = true;

	sysmenu_i = 0;
	sysmenu_y = 5;

	if (draw_background)
		LCDDriver_Fill(BG_COLOR);

	uint8_t current_selected_page = systemMenuIndex / LAYOUT->SYSMENU_MAX_ITEMS_ON_PAGE;
	if (current_selected_page * LAYOUT->SYSMENU_MAX_ITEMS_ON_PAGE > *sysmenu_item_count_selected)
		current_selected_page = 0;

	for (uint8_t m = 0; m < *sysmenu_item_count_selected; m++)
	{
		uint8_t current_page = m / LAYOUT->SYSMENU_MAX_ITEMS_ON_PAGE;
		if (current_selected_page == current_page)
			drawSystemMenuElement(sysmenu_handlers_selected[m].title, sysmenu_handlers_selected[m].type, sysmenu_handlers_selected[m].value, false);
	}

	LCD_UpdateQuery.SystemMenu = false;
	LCD_busy = false;
}

void eventRotateSystemMenu(int8_t direction)
{
	if (sysmenu_wifi_selectap_menu_opened)
	{
		SYSMENU_WIFI_SelectAPMenuMove(0);
		return;
	}
	if (sysmenu_wifi_setAPpassword_menu_opened)
	{
		SYSMENU_WIFI_RotatePasswordChar(direction);
		return;
	}
	if (sysmenu_trx_setCallsign_menu_opened)
	{
		SYSMENU_TRX_RotateCallsignChar(direction);
		return;
	}
	if (sysmenu_timeMenuOpened)
	{
		SYSMENU_HANDL_SETTIME(direction);
		return;
	}
	if (sysmenu_handlers_selected[systemMenuIndex].type != SYSMENU_INFOLINE)
		sysmenu_handlers_selected[systemMenuIndex].menuHandler(direction);
	if (sysmenu_handlers_selected[systemMenuIndex].type != SYSMENU_RUN)
		redrawCurrentItem();
}

void eventCloseSystemMenu(void)
{
	if (sysmenu_wifi_selectap_menu_opened)
	{
		sysmenu_wifi_selectap_menu_opened = false;
		systemMenuIndex = 0;
		drawSystemMenu(true);
		WIFI_InitStateIndex = 0;
		WIFI_State = WIFI_INITED;
	}
	else if (sysmenu_wifi_setAPpassword_menu_opened)
	{
		sysmenu_wifi_setAPpassword_menu_opened = false;
		systemMenuIndex = 0;
		drawSystemMenu(true);
		WIFI_InitStateIndex = 0;
		WIFI_State = WIFI_INITED;
	}
	else if (sysmenu_trx_setCallsign_menu_opened)
	{
		sysmenu_trx_setCallsign_menu_opened = false;
		systemMenuIndex = 0;
		drawSystemMenu(true);
	}
	else if (sysmenu_spectrum_opened)
	{
		sysmenu_spectrum_opened = false;
		SPEC_Stop();
		systemMenuIndex = 0;
		drawSystemMenu(true);
	}
	else if (sysmenu_timeMenuOpened)
	{
		sysmenu_timeMenuOpened = false;
		systemMenuIndex = 0;
		drawSystemMenu(true);
	}
	else if (sysmenu_swr_opened)
	{
		sysmenu_swr_opened = false;
		SWR_Stop();
		systemMenuIndex = 0;
		drawSystemMenu(true);
	}
	else if (sysmenu_TDM_CTRL_opened)			//Tisho
	{
		sysmenu_TDM_CTRL_opened = false;
		systemMenuIndex = 0;
		drawSystemMenu(true);
	}
	else if (sysmenu_infowindow_opened)
	{
		sysmenu_infowindow_opened = false;
		systemMenuIndex = 0;
		drawSystemMenu(true);
	}
	else if (sysmenu_services_opened)
	{
		sysmenu_handlers_selected = &sysmenu_handlers[0];
		sysmenu_item_count_selected = &sysmenu_item_count;
		sysmenu_onroot = true;
		systemMenuIndex = systemMenuRootIndex;
		sysmenu_services_opened = false;
		LCD_systemMenuOpened = false;
		LCD_UpdateQuery.Background = true;
		LCD_redraw();
	}
	else
	{
		if (sysmenu_onroot)
		{
			LCD_systemMenuOpened = false;
			LCD_UpdateQuery.Background = true;
			LCD_redraw();
		}
		else
		{
			sysmenu_handlers_selected = &sysmenu_handlers[0];
			sysmenu_item_count_selected = &sysmenu_item_count;
			sysmenu_onroot = true;
			systemMenuIndex = systemMenuRootIndex;
			drawSystemMenu(true);
		}
	}
	NeedSaveSettings = true;
	if (sysmenu_hiddenmenu_enabled)
		NeedSaveCalibration = true;
}

void eventCloseAllSystemMenu(void)
{
	sysmenu_handlers_selected = &sysmenu_handlers[0];
	sysmenu_item_count_selected = &sysmenu_item_count;
	sysmenu_onroot = true;
	systemMenuIndex = systemMenuRootIndex;
	LCD_systemMenuOpened = false;
	LCD_UpdateQuery.Background = true;
	LCD_redraw();
}

void eventSecRotateSystemMenu(int8_t direction)
{
	//wifi select AP menu
	if (sysmenu_wifi_selectap_menu_opened)
	{
		if (direction < 0)
			SYSMENU_WIFI_SelectAPMenuMove(-1);
		else
			SYSMENU_WIFI_SelectAPMenuMove(1);
		return;
	}
	//wifi set password menu
	if (sysmenu_wifi_setAPpassword_menu_opened)
	{
		if (direction < 0 && sysmenu_wifi_selected_ap_password_char_index > 0)
		{
			sysmenu_wifi_selected_ap_password_char_index--;
			SYSMENU_WIFI_DrawAPpasswordMenu(true);
		}
		else if (sysmenu_wifi_selected_ap_password_char_index < (MAX_WIFIPASS_LENGTH - 1))
		{
			sysmenu_wifi_selected_ap_password_char_index++;
			SYSMENU_WIFI_DrawAPpasswordMenu(true);
		}
		return;
	}
	//Callsign menu
	if (sysmenu_trx_setCallsign_menu_opened)
	{
		if (direction < 0 && sysmenu_trx_selected_callsign_char_index > 0)
		{
			sysmenu_trx_selected_callsign_char_index--;
			SYSMENU_TRX_DrawCallsignMenu(true);
		}
		else if (sysmenu_trx_selected_callsign_char_index < (MAX_CALLSIGN_LENGTH - 1))
		{
			sysmenu_trx_selected_callsign_char_index++;
			SYSMENU_TRX_DrawCallsignMenu(true);
		}
		return;
	}
	//spectrum analyzer
	if (sysmenu_spectrum_opened)
	{
		SPEC_Stop();
		sysmenu_spectrum_opened = false;
		LCDDriver_Fill(BG_COLOR);
		drawSystemMenu(true);
		return;
	}
	if (sysmenu_swr_opened)
		return;
	if (sysmenu_infowindow_opened)
		return;
	//time menu
	if (sysmenu_timeMenuOpened)
	{
		LCDDriver_Fill(BG_COLOR);
		if (direction < 0)
		{
			TimeMenuSelection--;
			if (TimeMenuSelection > 2)
				TimeMenuSelection = 2;
		}
		else
		{
			TimeMenuSelection++;
			if (TimeMenuSelection == 3)
				TimeMenuSelection = 0;
		}
		LCD_UpdateQuery.SystemMenu = true;
		return;
	}
	//other
	uint8_t current_page = systemMenuIndex / LAYOUT->SYSMENU_MAX_ITEMS_ON_PAGE;
	LCDDriver_drawFastHLine(0, (5 + (systemMenuIndex - current_page * LAYOUT->SYSMENU_MAX_ITEMS_ON_PAGE) * LAYOUT->SYSMENU_ITEM_HEIGHT) + 17, LAYOUT->SYSMENU_W, BG_COLOR);
	if (direction < 0)
	{
		if (systemMenuIndex > 0)
			systemMenuIndex--;
		else if (!sysmenu_hiddenmenu_enabled && sysmenu_onroot)
			systemMenuIndex = *sysmenu_item_count_selected - 2;
		else
			systemMenuIndex = *sysmenu_item_count_selected - 1;
	}
	else
	{
		if (systemMenuIndex < (*sysmenu_item_count_selected - 1))
		{
			systemMenuIndex++;
		}
		else
			systemMenuIndex = 0;
	}
	if (!sysmenu_hiddenmenu_enabled && sysmenu_handlers_selected[systemMenuIndex].type == SYSMENU_HIDDEN_MENU)
		systemMenuIndex = 0;
	while (systemMenuIndex > 0 && sysmenu_handlers_selected[systemMenuIndex].type == SYSMENU_INFOLINE)
		systemMenuIndex = 0;

	redrawCurrentItem();
	if (sysmenu_onroot)
		systemMenuRootIndex = systemMenuIndex;
	uint8_t new_page = systemMenuIndex / LAYOUT->SYSMENU_MAX_ITEMS_ON_PAGE;

	if (current_page != new_page)
		drawSystemMenu(true);
}

static void redrawCurrentItem(void)
{
	uint8_t current_page = systemMenuIndex / LAYOUT->SYSMENU_MAX_ITEMS_ON_PAGE;
	sysmenu_i = (uint8_t)(systemMenuIndex - current_page * LAYOUT->SYSMENU_MAX_ITEMS_ON_PAGE);
	sysmenu_y = 5 + (systemMenuIndex - current_page * LAYOUT->SYSMENU_MAX_ITEMS_ON_PAGE) * LAYOUT->SYSMENU_ITEM_HEIGHT;
	drawSystemMenuElement(sysmenu_handlers_selected[systemMenuIndex].title, sysmenu_handlers_selected[systemMenuIndex].type, sysmenu_handlers_selected[systemMenuIndex].value, true);
}

static void drawSystemMenuElement(char *title, SystemMenuType type, uint32_t *value, bool onlyVal)
{
	if (!sysmenu_hiddenmenu_enabled && type == SYSMENU_HIDDEN_MENU)
		return;

	char ctmp[10] = {0};
	if (!onlyVal)
	{
		LCDDriver_Fill_RectXY(0, sysmenu_y, LAYOUT->SYSMENU_W, sysmenu_y + 17, BG_COLOR);
		LCDDriver_printText(title, LAYOUT->SYSMENU_X1, sysmenu_y, FG_COLOR, BG_COLOR, 2);
	}

	uint16_t x_pos = LAYOUT->SYSMENU_X2;
	float32_t tmp_float = 0;
	switch (type)
	{
	case SYSMENU_UINT8:
		sprintf(ctmp, "%d", (uint8_t)*value);
		break;
	case SYSMENU_UINT16:
		sprintf(ctmp, "%d", (uint16_t)*value);
		break;
	case SYSMENU_UINT32:
		sprintf(ctmp, "%u", (uint32_t)*value);
		x_pos = LAYOUT->SYSMENU_X2_BIGINT;
		break;
	case SYSMENU_UINT32R:
		sprintf(ctmp, "%u", (uint32_t)*value);
		x_pos = LAYOUT->SYSMENU_X2R_BIGINT; //-V1048
		break;
	case SYSMENU_INT8:
		sprintf(ctmp, "%d", (int8_t)*value);
		break;
	case SYSMENU_INT16:
		sprintf(ctmp, "%d", (int16_t)*value);
		break;
	case SYSMENU_INT32:
		sprintf(ctmp, "%d", (int32_t)*value);
		x_pos = LAYOUT->SYSMENU_X2_BIGINT;
		break;
	case SYSMENU_FLOAT32:
		memcpy(&tmp_float, value, sizeof(float32_t));
		sprintf(ctmp, "%.2f", (double)tmp_float);
		x_pos = LAYOUT->SYSMENU_X2_BIGINT;
		break;
	case SYSMENU_BOOLEAN:
		sprintf(ctmp, "%d", (int8_t)*value);
		if ((uint8_t)*value == 1)
			sprintf(ctmp, "YES");
		else
			sprintf(ctmp, "NO");
		break;
	case SYSMENU_RUN:
		sprintf(ctmp, "RUN");
		break;
	case SYSMENU_MENU:
		sprintf(ctmp, "->");
		break;
	case SYSMENU_HIDDEN_MENU:
		sprintf(ctmp, "!!!");
		break;
	case SYSMENU_INFOLINE:
		break;
	}

	if (onlyVal)
		LCDDriver_Fill_RectWH(x_pos, sysmenu_y, 6 * 12, 13, BG_COLOR);
	if (type != SYSMENU_INFOLINE)
		LCDDriver_printText(ctmp, x_pos, sysmenu_y, FG_COLOR, BG_COLOR, 2);

	uint8_t current_selected_page = systemMenuIndex / LAYOUT->SYSMENU_MAX_ITEMS_ON_PAGE;
	if (systemMenuIndex == sysmenu_i + current_selected_page * LAYOUT->SYSMENU_MAX_ITEMS_ON_PAGE)
		LCDDriver_drawFastHLine(0, sysmenu_y + 17, LAYOUT->SYSMENU_W, FG_COLOR);
	sysmenu_i++;
	sysmenu_y += LAYOUT->SYSMENU_ITEM_HEIGHT;
}
