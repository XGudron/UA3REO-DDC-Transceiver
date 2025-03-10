/*
  Wolf Interface Example Project

  This sketch demonstrates basic functionality for develop user-space devices, which communicates with TRX Wolf

  https://github.com/XGudron/UA3REO-DDC-Transceiver
*/

#include "Wolf.h"

// the setup function runs once when you press reset or power the board
void setup() {
	Serial.begin(115200); // enable serial port for debugging

	WOLF_init(); // init TRX Wolf communication
}

// the loop function runs over and over again forever
void loop() {
	Serial.println("TRX Info:");
	Serial.println("Current VFO: " + String(TRX_WOLF.CurrentVFO == TRX_SELECTED_VFO_A ? "A" : "B"));
	Serial.println("VFO A Frequency: " + String(TRX_WOLF.VFO_A_Frequency));
	Serial.println("VFO A Mode: " + WOLF_modeToString(TRX_WOLF.VFO_A_Mode));
	Serial.println("VFO A Mode is CW? " + String(TRX_WOLF.VFO_A_Mode == TRX_MODE_CW ? "Yes" : "No"));
	Serial.println("VFO B Frequency: " + String(TRX_WOLF.VFO_B_Frequency));
	Serial.println("VFO B Mode: " + WOLF_modeToString(TRX_WOLF.VFO_B_Mode));
	Serial.println("In TX? " + String(TRX_WOLF.TX ? "Yes" : "No"));
	Serial.println("In Tune? " + String(TRX_WOLF.Tune ? "Yes" : "No"));

	String CAT_Answer = WOLF_catCommand("SM2"); // run CAT command and get answer
	Serial.println("CAT Answer (S-Meter dBm RX1): " + CAT_Answer);

	WOLF_setMessage("Hello World! " + String(millis())); // set message over FFT, may be disabled if not set

	Serial.println("---");

	delay(1000);
}
