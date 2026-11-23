#pragma once
#include <Arduino.h>
#include "FtxReceiver.h"
void startAudio(uint8_t gpio, bool enabled, LocalMode mode);
String audioStatusJson();
bool readLocalDecode(LocalDecode &row);
bool utcValid();
