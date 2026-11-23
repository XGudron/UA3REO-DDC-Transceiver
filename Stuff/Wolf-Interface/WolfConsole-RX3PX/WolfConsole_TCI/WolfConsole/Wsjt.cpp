#include "Wsjt.h"
#include <math.h>
#include <string.h>

namespace {
class Reader {
  const uint8_t *p;
  size_t remaining;
public:
  bool ok=true;
  Reader(const uint8_t *start,size_t size):p(start),remaining(size){}
  uint8_t byte(){if(!remaining){ok=false;return 0;}--remaining;return *p++;}
  uint32_t u32(){uint32_t n=0;for(unsigned i=0;i<4;++i)n=(n<<8)|byte();return n;}
  uint64_t u64(){uint64_t hi=u32();return (hi<<32)|u32();}
  double real(){const uint64_t bits=u64();double v;memcpy(&v,&bits,sizeof v);return v;}
  bool flag(){const uint8_t v=byte();if(v>1)ok=false;return v==1;}
  bool optionalFlag(){return remaining?flag():false;}
  template<size_t N> void string(char (&out)[N]){
    uint32_t n=u32();if(n==UINT32_MAX)n=0; // Qt null QByteArray
    if(n>=N||n>remaining){ok=false;return;}
    for(unsigned i=0;i<n;i++)if(p[i]<32||p[i]==127){ok=false;return;}
    memcpy(out,p,n);out[n]=0;p+=n;remaining-=n;
  }
};
}
bool readWsjt(const uint8_t *data,size_t len,WsjtEvent &e){
  e={};Reader r(data,len);
  if(r.u32()!=0xadbccbda)return false;
  uint32_t schema=r.u32();if(schema!=2&&schema!=3)return false;
  e.type=r.u32();r.string(e.id);if(!r.ok||!e.id[0])return false;
  if(e.type==0||e.type==3||e.type==6)return true; // Heartbeat, Clear, Close
  if(e.type==1){
    e.hz=r.u64();r.string(e.mode);
    return r.ok&&e.hz<1000000000000ULL&&e.mode[0];
  }
  if(e.type!=2&&e.type!=10)return false;
  e.isNew=r.flag();e.utcMs=r.u32();e.snr=int32_t(r.u32());e.dt=r.real();
  if(!r.ok||e.utcMs>=86400000u||!isfinite(e.dt)||fabs(e.dt)>600||e.snr< -128||e.snr>127)return false;
  if(e.type==2){
    e.hz=r.u32();r.string(e.mode);r.string(e.message);
    e.low=r.optionalFlag();e.offAir=r.optionalFlag();
    return r.ok&&e.message[0]&&e.hz<=100000;
  }
  e.hz=r.u64();e.drift=int32_t(r.u32());r.string(e.call);r.string(e.grid);
  e.power=int32_t(r.u32());e.offAir=r.optionalFlag();strcpy(e.mode,"WSPR");
  return r.ok&&e.hz>0&&e.hz<1000000000000ULL&&e.call[0];
}
