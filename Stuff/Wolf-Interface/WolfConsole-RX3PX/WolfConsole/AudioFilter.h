#pragma once
#include <stdint.h>
// 20 kHz ADC to 10 kHz PCM. Symmetric 95-tap Kaiser FIR in Q15.
class AudioFilter {
  static constexpr int taps = 95;
  const int16_t half[48]={0,1,2,-2,-4,2,8,0,-13,-5,18,15,-22,-30,20,49,-9,-71,-13,91,50,-102,-102,97,166,-65,-236,0,299,105,-341,-253,342,439,-278,-656,122,889,159,-1120,-626,1328,1418,-1494,-3054,1601,10285,14748};
  int16_t ring[taps*2]{};
  int pos=0, phase=0;
  int32_t baseline=2048<<12;
public:
  bool push(unsigned raw,int16_t &out) {
    baseline+=int32_t(raw)-(baseline>>12);
    int16_t sample=int16_t((int32_t(raw)-(baseline>>12))*8);
    ring[pos]=ring[pos+taps]=sample;
    if(++pos==taps)pos=0;
    phase^=1;if(phase)return false;
    const int16_t *oldest=ring+pos;
    int64_t acc=int32_t(half[47])*oldest[47];
    for(int i=0;i<47;i++)acc+=int32_t(half[i])*(oldest[i]+oldest[taps-1-i]);
    int32_t value=acc>>15;
    out=int16_t(value>32767?32767:value< -32768?-32768:value);return true;
  }
};
