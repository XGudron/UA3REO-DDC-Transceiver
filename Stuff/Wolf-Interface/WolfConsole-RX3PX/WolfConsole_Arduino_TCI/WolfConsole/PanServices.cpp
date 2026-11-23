#include "PanServices.h"
#include <WiFi.h>
#include <esp_heap_caps.h>
#include <esp_ota_ops.h>
#include <esp_timer.h>
#include <freertos/idf_additions.h>
#include <atomic>
#include <ctype.h>
#include <math.h>

namespace {
struct Spot { uint32_t seen; uint64_t hz; char call[24]; };
constexpr unsigned capacity=256;
constexpr uint32_t lifetime=300000;
Spot *spots;
unsigned nextSlot;
SemaphoreHandle_t spotLock;
SkimmerConfig source{};
std::atomic<int> linkState{0}; // 0 disabled, 1 connecting, 2 connected, 3 task/storage error

void acceptSpot(char *line){
  if(strncmp(line,"DX de ",6))return;
  char *p=strchr(line+6,':');
  if(!p)return;
  char *end;
  double khz=strtod(p+1,&end);
  if(end==p+1||!isspace((unsigned char)*end)||!isfinite(khz)||khz<1||khz>99999999)return;
  while(isspace((unsigned char)*end))++end;
  char *start=end;
  while(*end&&!isspace((unsigned char)*end)){
    if(!isalnum((unsigned char)*end)&&*end!='/')return;
    *end=toupper((unsigned char)*end);++end;
  }
  size_t n=end-start;
  if(n<3||n>=sizeof(Spot::call))return;
  *end=0;
  const uint64_t hz=llround(khz*1000.);
  const uint32_t now=millis();
  xSemaphoreTake(spotLock,portMAX_DELAY);
  unsigned pos=nextSlot;
  for(unsigned i=0;i<capacity;i++){
    if(spots[i].hz==hz&&!strcmp(spots[i].call,start)){pos=i;break;}
  }
  if(pos==nextSlot)nextSlot=(nextSlot+1)%capacity;
  spots[pos].hz=hz;spots[pos].seen=now;memcpy(spots[pos].call,start,n+1);
  xSemaphoreGive(spotLock);
}

void skimmerTask(void*){
  for(;;){
    NetworkClient client;
    linkState=1;
    if(client.connect(source.host,source.port,1500)){
      linkState=2;
      if(source.login[0])client.printf("%s\r\n",source.login);
      char line[256];unsigned used=0;bool overflow=false;
      uint8_t telnet=0,verb=0;
      while(client.connected()||client.available()){
        uint8_t buf[512];
        int n=client.available()?client.read(buf,sizeof buf):0;
        for(int i=0;i<n;i++){
          uint8_t c=buf[i];
          if(telnet==3){if(c==255)telnet=4;continue;}
          if(telnet==4){telnet=c==240?0:3;continue;}
          if(telnet==2){
            if(verb==251||verb==253){uint8_t reply[]={255,uint8_t(verb==251?254:252),c};client.write(reply,3);}
            telnet=0;continue;
          }
          if(telnet==1){verb=c;telnet=c==250?3:(c>=251&&c<=254?2:0);continue;}
          if(c==255){telnet=1;continue;}
          if(c=='\n'){
            line[used]=0;
            if(!overflow){char *dx=strstr(line,"DX de ");if(dx)acceptSpot(dx);}
            used=0;overflow=false;
          }else if(c>=32&&c<127){
            if(used<sizeof(line)-1)line[used++]=char(c);else overflow=true;
          }
        }
        vTaskDelay(pdMS_TO_TICKS(n>0?2:50));
      }
    }
    client.stop();linkState=1;vTaskDelay(pdMS_TO_TICKS(10000));
  }
}
String memoryPair(uint32_t caps){
  size_t total=heap_caps_get_total_size(caps),free=heap_caps_get_free_size(caps);
  return "["+String(total-free)+","+String(total)+"]";
}
}
String resourceJson(){
  static int64_t last=0;
  static uint32_t idle[2]={};
  static int load[2]={-1,-1};
  int64_t now=esp_timer_get_time(),elapsed=now-last;
#if configGENERATE_RUN_TIME_STATS && configUSE_TRACE_FACILITY && CONFIG_FREERTOS_RUN_TIME_STATS_USING_ESP_TIMER
  if(!last||elapsed>=1000000){
    for(int core=0;core<2;core++){
      TaskStatus_t s{};
      vTaskGetInfo(xTaskGetIdleTaskHandleForCore(core),&s,pdFALSE,eInvalid);
      uint32_t counter=s.ulRunTimeCounter;
      if(last&&elapsed<120000000){
        uint32_t delta=counter-idle[core];
        load[core]=100-int((100ULL*min(delta,uint32_t(elapsed))+elapsed/2)/elapsed);
      }else load[core]=-1;
      idle[core]=counter;
    }
    last=now;
  }
#endif
  const auto *partition=esp_ota_get_running_partition();
  static size_t bytes=ESP.getSketchSize();
  return "{\"cpu\":["+String(load[0])+","+String(load[1])+"],\"ram\":"+memoryPair(MALLOC_CAP_INTERNAL|MALLOC_CAP_8BIT)+",\"psram\":"+memoryPair(MALLOC_CAP_SPIRAM|MALLOC_CAP_8BIT)+",\"app\":["+String(bytes)+","+String(partition?partition->size:0)+"],\"mhz\":"+String(ESP.getCpuFreqMHz())+"}";
}
void startPanServices(WebServer &s,const SkimmerConfig &config){
  source=config;
  if(source.host[0]){
    spots=(Spot*)heap_caps_calloc(capacity,sizeof(Spot),MALLOC_CAP_SPIRAM|MALLOC_CAP_8BIT);
    spotLock=xSemaphoreCreateMutex();
    if(!spots||!spotLock||xTaskCreate(skimmerTask,"skimmer",4096,nullptr,1,nullptr)!=pdPASS)linkState=3;
  }
  s.on("/api/spots",HTTP_GET,[&s](){
    uint64_t low=strtoull(s.arg("lo").c_str(),nullptr,10),high=strtoull(s.arg("hi").c_str(),nullptr,10);
    String j="{\"state\":"+String(linkState.load())+",\"spots\":[";
    bool comma=false;uint32_t now=millis();
    if(spots&&spotLock&&high>low&&high-low<=400000&&xSemaphoreTake(spotLock,pdMS_TO_TICKS(30))){
      for(unsigned i=0;i<capacity;i++){
        const auto &r=spots[i];
        if(!r.hz||r.hz<low||r.hz>high||now-r.seen>=lifetime)continue;
        char b[90];snprintf(b,sizeof b,"%s[%llu,\"%s\",%lu]",comma?",":"",(unsigned long long)r.hz,r.call,(unsigned long)((now-r.seen)/1000));
        j+=b;comma=true;
      }
      xSemaphoreGive(spotLock);
    }
    s.sendHeader("Cache-Control","no-store");s.send(200,"application/json",j+"]}");
  });
}
