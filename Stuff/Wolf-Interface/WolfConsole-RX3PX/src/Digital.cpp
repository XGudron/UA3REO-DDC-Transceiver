#include "Digital.h"
#include "Wsjt.h"
#include <WiFiUdp.h>
#include <WiFi.h>
namespace {
constexpr uint16_t port=2237;
constexpr unsigned maxRows=32;
constexpr uint32_t aliveMs=100000, historyMs=15*60000;
WiFiUDP udp;
bool listening;
uint32_t lastBind,revision=1;
struct Source {char id[65], mode[17];uint32_t ip,last;uint64_t dial;bool active;};
Source sources[4]{};
struct Row {
  uint32_t at,utc;int32_t snr;double dt;int64_t df;uint64_t rf;
  uint8_t source;bool low;
  char mode[17],msg[129],call[25];
};
Row rows[maxRows];unsigned count;
void eraseRows(unsigned source){
  unsigned kept=0;
  for(unsigned i=0;i<count;i++)if(rows[i].source!=source){
    if(kept!=i)rows[kept]=rows[i];++kept;
  }
  if(kept!=count){count=kept;++revision;}
}
unsigned slot(const WsjtEvent &e,uint32_t ip,uint32_t now){
  unsigned oldest=0;
  for(unsigned i=0;i<4;i++){
    if(sources[i].id[0]&&sources[i].ip==ip&&!strcmp(sources[i].id,e.id))return i;
    if(!sources[i].id[0] || (sources[oldest].id[0]&&now-sources[i].last>now-sources[oldest].last))oldest=i;
  }
  eraseRows(oldest);auto &s=sources[oldest];s={};s.ip=ip;
  strlcpy(s.id,e.id,sizeof s.id);return oldest;
}
const char *modeOf(const WsjtEvent &e,const Source &s){
  if(e.type==10)return "WSPR";
  if(!strcmp(e.mode,"~"))return "FT8";
  if(!strcmp(e.mode,"+"))return "FT4";
  if(!strcmp(e.mode,"#"))return "JT65";
  if(!strcmp(e.mode,"@"))return "JT9";
  if(s.mode[0])return s.mode;
  return e.mode[0]?e.mode:"?";
}
void consume(const WsjtEvent &e,uint32_t ip,uint32_t now){
  const bool decode=e.type==2||e.type==10;
  if(decode&&(!e.isNew||e.offAir))return;
  unsigned i=slot(e,ip,now);auto &src=sources[i];
  if(src.id[0] && now-src.last>aliveMs){src.dial=0;src.mode[0]=0;src.active=false;}
  src.last=now;
  if(e.type==0)return;
  if(e.type==6||e.type==3){
    eraseRows(i);if(e.type==6)src={};return;
  }
  src.active=true;
  if(e.type==1){src.dial=e.hz;strlcpy(src.mode,e.mode,sizeof src.mode);return;}
  Row row{};row.at=now;row.utc=e.utcMs;row.snr=e.snr;row.dt=e.dt;row.low=e.low;row.source=i;
  strlcpy(row.mode,modeOf(e,src),sizeof row.mode);
  if(e.type==10){
    row.rf=e.hz;row.df=src.dial?int64_t(e.hz)-int64_t(src.dial):INT64_MIN;
    strlcpy(row.call,e.call,sizeof row.call);
    snprintf(row.msg,sizeof row.msg,"%s %s %ld dBm · drift %ld Hz/min",e.call,e.grid,(long)e.power,(long)e.drift);
  }else{
    row.df=(int64_t)e.hz;row.rf=src.dial?src.dial+e.hz:0;
    strlcpy(row.msg,e.message,sizeof row.msg);
  }
  for(unsigned k=0;k<count;k++){const Row &old=rows[k];
    if(old.source==i&&old.utc==row.utc&&old.df==row.df&&old.rf==row.rf&&!strcmp(old.msg,row.msg)&&!strcmp(old.mode,row.mode))return;
  }
  if(count==maxRows){memmove(rows,rows+1,sizeof(Row)*(maxRows-1));--count;}
  rows[count++]=row;++revision;
}
void quote(String &out,const char *s){out+='"';for(;*s;s++){if(*s=='"'||*s=='\\')out+='\\';out+=*s;}out+='"';}
void answer(WebServer &server){
  const uint32_t now=millis();bool active=false;
  for(const auto &s:sources)if(s.active&&now-s.last<aliveMs)active=true;
  String j;j.reserve(320+count*180);
  j="{\"listening\":"+String(listening?"true":"false")+",\"active\":"+(active?"true":"false")+",\"rev\":"+String(revision);
  if(server.arg("v")!=String(revision)){
    j+=",\"rows\":[";
    for(unsigned k=count;k>0;k--){
      const Row &r=rows[k-1];if(k!=count)j+=',';
      char nums[155],rf[24],df[24];
      if(r.rf)snprintf(rf,sizeof rf,"%llu",(unsigned long long)r.rf);else strcpy(rf,"null");
      if(r.df!=INT64_MIN)snprintf(df,sizeof df,"%lld",(long long)r.df);else strcpy(df,"null");
      snprintf(nums,sizeof nums,"{\"utc\":%lu,\"snr\":%ld,\"dt\":%.2f,\"df\":%s,\"rf\":%s,\"low\":%s,\"mode\":",(unsigned long)r.utc,(long)r.snr,r.dt,df,rf,r.low?"true":"false");
      j+=nums;quote(j,r.mode);j+=",\"msg\":";quote(j,r.msg);j+=",\"call\":";quote(j,r.call);
      j+=",\"id\":";quote(j,sources[r.source].id);j+='}';
    }
    j+=']';
  }
  j+='}';server.sendHeader("Cache-Control","no-store");server.send(200,"application/json",j);
}
}
void beginDigital(WebServer &s){
  listening=udp.begin(port);lastBind=millis();
  s.on("/api/digital",HTTP_GET,[&s](){answer(s);});
}
void pollDigital(){
  const uint32_t now=millis();
  if(!listening&&now-lastBind>5000){listening=udp.begin(port);lastBind=now;}
  unsigned expired=0;while(expired<count&&now-rows[expired].at>=historyMs)++expired;
  if(expired){count-=expired;memmove(rows,rows+expired,count*sizeof(Row));++revision;}
  if(!listening)return;
  for(unsigned k=0;k<12;k++){
    int n=udp.parsePacket();if(n<=0)break;
    uint8_t buf[1024];
    if(n>(int)sizeof buf){udp.clear();continue;}
    const uint32_t ip=uint32_t(udp.remoteIP());
    if(udp.read(buf,n)!=n)continue;WsjtEvent e;
    if(readWsjt(buf,n,e))consume(e,ip,now);
  }
}
