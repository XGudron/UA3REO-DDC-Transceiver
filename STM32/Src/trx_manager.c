#include "stm32h7xx_hal.h"
#include "main.h"
#include "trx_manager.h"
#include "functions.h"
#include "lcd.h"
#include "fpga.h"
#include "settings.h"
#include "wm8731.h"
#include "fpga.h"
#include "bands.h"
#include "agc.h"
#include "audio_filters.h"
#include "usbd_audio_if.h"
#include "cw_decoder.h"
#include "front_unit.h"
#include "rf_unit.h"
#include "system_menu.h"
#include "vad.h"

volatile bool TRX_ptt_hard = false;
volatile bool TRX_ptt_cat = false;
volatile bool TRX_old_ptt_cat = false;
volatile bool TRX_key_serial = false;
volatile bool TRX_old_key_serial = false;
volatile bool TRX_key_dot_hard = false;
volatile bool TRX_key_dash_hard = false;
volatile uint_fast16_t TRX_Key_Timeout_est = 0;
volatile bool TRX_RX1_IQ_swap = false;
volatile bool TRX_RX2_IQ_swap = false;
volatile bool TRX_TX_IQ_swap = false;
volatile bool TRX_Tune = false;
volatile bool TRX_Inited = false;
volatile int_fast16_t TRX_RX_dBm = -100;
volatile bool TRX_ADC_OTR = false;
volatile bool TRX_DAC_OTR = false;
volatile int16_t TRX_ADC_MINAMPLITUDE = 0;
volatile int16_t TRX_ADC_MAXAMPLITUDE = 0;
volatile int32_t TRX_VCXO_ERROR = 0;
volatile uint16_t TRX_Volume;
volatile uint32_t TRX_SNTP_Synced = 0; // time of the last time synchronization
volatile int_fast16_t TRX_SHIFT = 0;
volatile float32_t TRX_MAX_TX_Amplitude = MAX_TX_AMPLITUDE;
volatile float32_t TRX_PWR_Forward = 0;
volatile float32_t TRX_PWR_Backward = 0;
volatile float32_t TRX_SWR = 0;
volatile float32_t TRX_VLT_forward = 0;				//Tisho
volatile float32_t TRX_VLT_backward = 0;			//Tisho 
volatile float32_t TRX_ALC = 0;
volatile bool TRX_DAC_DIV0 = false;
volatile bool TRX_DAC_DIV1 = false;
volatile bool TRX_DAC_HP1 = false;
volatile bool TRX_DAC_HP2 = false;
volatile bool TRX_DAC_X4 = false;
volatile bool TRX_DCDC_Freq = false;
static uint_fast8_t autogain_wait_reaction = 0;	  // timer for waiting for a reaction from changing the ATT / PRE modes
volatile uint8_t TRX_AutoGain_Stage = 0;			  // stage of working out the amplification corrector
static uint32_t KEYER_symbol_start_time = 0;	  // start time of the automatic key character
static bool KEYER_symbol_status = false;		  // status (signal or period) of the automatic key symbol
volatile float32_t TRX_STM32_VREF = 3.3f;		  // voltage on STM32
volatile float32_t TRX_STM32_TEMPERATURE = 30.0f; // STM32 temperature
volatile float32_t TRX_IQ_phase_error = 0.0f;
volatile bool TRX_NeedGoToBootloader = false;
volatile bool TRX_Temporary_Stop_BandMap = false;
volatile bool TRX_Mute = false;
volatile uint32_t TRX_Temporary_Mute_StartTime = 0;
uint32_t TRX_freq_phrase = 0;
uint32_t TRX_freq_phrase2 = 0;
uint32_t TRX_freq_phrase_tx = 0;
volatile float32_t TRX_RF_Temperature = 0.0f;
volatile bool TRX_ScanMode = false;

static uint_fast8_t TRX_TXRXMode = 0; //0 - undef, 1 - rx, 2 - tx, 3 - txrx
static void TRX_Start_RX(void);
static void TRX_Start_TX(void);
static void TRX_Start_TXRX(void);

bool TRX_on_TX(void)
{
	if (TRX_ptt_hard || TRX_ptt_cat || TRX_Tune || CurrentVFO()->Mode == TRX_MODE_LOOPBACK || TRX_Key_Timeout_est > 0)
		return true;
	return false;
}

void TRX_Init()
{
	TRX_Start_RX();
	uint_fast8_t saved_mode = CurrentVFO()->Mode;
	TRX_setFrequency(CurrentVFO()->Freq, CurrentVFO());
	TRX_setMode(saved_mode, CurrentVFO());
	HAL_ADCEx_InjectedStart(&hadc1); //ADC RF-UNIT'а
	HAL_ADCEx_InjectedStart(&hadc3); //ADC CPU temperature
}

void TRX_Restart_Mode()
{
	uint_fast8_t mode = CurrentVFO()->Mode;
	//CLAR
	if (TRX.CLAR)
	{
		TRX.current_vfo = !TRX.current_vfo;
		TRX_setFrequency(CurrentVFO()->Freq, CurrentVFO());
		TRX_setMode(CurrentVFO()->Mode, CurrentVFO());
		LCD_UpdateQuery.FreqInfo = true;
		LCD_UpdateQuery.TopButtons = true;
		LCD_UpdateQuery.StatusInfoGUI = true;
	}
	//
	if (TRX_on_TX())
	{
		if (mode == TRX_MODE_LOOPBACK || mode == TRX_MODE_CW_L || mode == TRX_MODE_CW_U)
			TRX_Start_TXRX();
		else
			TRX_Start_TX();
	}
	else
	{
		TRX_Start_RX();
	}
	NeedReinitReverber = true;
	FFT_Reset();
}

static void TRX_Start_RX()
{
	if (TRX_TXRXMode == 1)
		return;
	//sendToDebug_str("RX MODE\r\n");
	RF_UNIT_UpdateState(false);
	WM8731_CleanBuffer();
	Processor_NeedRXBuffer = false;
	WM8731_Buffer_underrun = false;
	WM8731_DMA_state = true;
	WM8731_RX_mode();
	WM8731_start_i2s_and_dma();
	TRX_TXRXMode = 1;
}

static void TRX_Start_TX()
{
	if (TRX_TXRXMode == 2)
		return;
	//sendToDebug_str("TX MODE\r\n");
	RF_UNIT_UpdateState(false);
	WM8731_CleanBuffer();
	HAL_Delay(10); // delay before the RF signal is applied, so that the relay has time to trigger
	WM8731_TX_mode();
	WM8731_start_i2s_and_dma();
	TRX_TXRXMode = 2;
}

static void TRX_Start_TXRX()
{
	if (TRX_TXRXMode == 3)
		return;
	//sendToDebug_str("TXRX MODE\r\n");
	RF_UNIT_UpdateState(false);
	WM8731_CleanBuffer();
	WM8731_TXRX_mode();
	WM8731_start_i2s_and_dma();
	TRX_TXRXMode = 3;
}

void TRX_ptt_change(void)
{
	if (TRX_Tune)
		return;
	bool TRX_new_ptt_hard = !HAL_GPIO_ReadPin(PTT_IN_GPIO_Port, PTT_IN_Pin);
	if (TRX_ptt_hard != TRX_new_ptt_hard)
	{
		TRX_ptt_hard = TRX_new_ptt_hard;
		if (TRX_ptt_hard && (CurrentVFO()->Mode == TRX_MODE_LSB || CurrentVFO()->Mode == TRX_MODE_USB) && TRX.InputType_USB)
		{
			TRX.InputType_USB = false;
			TRX.InputType_MIC = true;
			TRX.InputType_LINE = false;
		}
		TRX_ptt_cat = false;
		LCD_UpdateQuery.StatusInfoGUI = true;
		FPGA_NeedSendParams = true;
		TRX_Restart_Mode();
	}
	if (TRX_ptt_cat != TRX_old_ptt_cat)
	{
		TRX_old_ptt_cat = TRX_ptt_cat;
		if (TRX_ptt_cat && (CurrentVFO()->Mode == TRX_MODE_DIGI_L || CurrentVFO()->Mode == TRX_MODE_DIGI_U))
		{
			TRX.InputType_USB = true;
			TRX.InputType_MIC = false;
			TRX.InputType_LINE = false;
		}
		LCD_UpdateQuery.StatusInfoGUI = true;
		FPGA_NeedSendParams = true;
		TRX_Restart_Mode();
	}
}

void TRX_key_change(void)
{
	if (TRX_Tune)
		return;
	if (CurrentVFO()->Mode != TRX_MODE_CW_L && CurrentVFO()->Mode != TRX_MODE_CW_U)
		return;
	bool TRX_new_key_dot_hard = !HAL_GPIO_ReadPin(KEY_IN_DOT_GPIO_Port, KEY_IN_DOT_Pin);
	if (TRX_key_dot_hard != TRX_new_key_dot_hard)
	{
		TRX_key_dot_hard = TRX_new_key_dot_hard;
		if (TRX_key_dot_hard == true)
			TRX_Key_Timeout_est = TRX.CW_Key_timeout;
		if (TRX.CW_Key_timeout == 0)
			TRX_ptt_cat = TRX_key_dot_hard;
		KEYER_symbol_start_time = 0;
		KEYER_symbol_status = false;
		LCD_UpdateQuery.StatusInfoGUI = true;
		FPGA_NeedSendParams = true;
		TRX_Restart_Mode();
	}
	bool TRX_new_key_dash_hard = !HAL_GPIO_ReadPin(KEY_IN_DASH_GPIO_Port, KEY_IN_DASH_Pin);
	if (TRX_key_dash_hard != TRX_new_key_dash_hard)
	{
		TRX_key_dash_hard = TRX_new_key_dash_hard;
		if (TRX_key_dash_hard == true)
			TRX_Key_Timeout_est = TRX.CW_Key_timeout;
		if (TRX.CW_Key_timeout == 0)
			TRX_ptt_cat = TRX_key_dash_hard;
		KEYER_symbol_start_time = 0;
		KEYER_symbol_status = false;
		LCD_UpdateQuery.StatusInfoGUI = true;
		FPGA_NeedSendParams = true;
		TRX_Restart_Mode();
	}
	if (TRX_key_serial != TRX_old_key_serial)
	{
		TRX_old_key_serial = TRX_key_serial;
		if (TRX_key_serial == true)
			TRX_Key_Timeout_est = TRX.CW_Key_timeout;
		if (TRX.CW_Key_timeout == 0)
			TRX_ptt_cat = TRX_key_serial;
		LCD_UpdateQuery.StatusInfoGUI = true;
		FPGA_NeedSendParams = true;
		TRX_Restart_Mode();
	}
}

void TRX_setFrequency(uint32_t _freq, VFO *vfo)
{
	if (_freq < 1)
		return;
	if (_freq >= MAX_RX_FREQ_HZ)
		_freq = MAX_RX_FREQ_HZ;

	vfo->Freq = _freq;
	if (sysmenu_spectrum_opened)
		return;

	//set DC-DC Sync freq
	uint32_t dcdc_offset_0 = _freq % DCDC_FREQ_0;
	uint32_t dcdc_offset_1 = _freq % DCDC_FREQ_1;
	if (dcdc_offset_0 > dcdc_offset_1)
		TRX_DCDC_Freq = 0;
	else
		TRX_DCDC_Freq = 1;

	//get band
	int_fast8_t bandFromFreq = getBandFromFreq(_freq, false);
	if (bandFromFreq >= 0)
	{
		TRX.BANDS_SAVED_SETTINGS[bandFromFreq].Freq = _freq;
	}
	if (TRX.BandMapEnabled && !TRX_Temporary_Stop_BandMap)
	{
		uint_fast8_t mode_from_bandmap = getModeFromFreq(vfo->Freq);
		if (vfo->Mode != mode_from_bandmap)
		{
			TRX_setMode(mode_from_bandmap, vfo);
			LCD_UpdateQuery.TopButtons = true;
		}
	}

	//get fpga freq phrase
	VFO *current_vfo = CurrentVFO();
	VFO *secondary_vfo = SecondaryVFO();
	TRX_freq_phrase = getRXPhraseFromFrequency((int32_t)current_vfo->Freq + TRX_SHIFT, 1);
	TRX_freq_phrase2 = getRXPhraseFromFrequency((int32_t)secondary_vfo->Freq + TRX_SHIFT, 2);
	TRX_freq_phrase_tx = getTXPhraseFromFrequency((int32_t)current_vfo->Freq + TRX_SHIFT);
	if (!TRX_on_TX())
	{
		switch (current_vfo->Mode)
		{
		case TRX_MODE_CW_L:
			TRX_freq_phrase_tx = getTXPhraseFromFrequency((int32_t)current_vfo->Freq - TRX.CW_GENERATOR_SHIFT_HZ);
			break;
		case TRX_MODE_CW_U:
			TRX_freq_phrase_tx = getTXPhraseFromFrequency((int32_t)current_vfo->Freq + TRX.CW_GENERATOR_SHIFT_HZ);
			break;
		default:
			TRX_freq_phrase_tx = getTXPhraseFromFrequency((int32_t)current_vfo->Freq);
			break;
		}
	}

	//
	TRX_MAX_TX_Amplitude = getMaxTXAmplitudeOnFreq(vfo->Freq);
	FPGA_NeedSendParams = true;
}

void TRX_setMode(uint_fast8_t _mode, VFO *vfo)
{
	if (vfo->Mode == TRX_MODE_LOOPBACK || _mode == TRX_MODE_LOOPBACK)
		LCD_UpdateQuery.StatusInfoGUI = true;
	vfo->Mode = _mode;
	if (vfo->Mode == TRX_MODE_LOOPBACK)
		TRX_Start_TXRX();

	switch (_mode)
	{
	case TRX_MODE_AM:
		vfo->LPF_Filter_Width = TRX.AM_LPF_Filter;
		vfo->HPF_Filter_Width = 0;
		break;
	case TRX_MODE_LSB:
	case TRX_MODE_USB:
	case TRX_MODE_DIGI_L:
	case TRX_MODE_DIGI_U:
		vfo->LPF_Filter_Width = TRX.SSB_LPF_Filter;
		vfo->HPF_Filter_Width = TRX.SSB_HPF_Filter;
		break;
	case TRX_MODE_CW_L:
	case TRX_MODE_CW_U:
		vfo->LPF_Filter_Width = TRX.CW_LPF_Filter;
		vfo->HPF_Filter_Width = TRX.CW_HPF_Filter;
		LCD_UpdateQuery.StatusInfoGUI = true;
		LCD_UpdateQuery.TextBar = true;
		break;
	case TRX_MODE_NFM:
		vfo->LPF_Filter_Width = TRX.FM_LPF_Filter;
		vfo->HPF_Filter_Width = 0;
		break;
	case TRX_MODE_WFM:
		vfo->LPF_Filter_Width = 0;
		vfo->HPF_Filter_Width = 0;
		break;
	}
	NeedReinitAudioFilters = true;
	NeedSaveSettings = true;
	LCD_UpdateQuery.StatusInfoBar = true;
	LCD_UpdateQuery.StatusInfoGUI = true;
}

void TRX_DoAutoGain(void)
{
	#define SKIP_CYCLES_DOWNSTAGE 10 //skip cycles on stage downgrade
	static uint8_t skip_cycles = 0;
	
	//Process AutoGain feature
	if (TRX.AutoGain && !TRX_on_TX())
	{
		int32_t max_amplitude = abs(TRX_ADC_MAXAMPLITUDE);
		if(abs(TRX_ADC_MINAMPLITUDE) > max_amplitude)
			max_amplitude = abs(TRX_ADC_MINAMPLITUDE);
		
		switch (TRX_AutoGain_Stage)
		{
		case 0: // stage 1 - LPF + BPF + ATT
			TRX.RF_Filters = true;
			TRX.LNA = false;
			TRX.ADC_PGA = false;
			TRX.ADC_Driver = false;
			TRX.ATT = true;
			FPGA_NeedSendParams = true;
			LCD_UpdateQuery.TopButtons = true;
			autogain_wait_reaction = 0;
			if(skip_cycles == 0)
			{
				sendToDebug_strln("AUTOGAIN LPF + BPF + ATT");
				resetVAD();
				TRX_AutoGain_Stage++;
			}
			else
				skip_cycles--;
			break;
		case 1:																					// changed the state, process the results
			if ((max_amplitude * db2rateV(-TRX.ATT_DB)) <= AUTOGAIN_TARGET_AMPLITUDE) // if we can turn off ATT - go to the next stage (+ 12dB)
				autogain_wait_reaction++;
			else
				autogain_wait_reaction = 0;
			if (autogain_wait_reaction >= AUTOGAIN_CORRECTOR_WAITSTEP)
			{
				TRX_AutoGain_Stage++;
				autogain_wait_reaction = 0;
			}
			break;
		case 2: // stage 2 - LPF + BPF
			TRX.RF_Filters = true;
			TRX.LNA = false;
			TRX.ATT = false;
			TRX.ADC_PGA = false;
			TRX.ADC_Driver = false;
			FPGA_NeedSendParams = true;
			LCD_UpdateQuery.TopButtons = true;
			autogain_wait_reaction = 0;
			if(skip_cycles == 0)
			{
				sendToDebug_strln("AUTOGAIN LPF + BPF");
				resetVAD();
				TRX_AutoGain_Stage++;
			}
			else
				skip_cycles--;
			break;
		case 3: // changed the state, process the results
			if (max_amplitude > AUTOGAIN_MAX_AMPLITUDE || TRX_ADC_OTR)
			{
				TRX_AutoGain_Stage -= 3; // too much gain, go back one step
				skip_cycles = SKIP_CYCLES_DOWNSTAGE;
			}
			if ((max_amplitude * db2rateV(ADC_PGA_GAIN_DB)) <= AUTOGAIN_TARGET_AMPLITUDE) // if we can turn off ATT - go to the next stage (+ 12dB)
				autogain_wait_reaction++;
			else
				autogain_wait_reaction = 0;
			if (autogain_wait_reaction >= AUTOGAIN_CORRECTOR_WAITSTEP)
			{
				TRX_AutoGain_Stage++;
				autogain_wait_reaction = 0;
			}
			break;
		case 4: // stage 3 - LPF + BPF + PGA
			TRX.RF_Filters = true;
			TRX.LNA = false;
			TRX.ATT = false;
			TRX.ADC_PGA = true;
			TRX.ADC_Driver = false;
			FPGA_NeedSendParams = true;
			LCD_UpdateQuery.TopButtons = true;
			autogain_wait_reaction = 0;
			if(skip_cycles == 0)
			{
				sendToDebug_strln("AUTOGAIN LPF + BPF + PGA");
				resetVAD();
				TRX_AutoGain_Stage++;
			}
			else
				skip_cycles--;
			break;
		case 5: // changed the state, process the results
			if (max_amplitude > AUTOGAIN_MAX_AMPLITUDE || TRX_ADC_OTR)
			{
				TRX_AutoGain_Stage -= 3; // too much gain, go back one step
				skip_cycles = SKIP_CYCLES_DOWNSTAGE;
			}
			if ((max_amplitude * db2rateV(ADC_DRIVER_GAIN_DB)) <= AUTOGAIN_TARGET_AMPLITUDE) // if we can turn off ATT - go to the next stage (+ 12dB)
				autogain_wait_reaction++;
			else
			{
				autogain_wait_reaction = 0;
			}
			if (autogain_wait_reaction >= AUTOGAIN_CORRECTOR_WAITSTEP)
			{
				TRX_AutoGain_Stage++;
				autogain_wait_reaction = 0;
			}
			break;
		case 6: // stage 4 - LPF + BPF + PGA + DRIVER
			TRX.RF_Filters = true;
			TRX.LNA = false;
			TRX.ATT = false;
			TRX.ADC_PGA = true;
			TRX.ADC_Driver = true;
			FPGA_NeedSendParams = true;
			LCD_UpdateQuery.TopButtons = true;
			autogain_wait_reaction = 0;
			if(skip_cycles == 0)
			{
				sendToDebug_strln("AUTOGAIN LPF + BPF + PGA + DRIVER");
				resetVAD();
				TRX_AutoGain_Stage++;
			}
			else
				skip_cycles--;
			break;
		case 7: // changed the state, process the results
			if (max_amplitude > AUTOGAIN_MAX_AMPLITUDE || TRX_ADC_OTR)
			{
				TRX_AutoGain_Stage -= 3;															// too much gain, go back one step
				skip_cycles = SKIP_CYCLES_DOWNSTAGE;
			}
			if ((max_amplitude * db2rateV(ADC_LNA_GAIN_DB * 2)) <= AUTOGAIN_TARGET_AMPLITUDE) // if we can turn off ATT - go to the next stage (+ 12dB)
				autogain_wait_reaction++;
			else
				autogain_wait_reaction = 0;
			if (autogain_wait_reaction >= AUTOGAIN_CORRECTOR_WAITSTEP)
			{
				TRX_AutoGain_Stage++;
				autogain_wait_reaction = 0;
			}
			break;
		case 8: // stage 5 - LPF + BPF + PGA + DRIVER + LNA
			TRX.RF_Filters = true;
			TRX.LNA = true;
			TRX.ATT = false;
			TRX.ADC_PGA = true;
			TRX.ADC_Driver = true;
			FPGA_NeedSendParams = true;
			LCD_UpdateQuery.TopButtons = true;
			autogain_wait_reaction = 0;
			if(skip_cycles == 0)
			{
				sendToDebug_strln("AUTOGAIN LPF + BPF + PGA + DRIVER + LNA");
				resetVAD();
				TRX_AutoGain_Stage++;
			}
			else
				skip_cycles--;
			break;
		case 9: // changed the state, process the results
			if (max_amplitude > AUTOGAIN_MAX_AMPLITUDE || TRX_ADC_OTR)
			{
				TRX_AutoGain_Stage -= 3; // too much gain, go back one step
				skip_cycles = SKIP_CYCLES_DOWNSTAGE;
			}
			break;
			
		default:
			TRX_AutoGain_Stage = 0;
			break;
		}
		
		int8_t band = getBandFromFreq(CurrentVFO()->Freq, true);
		if (band > 0)
		{
			TRX.BANDS_SAVED_SETTINGS[band].ATT = TRX.ATT;
			TRX.BANDS_SAVED_SETTINGS[band].LNA = TRX.LNA;
			TRX.BANDS_SAVED_SETTINGS[band].ADC_Driver = TRX.ADC_Driver;
			TRX.BANDS_SAVED_SETTINGS[band].ADC_PGA = TRX.ADC_PGA;
			TRX.BANDS_SAVED_SETTINGS[band].AutoGain_Stage = TRX_AutoGain_Stage;
		}
	}
}

void TRX_DBMCalculate(void)
{
	if(Processor_RX_Power_value == 0)
		return;
	
	float32_t adc_volts = Processor_RX_Power_value * (TRX.ADC_PGA ? (ADC_RANGE_PGA / 2.0f) : (ADC_RANGE / 2.0f));
	if(TRX.ADC_Driver)
		adc_volts *= db2rateV(-ADC_DRIVER_GAIN_DB);
	TRX_RX_dBm = (int16_t)(10.0f * log10f_fast((adc_volts * adc_volts) / (ADC_INPUT_IMPEDANCE * 0.001f)));
	TRX_RX_dBm += CALIBRATE.smeter_calibration;
	
	Processor_RX_Power_value = 0;
}

float32_t TRX_GenerateCWSignal(float32_t power)
{
	if (!TRX.CW_KEYER)
		return power;

	uint32_t dot_length_ms = 1200 / TRX.CW_KEYER_WPM;
	uint32_t dash_length_ms = dot_length_ms * 3;
	uint32_t sim_space_length_ms = dot_length_ms;
	uint32_t curTime = HAL_GetTick();
	if (TRX_key_dot_hard && (KEYER_symbol_start_time + dot_length_ms) > curTime) // point is clamped
	{
		if (KEYER_symbol_status)
			return power;
		else
			return 0.0f;
	}
	else if (TRX_key_dash_hard && (KEYER_symbol_start_time + dash_length_ms) > curTime) // dash pressed
	{
		if (KEYER_symbol_status)
			return power;
		else
			return 0.0f;
	}
	else if ((KEYER_symbol_start_time + sim_space_length_ms) < curTime)
	{
		KEYER_symbol_start_time = curTime;
		KEYER_symbol_status = !KEYER_symbol_status;
	}
	return 0.0f;
}

float32_t TRX_getSTM32H743Temperature(void)
{
	uint16_t TS_CAL1 = *((uint16_t *)0x1FF1E820); // TS_CAL1 Temperature sensor raw data acquired value at 30 °C, VDDA=3.3 V //-V566
	uint16_t TS_CAL2 = *((uint16_t *)0x1FF1E840); // TS_CAL2 Temperature sensor raw data acquired value at 110 °C, VDDA=3.3 V //-V566
	uint32_t TS_DATA = HAL_ADCEx_InjectedGetValue(&hadc3, ADC_INJECTED_RANK_1);
	float32_t result = ((110.0f - 30.0f) / ((float32_t)TS_CAL2 - (float32_t)TS_CAL1)) * ((float32_t)TS_DATA - (float32_t)TS_CAL1) + 30; //from reference
	return result;
}

float32_t TRX_getSTM32H743vref(void)
{
	uint16_t VREFINT_CAL = *VREFINT_CAL_ADDR; // VREFIN_CAL Raw data acquired at temperature of 30 °C, VDDA = 3.3 V //-V566
	uint32_t VREFINT_DATA = HAL_ADCEx_InjectedGetValue(&hadc3, ADC_INJECTED_RANK_2);
	float32_t result = 3.3f * (float32_t)VREFINT_CAL / (float32_t)VREFINT_DATA; //from reference
	return result;
}

void TRX_TemporaryMute(void)
{
	WM8731_Mute();
	TRX_Temporary_Mute_StartTime = HAL_GetTick();
}

//process frequency scanner
void TRX_ProcessScanMode(void)
{
	static bool oldState = false;
	static uint32_t StateChangeTime = 0;
	if(oldState != VAD_Muting)
	{
		oldState = VAD_Muting;
		StateChangeTime = HAL_GetTick();
	}
	bool goSweep = false;
	if(VAD_Muting && ((HAL_GetTick() - StateChangeTime) > SCANNER_NOSIGNAL_TIME))
		goSweep = true;
	if(!VAD_Muting && ((HAL_GetTick() - StateChangeTime) > SCANNER_SIGNAL_TIME))
		goSweep = true;
	
	if(goSweep)
	{
		int8_t band = getBandFromFreq(CurrentVFO()->Freq, false);
		for(uint8_t region_id = 0; region_id < BANDS[band].regionsCount; region_id++)
		{
			if((BANDS[band].regions[region_id].startFreq <= CurrentVFO()->Freq) && (BANDS[band].regions[region_id].endFreq > CurrentVFO()->Freq))
			{
				uint32_t new_freq = (CurrentVFO()->Freq + SCANNER_FREQ_STEP) / SCANNER_FREQ_STEP * SCANNER_FREQ_STEP;
				if(new_freq >= BANDS[band].regions[region_id].endFreq)
					new_freq = BANDS[band].regions[region_id].startFreq;
				
				TRX_setFrequency(new_freq, CurrentVFO());
				LCD_UpdateQuery.FreqInfo = true;
				StateChangeTime = HAL_GetTick();
				break;
			}
		}
	}
}
