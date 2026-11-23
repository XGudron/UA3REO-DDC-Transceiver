#include "AudioRx.h"
#include "AudioFilter.h"
#include "PcmStream.h"
#include "Radio.h"
#include <esp_adc/adc_continuous.h>
#include <esp_heap_caps.h>
#include <sys/time.h>
#include <atomic>
#include <WiFi.h>

namespace {
constexpr size_t adcFrameBytes=2*frameSamples*SOC_ADC_DIGI_RESULT_BYTES;
adc_continuous_handle_t adc;
adc_channel_t channel;
uint8_t audioPin;
LocalMode selectedMode;
const char *captureState="off";
bool streamReady,decoderReady;
QueueHandle_t freeSlots, jobs, messages;
struct Slot {int16_t *pcm;uint32_t utcMs,epoch;int64_t dial;};
Slot slots[2]{};
std::atomic<uint32_t> lastLevel{0},clipCount{0},slotCount{0},skipped{0},cpuMs{0},overflows{0};
std::atomic<bool> decoding{false},dspError{false};
bool IRAM_ATTR overflow(adc_continuous_handle_t,const adc_continuous_evt_data_t*,void*) {
  overflows.fetch_add(1,std::memory_order_relaxed);return false;
}
int64_t wallUs(){timeval tv;gettimeofday(&tv,nullptr);return int64_t(tv.tv_sec)*1000000+tv.tv_usec;}
uint32_t workerEpoch; // accessed only by the DSP worker
bool currentSession(){return workerEpoch&&workerEpoch==digitalEpoch();}
void publish(const LocalDecode &row){
  if(currentSession()){LocalDecode tagged=row;tagged.epoch=workerEpoch;xQueueSend(messages,&tagged,0);}
}
void decodeTask(void*) {
  int index;
  while(true)if(xQueueReceive(jobs,&index,portMAX_DELAY)==pdTRUE){
    auto &s=slots[index];workerEpoch=s.epoch;
    if(currentSession()){
      decoding=true;uint32_t start=millis();
      dspError=!decodeSlot(s.pcm,selectedMode,s.utcMs,s.dial,publish,currentSession);
      cpuMs=millis()-start;if(currentSession())++slotCount;decoding=false;
    }
    xQueueSend(freeSlots,&index,portMAX_DELAY);
  }
}
bool setupDecoder() {
  if(selectedMode==LocalMode::off)return false;
  const int length=selectedMode==LocalMode::ft4?75000:150000;
  for(auto &s:slots)s.pcm=(int16_t*)heap_caps_calloc(length,sizeof(int16_t),MALLOC_CAP_SPIRAM|MALLOC_CAP_8BIT);
  jobs=xQueueCreate(2,sizeof(int));freeSlots=xQueueCreate(2,sizeof(int));messages=xQueueCreate(40,sizeof(LocalDecode));
  if(slots[0].pcm&&slots[1].pcm&&jobs&&freeSlots&&messages){
    for(int i=0;i<2;i++)xQueueSend(freeSlots,&i,0);
    if(xTaskCreatePinnedToCore(decodeTask,"wolf-ftx",16384,nullptr,1,nullptr,1)==pdPASS)return true;
  }
  for(auto &s:slots){free(s.pcm);s.pcm=nullptr;}
  if(jobs)vQueueDelete(jobs);
  if(freeSlots)vQueueDelete(freeSlots);
  if(messages)vQueueDelete(messages);
  jobs=freeSlots=messages=nullptr;return false;
}
// Only the capture task owns this slot assembler. A finished buffer is handed
// to the decoder by queue; it cannot be reused until the worker returns it.
class Assembler {
  int64_t epoch=-1;int current=-1,written=0;
  const int length=selectedMode==LocalMode::ft4?75000:150000;
  void finish() {
    if(current<0)return;
    if(written>=length-600){
      memset(slots[current].pcm+written,0,(length-written)*sizeof(int16_t));
      if(slots[current].dial!=radioAudioDial())slots[current].dial=0;
      xQueueSend(jobs,&current,0);
    }else{xQueueSend(freeSlots,&current,0);++skipped;}
    current=-1;
  }
public:
  void reset(){
    if(current>=0){xQueueSend(freeSlots,&current,0);current=-1;++skipped;}
    epoch=-1;written=0;
  }
  void append(const AudioFrame &frame,int64_t firstUs,uint32_t session) {
    int64_t sample=firstUs/100;
    for(unsigned from=0;from<frameSamples;){
      int64_t id=sample/length;int pos=int(sample%length);
      unsigned n=std::min<unsigned>(frameSamples-from,length-pos);
      if(epoch>=0&&id<epoch){sample+=n;from+=n;continue;}
      if(epoch!=id){
        finish();epoch=id;written=0;
        if(pos<=600&&xQueueReceive(freeSlots,&current,0)==pdTRUE){
          slots[current].utcMs=uint32_t((id*length/10)%86400000);
          slots[current].epoch=session;slots[current].dial=radioAudioDial();
          memset(slots[current].pcm,0,pos*sizeof(int16_t));written=pos;
        }else{current=-1;if(pos<=600)++skipped;}
      }
      if(current>=0){
        int take=std::min<int>(n,length-written);
        memcpy(slots[current].pcm+written,frame.pcm+from,take*sizeof(int16_t));written+=take;
      }
      sample+=n;from+=n;
    }
  }
};
void captureTask(void*) {
  alignas(4) uint8_t dma[adcFrameBytes];
  AudioFilter filter;Assembler assembler;AudioFrame frame;
  unsigned filled=0,peak=0,clips=0;int64_t nextUs=0;uint32_t lastOverflow=0,lastEpoch=0;
  for(;;) {
    uint32_t bytes=0;
    esp_err_t rc=adc_continuous_read(adc,dma,sizeof dma,&bytes,100);
    if(rc!=ESP_OK){assembler.reset();nextUs=0;filled=0;vTaskDelay(1);continue;}
    if(lastOverflow!=overflows.load()){lastOverflow=overflows.load();assembler.reset();nextUs=0;filled=0;}
    const int64_t end=wallUs();auto *raw=reinterpret_cast<const adc_digi_output_data_t*>(dma);
    const size_t records=bytes/SOC_ADC_DIGI_RESULT_BYTES;
    for(size_t i=0;i<records;i++){
      const auto &r=raw[i].type2;
      if(r.channel!=channel)continue;
      unsigned sample=r.data;
      if(sample<12||sample>4083)++clips;
      int16_t value;if(!filter.push(sample,value))continue;
      peak=std::max(peak,unsigned(abs(int(value))));frame.pcm[filled++]=value;
      if(filled!=frameSamples)continue;
      lastLevel=peak;clipCount+=clips;sendPcmFrame(frame);
      uint32_t session=digitalEpoch();
      if(session!=lastEpoch){assembler.reset();nextUs=0;lastEpoch=session;}
      if(session&&decoderReady&&utcValid()){
        int64_t observed=end-int64_t(records-1-i)*50-frameSamples*100;
        if(!nextUs||llabs(observed-nextUs)>100000){assembler.reset();nextUs=observed;}
        else nextUs+=(observed-nextUs)/32;
        assembler.append(frame,nextUs,session);nextUs+=frameSamples*100;
      }
      filled=0;peak=clips=0;
    }
  }
}
}
bool utcValid(){return time(nullptr)>=1704067200;}
bool readLocalDecode(LocalDecode &row){while(messages&&xQueueReceive(messages,&row,0)==pdTRUE)if(row.epoch==digitalEpoch())return true;return false;}
void startAudio(uint8_t gpio,bool enabled,LocalMode mode) {
  audioPin=gpio;selectedMode=mode;if(!enabled)return;
  adc_unit_t unit;
  if(adc_continuous_io_to_channel(gpio,&unit,&channel)!=ESP_OK||unit!=ADC_UNIT_1){captureState="pin";return;}
  adc_continuous_handle_cfg_t init{};init.max_store_buf_size=adcFrameBytes*8;init.conv_frame_size=adcFrameBytes;init.flags.flush_pool=1;
  if(adc_continuous_new_handle(&init,&adc)!=ESP_OK){captureState="dma";return;}
  adc_digi_pattern_config_t pattern{};pattern.atten=ADC_ATTEN_DB_12;pattern.channel=channel;pattern.unit=unit;pattern.bit_width=12;
  adc_continuous_config_t config{};config.pattern_num=1;config.adc_pattern=&pattern;config.sample_freq_hz=20000;
  config.conv_mode=ADC_CONV_SINGLE_UNIT_1;config.format=ADC_DIGI_OUTPUT_FORMAT_TYPE2;
  adc_continuous_evt_cbs_t cb{};cb.on_pool_ovf=overflow;
  if(adc_continuous_config(adc,&config)!=ESP_OK||adc_continuous_register_event_callbacks(adc,&cb,nullptr)!=ESP_OK||
     adc_continuous_start(adc)!=ESP_OK){adc_continuous_deinit(adc);captureState="adc";return;}
  WiFi.setSleep(false);
  decoderReady=setupDecoder();streamReady=startPcmStream();
  if(xTaskCreatePinnedToCore(captureTask,"wolf-adc",6144,nullptr,5,nullptr,1)!=pdPASS){
    adc_continuous_stop(adc);adc_continuous_deinit(adc);captureState="task";return;
  }
  captureState="ok";
}
String audioStatusJson(){
  char out[384];snprintf(out,sizeof out,
    "{\"capture\":\"%s\",\"digi\":%u,\"gpio\":%u,\"mode\":%u,\"stream\":%d,\"ready\":%d,\"clock\":%d,\"busy\":%d,\"error\":%d,\"peak\":%u,\"clips\":%lu,\"slots\":%lu,\"skip\":%lu,\"overflow\":%lu,\"ms\":%lu,\"clients\":%u}",
    captureState,unsigned(digitalEpoch()!=0),audioPin,unsigned(selectedMode),streamReady,decoderReady,utcValid(),int(decoding.load()),int(dspError.load()),
    unsigned(lastLevel.load()),(unsigned long)clipCount.load(),(unsigned long)slotCount.load(),(unsigned long)skipped.load(),
    (unsigned long)overflows.load(),(unsigned long)cpuMs.load(),audioListeners());
  return out;
}
