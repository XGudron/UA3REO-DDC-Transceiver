#include "Tci.h"
#include "Radio.h"
#include "DeviceSettings.h"
#include <WiFi.h>
#include <WebSocketsServer.h>
#include <base64.h>
#include <esp_random.h>

namespace {
WebSocketsServer ws(50001);
struct Client {
  bool live=false, panel=false, force=true, nativeHold=false;
  uint32_t owner=0, meterPeriod=0, meterAt=0, lastMeasurement=0;
  String request,holdToken;
  TciRadio previous{};
};
Client peers[WEBSOCKETS_SERVER_CLIENT_MAX];
const char *const names[]={"LSB","USB","CW","NFM","WFM","AM","SAM","SAM-L","SAM-U","DIGL","DIGU","IQ","LOOP","RTTY","DSB"};
const char *const setModes[]={"LSB","USB","CW","NFM","SAM","DIGL","IQ","DIGU"};
const char *const modeCodes[]={"1","2","3","4","5","6","8","9"};
const char *boolean(int x){return x?"true":"false";}
void sendText(uint8_t n,const String &s){ws.sendTXT(n,s.c_str(),s.length());}
// Keep one standard TCI command per WebSocket text message for older clients.
void sendCommands(uint8_t n,const String &commands){
  unsigned start=0;
  while(start<commands.length()){
    int end=commands.indexOf(';',start);
    if(end<0)break;
    ws.sendTXT(n,commands.c_str()+start,end-start+1);
    start=end+1;
  }
}
bool numeric(const String &s,size_t max){
  if(!s.length()||s.length()>max)return false;
  for(char c:s)if(c<'0'||c>'9')return false;
  return true;
}
// The console sends printable CAT arguments in padded RFC 4648 Base64.
bool decodeArgument(const String &encoded,String &value){
  static const char alphabet[]="ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
  if(encoded.length()>100 || encoded.length()%4)return false;
  char buffer[76];
  unsigned size=0, bits=0, pending=0, padding=0;
  for(char c:encoded){
    if(c=='='){++padding;continue;}
    const char *digit=strchr(alphabet,c);
    if(c==0 || !digit || padding)return false;
    pending=(pending<<6)|unsigned(digit-alphabet);
    bits+=6;
    if(bits>=8){
      bits-=8;
      unsigned byte=pending>>bits;
      if(byte<32 || byte>126)return false;
      buffer[size++]=char(byte);
      pending&=(1u<<bits)-1;
    }
  }
  if(padding>2 || bits!=2*padding || pending)return false;
  buffer[size]=0;
  value=buffer;
  return value.length()==size;
}
void answer(uint8_t n,int status,const String &msg){
  auto &id=peers[n].request;
  if(id.length())sendText(n,"wolf_ack:"+id+","+String(status)+","+base64::encode(msg)+";");
  else if(status>=400)sendText(n,"wolf_error:"+String(status)+","+base64::encode(msg)+";");
  id="";
}
void action(uint8_t n,const String &op,const String &value,bool standardHold=false){
  Client &p=peers[n]; String msg;
  int code=restartingDevice()?503:control(op,value,p.owner,msg);
  if(code<300){
    if(op=="ptt"||op=="tune")p.nativeHold=standardHold;
    if(op=="rx"||op=="release"){p.nativeHold=false;p.holdToken="";}
  }
  if(op=="renew"&&code>=400)p.holdToken="";
  answer(n,code,msg);
}
void syncState(uint8_t n,const TciRadio &r,bool all){
  Client &p=peers[n];auto &old=p.previous;
  String s;
  if(all||r.online!=old.online)s+=r.online?"start;":"stop;";
  for(int i=0;i<2;i++)if(r.hz[i]>0&&(all||old.hz[i]!=r.hz[i]))
    s+="vfo:0,"+String(i)+","+String((unsigned long long)r.hz[i])+";";
  if(r.active>=0&&r.hz[r.active]>0&&(all||old.active!=r.active||old.hz[r.active]!=r.hz[r.active]))
    s+="dds:0,"+String((unsigned long long)r.hz[r.active])+";";
  if(r.mode>=0&&r.mode<int(sizeof names/sizeof *names)&&(all||old.mode!=r.mode))
    s+=String("modulation:0,")+names[r.mode]+";";
  if(r.tx>=0&&(all||old.tx!=r.tx))s+=String("trx:0,")+boolean(r.tx)+";";
  if(r.tune>=0&&(all||old.tune!=r.tune))s+=String("tune:0,")+boolean(r.tune)+";";
  if(r.split>=0&&(all||old.split!=r.split))s+=String("split_enable:0,")+boolean(r.split)+";";
  if(r.drive>=0&&(all||old.drive!=r.drive))s+="drive:0,"+String(r.drive)+";";
  if(s.length())sendCommands(n,s);
  p.previous=r;p.force=false;
}
void command(uint8_t n,String line){
  Client &p=peers[n];
  line.trim();if(!line.length())return;
  int colon=line.indexOf(':');
  String key=colon<0?line:line.substring(0,colon);
  key.toLowerCase();
  String args[3];int count=0;
  if(colon>=0){
    int from=colon+1;
    for(;;){
      if(count==3){answer(n,400,"Слишком много аргументов");return;}
      int comma=line.indexOf(',',from);
      args[count++]=comma<0?line.substring(from):line.substring(from,comma);
      if(comma<0)break;
      from=comma+1;
    }
  }
  String &a=args[0],&b=args[1],&c=args[2];
  if(key=="wolf_id"&&count==1&&numeric(a,9)){p.request=a;return;}
  if(key=="wolf_panel"&&!count){p.panel=true;return;}
  if(key=="wolf_hold"&&count==2&&numeric(b,10)){
    if(a=="ptt"||a=="tune"){
      if(p.holdToken.length()){answer(n,409,"Кнопка уже удерживается");return;}
      String msg;int code=restartingDevice()?503:control(a,"",p.owner,msg);
      if(code<300){p.holdToken=b;p.nativeHold=false;}
      answer(n,code,msg);return;
    }
    if((a=="renew"||a=="release")&&p.holdToken==b){
      if(a=="release")p.holdToken="";
      action(n,a,"");return;
    }
    answer(n,a=="release"?200:409,"Удержание завершено");return;
  }
  if(key=="wolf_cmd"&&count==2){
    if(a=="ptt"||a=="tune"||a=="renew"||a=="release"){answer(n,400,"Нужен номер удержания");return;}
    if(b.length()>100){answer(n,400,"Слишком длинное значение");return;}
    String value;
    if(!decodeArgument(b,value)){answer(n,400,"Некорректное значение Base64");return;}
    action(n,a,value);return;
  }
  if(key=="rx_sensors_enable")a.toLowerCase();
  if(key=="trx"||key=="tune"||key=="split_enable")b.toLowerCase();
  if(key=="rx_sensors_enable"&&(count==1||count==2)&&(a=="true"||a=="false")){
    p.meterPeriod=a=="false"?0:constrain(count==2?b.toInt():250L,30L,1000L);return;
  }
  if(key=="vfo"&&(count==2||count==3)&&a=="0"&&(b=="0"||b=="1")){
    if(count==3)action(n,b=="0"?"fa":"fb",c);
    else {auto r=radioForTci();int i=b.toInt();if(r.hz[i]>0)sendText(n,"vfo:0,"+b+","+String((unsigned long long)r.hz[i])+";");}
    return;
  }
  if(a=="0"&&(key=="trx"||key=="tune"||key=="modulation"||key=="split_enable"||key=="drive"||key=="dds")){
    auto r=radioForTci();
    if(count==1){syncState(n,r,true);return;}
    if(count==2){
      if(key=="drive"){action(n,"power",b);return;}
      if(key=="dds"&&r.active>=0){action(n,r.active?"dialB":"dialA",b);return;}
      if(key=="modulation"){
        b.toUpperCase();
        for(unsigned i=0;i<sizeof setModes/sizeof *setModes;i++)
          if(b==setModes[i]){action(n,"mode",modeCodes[i]);return;}
      }else if(b=="true"||b=="false"){
        if(key=="split_enable"){action(n,"split",b=="true"?"1":"0");return;}
        if(key=="trx"||key=="tune"){action(n,b=="true"?(key=="trx"?"ptt":"tune"):"release","",true);return;}
      }
    }
  }
  if(key=="iq_start"||key=="audio_start"){answer(n,501,"Wolf Interface I²C не передаёт отсчёты");return;}
  if(p.request.length())answer(n,400,"Неизвестная команда или аргумент TCI");
}
void event(uint8_t n,WStype_t type,uint8_t *bytes,size_t length){
  if(n>=WEBSOCKETS_SERVER_CLIENT_MAX)return;
  auto &p=peers[n];
  if(type==WStype_DISCONNECTED){
    if(p.live){String ignored;control("release","",p.owner,ignored);}
    p=Client{};return;
  }
  if(type==WStype_CONNECTED){
    p=Client{};p.live=true;p.owner=esp_random();if(!p.owner)p.owner=n+1;
    sendCommands(n,"protocol:WolfConsole,1.8;device:WOLF;trx_count:1;channel_count:2;receive_only:false;vfo_limits:1,750000000;modulations_list:LSB,USB,CW,NFM,SAM,DIGL,IQ,DIGU;");
    syncState(n,radioForTci(),true);sendText(n,"ready;");return;
  }
  if(type!=WStype_TEXT||!length||length>1536)return;
  size_t start=0;
  for(size_t i=0;i<length;i++)if(bytes[i]==';'){
    command(n,String((const char*)bytes+start,i-start));start=i+1;
  }
  p.request=""; // Correlation applies only within this WebSocket message.
}
bool headerOK(String name,String value){
  if(!name.equalsIgnoreCase("Origin"))return true;
  value.toLowerCase();
  return value=="http://"+WiFi.softAPIP().toString() || value=="http://"+WiFi.localIP().toString()
    || value=="http://wolf-console.local" || value=="http://wolf-console" || value=="http://wolf-console.local.";
}
}
void beginTci(){
  ws.begin();ws.onValidateHttpHeader(headerOK,nullptr,0);ws.onEvent(event);
  ws.enableHeartbeat(1200,900,2);
}
void pollTci(){
  ws.loop();
  static uint32_t last=0;
  uint32_t now=millis();if(now-last<250)return;last=now;
  bool any=false;
  for(const auto &p:peers)any|=p.live;
  if(!any)return;
  auto radio=radioForTci();String state;
  for(uint8_t n=0;n<WEBSOCKETS_SERVER_CLIENT_MAX;n++){
    auto &p=peers[n];if(!p.live)continue;
    if(p.nativeHold){String ignored;if(control("renew","",p.owner,ignored)!=200)p.nativeHold=false;}
    syncState(n,radio,p.force);
    if(p.panel){
      if(!state.length())state="wolf_state:"+base64::encode(radioJson())+";";
      sendText(n,state);
    }
    if(p.meterPeriod&&radio.signalTime&&radio.signalTime!=p.lastMeasurement&&now-p.meterAt>=p.meterPeriod){
      sendText(n,"rx_sensors:0,"+String(radio.signal)+";");
      p.lastMeasurement=radio.signalTime;p.meterAt=now;
    }
  }
}
