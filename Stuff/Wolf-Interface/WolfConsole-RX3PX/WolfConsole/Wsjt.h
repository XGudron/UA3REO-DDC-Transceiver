#pragma once
#include <stddef.h>
#include <stdint.h>

// Incoming WSJT-X datagrams use the big-endian Qt QDataStream representation.
struct WsjtEvent {
  uint32_t type, utcMs;
  int32_t snr, drift, power;
  double dt;
  uint64_t hz; // Status: dial Hz. Decode: audio offset Hz. WSPR: absolute Hz.
  bool isNew, low, offAir;
  char id[65], mode[17], message[129], call[25], grid[9];
};
bool readWsjt(const uint8_t *bytes, size_t length, WsjtEvent &event);
