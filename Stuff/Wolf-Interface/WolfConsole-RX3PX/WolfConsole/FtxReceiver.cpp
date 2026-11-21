#include "FtxReceiver.h"
#include "src/ftx/decode.h"
#include "src/ftx/encode.h"
#include <algorithm>
#include "src/fft/kiss_fftr.h"
#include <math.h>
#include <stdlib.h>
#include <string.h>
#ifdef ARDUINO
#include <Arduino.h>
#include <esp_heap_caps.h>
#endif

namespace {
void *workspace(size_t size) {
#ifdef ARDUINO
  return heap_caps_malloc(size, MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
#else
  return malloc(size);
#endif
}
void cooperate() {
#ifdef ARDUINO
  vTaskDelay(1);
#endif
}
struct Memory {
  void *ptr;
  explicit Memory(size_t size):ptr(workspace(size)){}
  ~Memory(){free(ptr);}
};
// Estimate on-tone power from the CRC-checked symbol sequence. A local median
// rejects adjacent stations. Correct exponential-noise median to mean, then
// refer the result from one Hann bin (ENBW 1.5 bins) to 2500 Hz.
int estimateSnr(const ftx_waterfall_t &wf,const ftx_candidate_t &c,
                const ftx_message_t &msg,int nfft,bool ft4) {
  uint8_t tones[FT4_NN];
  if(ft4)ft4_encode(msg.payload,tones);else ft8_encode(msg.payload,tones);
  const int symbols=ft4?FT4_NN:FT8_NN;
  float ratios[FT4_NN];int used=0;
  for(int i=1;i<symbols-1;i++) {
    int t=c.time_offset+i;if(t<0||t>=wf.num_blocks)continue;
    const uint8_t *line=wf.mag+t*wf.block_stride+(c.time_sub*2+c.freq_sub)*wf.num_bins;
    int signal=c.freq_offset+tones[i];uint8_t noise[24];int n=0;
    for(int d=9;d<=20;d++){
      if(c.freq_offset-d>=0)noise[n++]=line[c.freq_offset-d];
      if(c.freq_offset+d<wf.num_bins)noise[n++]=line[c.freq_offset+d];
    }
    if(n<4)continue;
    std::nth_element(noise,noise+n/2,noise+n);
    ratios[used++]=0.6931472f*powf(10.f,0.05f*(int(line[signal])-noise[n/2]));
  }
  if(!used)return -128;
  // Trim extreme symbols before averaging the linear power estimates.
  std::sort(ratios,ratios+used);int trim=used/12;
  float sum=0;for(int i=trim;i<used-trim;i++)sum+=ratios[i];
  float excess=sum/(used-2*trim)-1.0f;
  if(excess<=0.f)return -128;
  return int(lroundf(10.f*log10f(excess*(1.5f*audioRate/nfft)/2500.f)));
}
struct Callsign {uint32_t hash; char name[12];};
Callsign calls[128]{};
uint8_t nextCall;
void saveCall(const char *s, uint32_t hash) {
  for(const auto &c:calls) if(c.name[0]&&hash==c.hash&&!strcmp(c.name,s))return;
  auto &c=calls[nextCall++ % 128]; c.hash=hash;
  snprintf(c.name,sizeof c.name,"%s",s);
}
bool findCall(ftx_callsign_hash_type_t type,uint32_t hash,char *out) {
  unsigned shift=type==FTX_CALLSIGN_HASH_10_BITS?12:type==FTX_CALLSIGN_HASH_12_BITS?10:0;
  const char *match=nullptr;
  for(const auto &c:calls)if(c.name[0]&&(c.hash>>shift)==hash){
    if(match&&strcmp(c.name,match))return false;
    match=c.name;
  }
  if(match)strcpy(out,match);
  return match!=nullptr;
}
}

// Decode one complete UTC slot. All scratch arrays live outside the task stack.
bool decodeSlot(const int16_t *pcm, LocalMode mode, uint32_t utcMs,
                int64_t signedDial, DecodeSink publish, bool (*keepRunning)()) {
  bool ft4=mode==LocalMode::ft4;
  int samples=ft4?75000:150000, symbol=ft4?480:1600;
  const int nfft=symbol*2, hop=symbol/2, blocks=samples/symbol;
  const float period=float(symbol)/audioRate;
  const int minBin=int(100*period), bins=int(4000*period)+1-minBin;
  size_t cfgSize=0;kiss_fftr_alloc(nfft,0,nullptr,&cfgSize);
  Memory config(cfgSize), spec(size_t(blocks)*4*bins), real(sizeof(float)*nfft),
         win(sizeof(float)*nfft), complex(sizeof(kiss_fft_cpx)*(nfft/2+1));
  if(!config.ptr||!spec.ptr||!win.ptr||!complex.ptr||!real.ptr)return false;
  auto fft=kiss_fftr_alloc(nfft,0,config.ptr,&cfgSize);
  auto *mag=static_cast<uint8_t*>(spec.ptr);
  auto *temp=static_cast<float*>(real.ptr), *window=static_cast<float*>(win.ptr);
  auto *binsOut=static_cast<kiss_fft_cpx*>(complex.ptr);
  for(int i=0;i<nfft;i++)window[i]=(1.f-cosf(6.28318530718f*i/nfft))/(32768.f*nfft);
  ftx_waterfall_t wf{};
  wf.max_blocks=wf.num_blocks=blocks;wf.num_bins=bins;wf.time_osr=wf.freq_osr=2;
  wf.block_stride=4*bins;wf.mag=mag;wf.protocol=ft4?FTX_PROTOCOL_FT4:FTX_PROTOCOL_FT8;
  for(int sub=0;sub<2*blocks;sub++) {
    const int first=(sub+1)*hop-nfft;
    for(int i=0;i<nfft;i++)temp[i]=first+i<0?0.f:window[i]*pcm[first+i];
    kiss_fftr(fft,temp,binsOut);
    for(int f=0;f<2;f++)for(int i=0;i<bins;i++) {
      const auto z=binsOut[2*(minBin+i)+f];
      float scaled=240.f+20.f*log10f(1e-12f+z.r*z.r+z.i*z.i);
      *mag++=uint8_t(fminf(255.f,fmaxf(0.f,scaled)));
    }
    if(sub%12==0){if(keepRunning&&!keepRunning())return true;cooperate();}
  }
  ftx_candidate_t candidates[48];
  int n=ftx_find_candidates(&wf,48,candidates,10);
  ftx_message_t seen[48]; int used=0;
  ftx_callsign_hash_interface_t cache{findCall,saveCall};
  for(int i=0;i<n;i++) {
    if(keepRunning&&!keepRunning())return true;
    if(i%3==0)cooperate();
    const auto &c=candidates[i];ftx_message_t msg{};ftx_decode_status_t status{};
    if(!ftx_decode_candidate(&wf,&c,18,&msg,&status))continue;
    bool duplicate=false;
    for(int j=0;j<used;j++)if(!memcmp(msg.payload,seen[j].payload,sizeof msg.payload))duplicate=true;
    if(duplicate)continue;
    LocalDecode row{};ftx_message_offsets_t offsets;
    if(ftx_message_decode(&msg,&cache,row.message,&offsets)!=FTX_MESSAGE_RC_OK)continue;
    seen[used++]=msg;row.utcMs=utcMs;
    row.snr=estimateSnr(wf,c,msg,nfft,ft4);
    row.df=lroundf((minBin+c.freq_offset+0.5f*c.freq_sub)/period);
    // Window centre relative to nominal first on-air symbol (FT8 0.5 s, FT4 0.5 s ramp).
    row.dt=(c.time_offset+0.5f*c.time_sub-0.5f)*period-0.5f;
    if(signedDial>0)row.rf=signedDial+row.df;
    else if(signedDial< -row.df)row.rf=-signedDial-row.df;
    strcpy(row.mode,ft4?"FT4":"FT8");publish(row);
  }
  return true;
}
