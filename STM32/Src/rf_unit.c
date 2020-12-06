#include "stm32h7xx_hal.h"
#include "main.h"
#include "rf_unit.h"
#include "lcd.h"
#include "trx_manager.h"
#include "settings.h"
#include "system_menu.h"
#include "functions.h"
#include "audio_filters.h"

#define SENS_TABLE_COUNT 24
static const int16_t KTY81_120_sensTable[SENS_TABLE_COUNT][2] = { // table of sensor characteristics
	{-55, 490},
	{-50, 515},
	{-40, 567},
	{-30, 624},
	{-20, 684},
	{-10, 747},
	{0, 815},
	{10, 886},
	{20, 961},
	{25, 1000},
	{30, 1040},
	{40, 1122},
	{50, 1209},
	{60, 1299},
	{70, 1392},
	{80, 1490},
	{90, 1591},
	{100, 1696},
	{110, 1805},
	{120, 1915},
	{125, 1970},
	{130, 2023},
	{140, 2124},
	{150, 2211}};

static uint8_t getBPFByFreq(uint32_t freq)
{
	if (freq >= CALIBRATE.BPF_0_START && freq < CALIBRATE.BPF_0_END)
		return 0;
	if (freq >= CALIBRATE.BPF_1_START && freq < CALIBRATE.BPF_1_END)
		return 1;
	if (freq >= CALIBRATE.BPF_2_START && freq < CALIBRATE.BPF_2_END)
		return 2;
	if (freq >= CALIBRATE.BPF_3_START && freq < CALIBRATE.BPF_3_END)
		return 3;
	if (freq >= CALIBRATE.BPF_4_START && freq < CALIBRATE.BPF_4_END)
		return 4;
	if (freq >= CALIBRATE.BPF_5_START && freq < CALIBRATE.BPF_5_END)
		return 5;
	if (freq >= CALIBRATE.BPF_6_START && freq < CALIBRATE.BPF_6_END)
		return 6;
	if (freq >= CALIBRATE.BPF_HPF)
		return 7;
	return 255;
}

void RF_UNIT_UpdateState(bool clean) // pass values to RF-UNIT
{
	bool dualrx_lpf_disabled = false;
	bool dualrx_bpf_disabled = false;
	if (TRX.Dual_RX && SecondaryVFO()->Freq > CALIBRATE.LPF_END)
		dualrx_lpf_disabled = true;
	if (TRX.Dual_RX && getBPFByFreq(CurrentVFO()->Freq) != getBPFByFreq(SecondaryVFO()->Freq))
		dualrx_bpf_disabled = true;

	float32_t att_val = TRX.ATT_DB;
	bool att_val_16 = false, att_val_8 = false, att_val_4 = false, att_val_2 = false, att_val_1 = false, att_val_05 = false;
	if (att_val >= 16.0f)
	{
		att_val_16 = true;
		att_val -= 16.0f;
	}
	if (att_val >= 8.0f)
	{
		att_val_8 = true;
		att_val -= 8.0f;
	}
	if (att_val >= 4.0f)
	{
		att_val_4 = true;
		att_val -= 4.0f;
	}
	if (att_val >= 2.0f)
	{
		att_val_2 = true;
		att_val -= 2.0f;
	}
	if (att_val >= 1.0f)
	{
		att_val_1 = true;
		att_val -= 1.0f;
	}
	if (att_val >= 0.5f)
	{
		att_val_05 = true;
		att_val -= 0.5f;
	}

	uint8_t bpf = getBPFByFreq(CurrentVFO()->Freq);

	uint8_t band_out = 0;
	int8_t band = getBandFromFreq(CurrentVFO()->Freq, true);
	if (band == 0) //2200m
		band_out = 0;
	if (band == 3) //160m
		band_out = 1;
	if (band == 6) //80m
		band_out = 2;
	if (band == 10) //60m
		band_out = 3;
	if (band == 12) //40m
		band_out = 4;
	if (band == 15) //30m
		band_out = 5;
	if (band == 18) //20m
		band_out = 6;
	if (band == 21) //17m
		band_out = 7;
	if (band == 23) //15m
		band_out = 8;
	if (band == 25) //12m
		band_out = 9;
	if (band == 27) //CB
		band_out = 10;
	if (band == 28) //10m
		band_out = 11;
	if (band == 29) //6m
		band_out = 12;
	if (band == 30) //FM
		band_out = 13;
	if (band == 32) //2m
		band_out = 14;
	if (band == 33) //70cm
		band_out = 15;

	HAL_GPIO_WritePin(RFUNIT_RCLK_GPIO_Port, RFUNIT_RCLK_Pin, GPIO_PIN_RESET); //latch
	MINI_DELAY
	for (uint8_t registerNumber = 0; registerNumber < 24; registerNumber++)
	{
		HAL_GPIO_WritePin(RFUNIT_CLK_GPIO_Port, RFUNIT_CLK_Pin, GPIO_PIN_RESET); // data block
		MINI_DELAY
		HAL_GPIO_WritePin(RFUNIT_DATA_GPIO_Port, RFUNIT_DATA_Pin, GPIO_PIN_RESET); //data
		MINI_DELAY
		if (!clean)
		{
			//U7-QH LPF_ON
			if (registerNumber == 0 && TRX.RF_Filters && (CurrentVFO()->Freq <= CALIBRATE.LPF_END) && !dualrx_lpf_disabled)
				HAL_GPIO_WritePin(RFUNIT_DATA_GPIO_Port, RFUNIT_DATA_Pin, GPIO_PIN_SET);
			//U7-QG LNA_ON
			if (registerNumber == 1 && !TRX_on_TX() && TRX.LNA)
				HAL_GPIO_WritePin(RFUNIT_DATA_GPIO_Port, RFUNIT_DATA_Pin, GPIO_PIN_SET);
			//U7-QF ATT_ON_0.5
			if (registerNumber == 2 && TRX.ATT && att_val_05)
				HAL_GPIO_WritePin(RFUNIT_DATA_GPIO_Port, RFUNIT_DATA_Pin, GPIO_PIN_SET);
			//U7-QE ATT_ON_1
			if (registerNumber == 3 && TRX.ATT && att_val_1)
				HAL_GPIO_WritePin(RFUNIT_DATA_GPIO_Port, RFUNIT_DATA_Pin, GPIO_PIN_SET);
			//U7-QD ATT_ON_2
			if (registerNumber == 4 && TRX.ATT && att_val_2)
				HAL_GPIO_WritePin(RFUNIT_DATA_GPIO_Port, RFUNIT_DATA_Pin, GPIO_PIN_SET);
			//U7-QC ATT_ON_4
			if (registerNumber == 5 && TRX.ATT && att_val_4)
				HAL_GPIO_WritePin(RFUNIT_DATA_GPIO_Port, RFUNIT_DATA_Pin, GPIO_PIN_SET);
			//U7-QB ATT_ON_8
			if (registerNumber == 6 && TRX.ATT && att_val_8)
				HAL_GPIO_WritePin(RFUNIT_DATA_GPIO_Port, RFUNIT_DATA_Pin, GPIO_PIN_SET);
			//U7-QA ATT_ON_16
			if (registerNumber == 7 && TRX.ATT && att_val_16)
				HAL_GPIO_WritePin(RFUNIT_DATA_GPIO_Port, RFUNIT_DATA_Pin, GPIO_PIN_SET);

			//U1-QH NOT USED
			//if (registerNumber == 8)
			//U1-QG BPF_2_A0
			if (registerNumber == 9 && TRX.RF_Filters && !dualrx_bpf_disabled && (bpf == 1 || bpf == 2))
				HAL_GPIO_WritePin(RFUNIT_DATA_GPIO_Port, RFUNIT_DATA_Pin, GPIO_PIN_SET);
			//U1-QF BPF_2_A1
			if (registerNumber == 10 && TRX.RF_Filters && !dualrx_bpf_disabled && (bpf == 4 || bpf == 2))
				HAL_GPIO_WritePin(RFUNIT_DATA_GPIO_Port, RFUNIT_DATA_Pin, GPIO_PIN_SET);
			//U1-QE BPF_2_!EN
			if (registerNumber == 11 && (!TRX.RF_Filters || dualrx_bpf_disabled || (bpf != 1 && bpf != 2 && bpf != 3 && bpf != 4)))
				HAL_GPIO_WritePin(RFUNIT_DATA_GPIO_Port, RFUNIT_DATA_Pin, GPIO_PIN_SET);
			//U1-QD BPF_1_A0
			if (registerNumber == 12 && TRX.RF_Filters && !dualrx_bpf_disabled && (bpf == 5 || bpf == 6))
				HAL_GPIO_WritePin(RFUNIT_DATA_GPIO_Port, RFUNIT_DATA_Pin, GPIO_PIN_SET);
			//U1-QC BPF_1_A1
			if (registerNumber == 13 && TRX.RF_Filters && !dualrx_bpf_disabled && (bpf == 0 || bpf == 6))
				HAL_GPIO_WritePin(RFUNIT_DATA_GPIO_Port, RFUNIT_DATA_Pin, GPIO_PIN_SET);
			//U1-QB BPF_1_!EN
			if (registerNumber == 14 && (!TRX.RF_Filters || dualrx_bpf_disabled || (bpf != 0 && bpf != 5 && bpf != 6 && bpf != 7)))
				HAL_GPIO_WritePin(RFUNIT_DATA_GPIO_Port, RFUNIT_DATA_Pin, GPIO_PIN_SET);
			//U1-QA BPF_ON
			if (registerNumber == 15 && TRX.RF_Filters && !dualrx_bpf_disabled && bpf != 255)
				HAL_GPIO_WritePin(RFUNIT_DATA_GPIO_Port, RFUNIT_DATA_Pin, GPIO_PIN_SET);

			//U3-QH BAND_OUT_0
			if (registerNumber == 16 && bitRead(band_out, 0))
				HAL_GPIO_WritePin(RFUNIT_DATA_GPIO_Port, RFUNIT_DATA_Pin, GPIO_PIN_SET);
			//U3-QG PTT_OUT
			if (registerNumber == 17 && TRX_on_TX() && CurrentVFO()->Mode != TRX_MODE_LOOPBACK)
				HAL_GPIO_WritePin(RFUNIT_DATA_GPIO_Port, RFUNIT_DATA_Pin, GPIO_PIN_SET);
			//U3-QF BAND_OUT_2
			if (registerNumber == 18 && bitRead(band_out, 2))
				HAL_GPIO_WritePin(RFUNIT_DATA_GPIO_Port, RFUNIT_DATA_Pin, GPIO_PIN_SET);
			//U3-QE BAND_OUT_3
			if (registerNumber == 19 && bitRead(band_out, 3))
				HAL_GPIO_WritePin(RFUNIT_DATA_GPIO_Port, RFUNIT_DATA_Pin, GPIO_PIN_SET);
			//U3-QD TUNE_OUT
			if (registerNumber == 20 && TRX_Tune)
				HAL_GPIO_WritePin(RFUNIT_DATA_GPIO_Port, RFUNIT_DATA_Pin, GPIO_PIN_SET);
			//U3-QC BAND_OUT_1
			if (registerNumber == 21 && bitRead(band_out, 1))
				HAL_GPIO_WritePin(RFUNIT_DATA_GPIO_Port, RFUNIT_DATA_Pin, GPIO_PIN_SET);
			//U3-QB FAN_OUT
			if (registerNumber == 22) //FAN
			{
				static bool fan_status = false;
				static bool fan_pwm = false;
				if (fan_status && TRX_RF_Temperature <= FAN_MEDIUM_STOP) // Temperature at which the fan stops
					fan_status = false;
				if (!fan_status &&TRX_RF_Temperature >= FAN_MEDIUM_START) // Temperature at which the fan starts at half power
				{
					fan_status = true;
					fan_pwm = true;
				}
				if (TRX_RF_Temperature >= FAN_FULL_START) // Temperature at which the fan starts at full power
					fan_pwm = false;

				if (fan_status)
				{
					if (fan_pwm) //PWM
					{
						const uint8_t on_ticks = 1;
						const uint8_t off_ticks = 1;
						static bool pwm_status = false; //true - on false - off
						static uint8_t pwm_ticks = 0;
						pwm_ticks++;
						if (pwm_status)
							HAL_GPIO_WritePin(RFUNIT_DATA_GPIO_Port, RFUNIT_DATA_Pin, GPIO_PIN_SET);
						if ((pwm_status && pwm_ticks == on_ticks) || (!pwm_status && pwm_ticks == off_ticks))
						{
							pwm_status = !pwm_status;
							pwm_ticks = 0;
						}
					}
					else
						HAL_GPIO_WritePin(RFUNIT_DATA_GPIO_Port, RFUNIT_DATA_Pin, GPIO_PIN_SET);
				}
			}
			//U3-QA ANT1_TX_OUT
			if (registerNumber == 23 && !TRX.ANT && TRX_on_TX()) //ANT1
				HAL_GPIO_WritePin(RFUNIT_DATA_GPIO_Port, RFUNIT_DATA_Pin, GPIO_PIN_SET);
			if (registerNumber == 23 && TRX.ANT && !TRX_on_TX()) //ANT2
				HAL_GPIO_WritePin(RFUNIT_DATA_GPIO_Port, RFUNIT_DATA_Pin, GPIO_PIN_SET);
		}
		MINI_DELAY
		HAL_GPIO_WritePin(RFUNIT_CLK_GPIO_Port, RFUNIT_CLK_Pin, GPIO_PIN_SET);
	}
	MINI_DELAY
	HAL_GPIO_WritePin(RFUNIT_CLK_GPIO_Port, RFUNIT_CLK_Pin, GPIO_PIN_RESET);
	MINI_DELAY
	HAL_GPIO_WritePin(RFUNIT_RCLK_GPIO_Port, RFUNIT_RCLK_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(RFUNIT_OE_GPIO_Port, RFUNIT_OE_Pin, GPIO_PIN_RESET);
}

void RF_UNIT_ProcessSensors(void)
{
	//THERMAL
	
	float32_t rf_thermal = (float32_t)(HAL_ADCEx_InjectedGetValue(&hadc1, ADC_INJECTED_RANK_3)) * TRX_STM32_VREF / 16383.0f;
	
	float32_t therm_resistance = -2000.0f * rf_thermal / (-3.3f + rf_thermal);
	uint_fast8_t point_left = 0;
	uint_fast8_t point_right = SENS_TABLE_COUNT - 1;
	for (uint_fast8_t i = 0; i < SENS_TABLE_COUNT; i++)
		if (KTY81_120_sensTable[i][1] < therm_resistance)
			point_left = i;
	for (uint_fast8_t i = (SENS_TABLE_COUNT - 1); i > 0; i--)
		if (KTY81_120_sensTable[i][1] >= therm_resistance)
			point_right = i;
	float32_t power_left = (float32_t)KTY81_120_sensTable[point_left][0];
	float32_t power_right = (float32_t)KTY81_120_sensTable[point_right][0];
	float32_t part_point_left = therm_resistance - KTY81_120_sensTable[point_left][1];
	float32_t part_point_right = KTY81_120_sensTable[point_right][1] - therm_resistance;
	float32_t part_point = part_point_left / (part_point_left + part_point_right);
	float32_t TRX_RF_Temperature_new = (power_left * (1.0f - part_point)) + (power_right * (part_point));
	if(TRX_RF_Temperature_new < 0.0f)
		TRX_RF_Temperature_new = 0.0f;
	if(fabsf(TRX_RF_Temperature_new - TRX_RF_Temperature) > 0.5f) //hysteresis
		TRX_RF_Temperature = TRX_RF_Temperature_new;
	
	//SWR
	float32_t forward = (float32_t)(HAL_ADCEx_InjectedGetValue(&hadc1, ADC_INJECTED_RANK_2)) * TRX_STM32_VREF / 16383.0f;
	float32_t backward = (float32_t)(HAL_ADCEx_InjectedGetValue(&hadc1, ADC_INJECTED_RANK_1)) * TRX_STM32_VREF / 16383.0f;
	//float32_t alc = HAL_ADCEx_InjectedGetValue(&hadc1, ADC_INJECTED_RANK_4) * TRX_STM32_VREF / 16383.0f;
	
//	static float32_t TRX_VLT_forward = 0.0f;		//Tisho
//	static float32_t TRX_VLT_backward = 0.0f;		//Tisho

	#if (defined(SWR_AD8307_LOG)) 				//If it is used the Log amp. AD8307 
	
	float32_t P_FW_dBm, P_BW_dBm;
	float32_t V_FW_Scaled, V_BW_Scaled;
	//float32_t NewSWR;
	
	TRX_VLT_forward = TRX_VLT_forward + (forward - TRX_VLT_forward) / 4;
	TRX_VLT_backward = TRX_VLT_backward + (backward - TRX_VLT_backward) / 4;
	
	//Calculate the Forward values
	P_FW_dBm = ((TRX_VLT_forward*1000) - CALIBRATE.FW_AD8307_OFFS)/(CALIBRATE.FW_AD8307_SLP); 
	V_FW_Scaled = pow(10,(double)((P_FW_dBm-10)/20));							//Calculate in voltage (Vp - 50ohm terminated)
	TRX_PWR_Forward = pow(10,(double)((P_FW_dBm-30)/10));					//Calculate in W
	
	//Calculate the Backward values
	P_BW_dBm = ((TRX_VLT_backward*1000) - CALIBRATE.BW_AD8307_OFFS)/(CALIBRATE.BW_AD8307_SLP); 
	V_BW_Scaled = pow(10,(double)((P_BW_dBm-10)/20));								//Calculate in voltage (Vp - 50ohm terminated)
	TRX_PWR_Backward = pow(10,(double)((P_BW_dBm-30)/10));					//Calculate in W
	
	TRX_SWR = (V_FW_Scaled + V_BW_Scaled) / (V_FW_Scaled - V_BW_Scaled);		//Calculate SWR
	
	//TRX_SWR = TRX_SWR + (NewSWR - TRX_SWR) / 2;
	
	if (TRX_SWR > 10.0f)
			TRX_SWR = 10.0f;
	if (TRX_SWR < 0.0f)
			TRX_SWR = 0.0f;
	
	#else						//if it is used the standard measure (diode rectifier)
	forward = forward / (1510.0f / (0.1f + 1510.0f)); // adjust the voltage based on the voltage divider (0.1 ohm and 510 ohm)
	if (forward < 0.01f)							  // do not measure less than 10mV
	{
		TRX_VLT_forward = 0.0f;
		TRX_VLT_backward = 0.0f;
		TRX_SWR = 1.0f;
	}
	else
	{
		forward += 0.21f;							  // drop on diode
		forward = forward * CALIBRATE.swr_trans_rate; // Transformation ratio of the SWR meter

		backward = backward / (1510.0f / (0.1f + 1510.0f)); // adjust the voltage based on the voltage divider (0.1 ohm and 510 ohm)
		if (backward >= 0.01f)								// do not measure less than 10mV
		{
			backward += 0.21f;								// drop on diode
			backward = backward * CALIBRATE.swr_trans_rate; // Transformation ratio of the SWR meter
		}
		else
			backward = 0.001f;

		TRX_VLT_forward = TRX_VLT_forward + (forward - TRX_VLT_forward) / 2;
		TRX_VLT_backward = TRX_VLT_backward + (backward - TRX_VLT_backward) / 2;
		TRX_SWR = (TRX_VLT_forward + TRX_VLT_backward) / (TRX_VLT_forward - TRX_VLT_backward);

		if (TRX_VLT_backward > TRX_VLT_forward)
			TRX_SWR = 10.0f;
		if (TRX_SWR > 10.0f)
			TRX_SWR = 10.0f;

		TRX_PWR_Forward = (TRX_VLT_forward * TRX_VLT_forward) / 50.0f;
		if (TRX_PWR_Forward < 0.0f)
			TRX_PWR_Forward = 0.0f;
		TRX_PWR_Backward = (TRX_VLT_backward * TRX_VLT_backward) / 50.0f;
		if (TRX_PWR_Backward < 0.0f)
	
		TRX_PWR_Backward = 0.0f;
	}
	#endif
}

//Tisho
//used to controll the calibration of the FW and BW power measurments
void RF_UNIT_MeasureVoltage(void)
{
	float32_t forward = (float32_t)(HAL_ADCEx_InjectedGetValue(&hadc1, ADC_INJECTED_RANK_2)) * TRX_STM32_VREF / 16383.0f;
	float32_t backward = (float32_t)(HAL_ADCEx_InjectedGetValue(&hadc1, ADC_INJECTED_RANK_1)) * TRX_STM32_VREF / 16383.0f;
	//use the TRX_VLT_forward and TRX_VLT_backward global variables 
	//for the raw ADC input voltages 
	//in the TDM_Voltages() the other stuff will be calculated localy 

	static float32_t VLT_forward = 0.0f;		
	static float32_t VLT_backward = 0.0f;
//	TRX_VLT_forward = (float32_t)(HAL_ADCEx_InjectedGetValue(&hadc1, ADC_INJECTED_RANK_2)) * TRX_STM32_VREF / 16383.0f;
//	TRX_VLT_backward= (float32_t)(HAL_ADCEx_InjectedGetValue(&hadc1, ADC_INJECTED_RANK_1)) * TRX_STM32_VREF / 16383.0f;
	VLT_forward = VLT_forward + (forward - VLT_forward) / 10;
	VLT_backward = VLT_backward + (backward - VLT_backward) / 10;
	
	TRX_VLT_forward = VLT_forward;
	TRX_VLT_backward = VLT_backward;
}