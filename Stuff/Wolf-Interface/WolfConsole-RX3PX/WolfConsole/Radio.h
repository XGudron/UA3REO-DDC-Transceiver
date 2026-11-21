#pragma once
#include <Arduino.h>
void startRadio(uint8_t sda, uint8_t scl);
bool radioTransmitting();
String radioJson();
int control(const String &op, const String &value, uint32_t owner, String &response);
int64_t radioAudioDial();

uint32_t digitalEpoch();

struct TciRadio {
  bool online;
  int active, mode, tx, tune, split, drive;
  int64_t hz[2];
  int signal;
  uint32_t signalTime;
};
TciRadio radioForTci();
