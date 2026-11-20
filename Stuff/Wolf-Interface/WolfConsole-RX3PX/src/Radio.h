#pragma once
#include <Arduino.h>
void startRadio(uint8_t sda, uint8_t scl);
bool radioTransmitting();
String radioJson();
int control(const String &op, const String &value, uint32_t owner, String &response);
