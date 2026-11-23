#pragma once
#include <stdint.h>
constexpr unsigned frameSamples=100; // 10 ms of 10 kHz mono
struct AudioFrame { int16_t pcm[frameSamples]; };
bool startPcmStream();
void sendPcmFrame(const AudioFrame &frame);
unsigned audioListeners();
