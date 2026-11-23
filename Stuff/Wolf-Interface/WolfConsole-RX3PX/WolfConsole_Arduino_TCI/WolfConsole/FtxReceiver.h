#pragma once
#include <stdint.h>
enum class LocalMode : uint8_t { off, ft8, ft4 };
struct LocalDecode {
  uint32_t utcMs,epoch;
  int32_t df;
  int16_t snr; // approximate S/N in a 2500 Hz reference bandwidth, or -128
  float dt;
  uint64_t rf;
  char mode[4];
  char message[64];
};
using DecodeSink = void (*)(const LocalDecode &);
constexpr int audioRate = 10000;
bool decodeSlot(const int16_t *pcm, LocalMode mode, uint32_t utcMs,
                int64_t signedDial, DecodeSink publish, bool (*keepRunning)()=nullptr);
