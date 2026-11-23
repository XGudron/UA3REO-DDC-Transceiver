#include "Radio.h"
#include "Bus.h"
#include <cmath>
#include <atomic>
#include <ctype.h>

// All radio state below is owned by the radio task or a short locked HTTP call.
// Only the mailbox and frame queue cross into the hardware interrupt.
namespace {
SemaphoreHandle_t lock;
struct Guard { Guard(){ xSemaphoreTake(lock, portMAX_DELAY); } ~Guard(){ xSemaphoreGive(lock); } };
struct Meter {
  const char *key; const char *prefix; char value[20]; uint32_t when;
};
Meter meters[] = {
  {"ID","ID"}, {"SM2","SM3"}, {"SM3","SM3"}, {"PC","PC"}, {"FW","FW"},
  {"GC","GC"}, {"NB","NB"}, {"NR","NR"}, {"NT","NT"}, {"FS","FS"}, {"PA","PA"},
  {"RA","RA"}, {"MT","MT"}, {"LK","LK"}, {"SC","SC"}, {"AN","AN"}, {"AC","AC"},
  {"RG","RG"}, {"AG0","AG0"}, {"AG1","AG1"}, {"MG","MG"}, {"KS","KS"},
  {"GT","GT"}, {"XT","XT"}, {"VX","VX"}, {"NL","NL"}, {"RL","RL"},
  {"SL","SL"}, {"SH","SH"}, {"VD","VD"}, {"SM0","SM0"}, {"SM1","SM1"},
  {"RM.W","RM4"}, {"RM.S","RM1"}
};
constexpr int N = sizeof(meters)/sizeof(*meters);
int indexOf(const char *key) {
  for(int i=0;i<N;i++) if(!strcmp(meters[i].key,key)) return i;
  return -1;
}
uint64_t data[10]{};
uint32_t stamp[10]{};
uint32_t frameCount, badCount, timeoutCount, seenPoll, pollAt, nextId, nextPoll, lastSignal, lastPanel;
bool busOK, online, linkEnabled=true;
int gatedMode=-1;
uint32_t sequence;
std::atomic<uint32_t> digiSession{0};
int queuedVfo=-1;bool queuedAntenna=false;
int expectedAnt=-1;uint32_t lastAntQuery;
char lastCmd[64], lastReply[96], manualReply[128], userNote[120];
const char *const infoQueries[]={"FA","FB","VS","FT","MD","IF","OI"};
bool fresh(unsigned id){return stamp[id] && millis()-stamp[id]<3500;}
// Bind an RX1 meter reply to the stable tuning interval in which it was requested.
uint32_t tuningGeneration=1, meterGeneration, sampleTime;
uint64_t sampleHz;
int sampleDbm;
int64_t activeRxHz(){
  if(!online||!fresh(1)||!fresh(7)||!fresh(8)||data[7]||data[8])return 0;
  unsigned a=data[1];
  return a<2&&fresh(2+a)&&fresh(4+a)?data[2+a]:0;
}
struct Job { uint32_t id, since; int meter; bool picked, manual; char cmd[64]; };
Job job{};
char queued[64]; int queuedMeter=-1;
int slow = 3, txPhase = 0, refresh = -1;
bool backgroundTurn;
uint32_t ownerId, renewed;
uint8_t radioSDA, radioSCL;
bool tuning, stopWanted;
char tuneOff[8] = "AC000";
bool due(uint32_t now,uint32_t t){return int32_t(now-t)>=0;}
bool isTS(){return !strcmp(meters[0].value,"019");}
void note(const char *s){strlcpy(userNote,s,sizeof(userNote));}
void clearJob(){publishCat("",0);job={};job.meter=-1;}
void launch(const char *cmd,int meter=-1,bool manual=false) {
  job={}; job.id=++nextId;
  if(!job.id) job.id=++nextId;
  if(meter==1)meterGeneration=activeRxHz()?tuningGeneration:0;
  job.since=millis();job.meter=meter;job.manual=manual;
  strlcpy(job.cmd,cmd,sizeof(job.cmd));strlcpy(lastCmd,cmd,sizeof(lastCmd));
  publishCat(cmd,job.id);
}
void stopLease(){
  if(!ownerId && !stopWanted) return;
  queued[0]=0;ownerId=0;stopWanted=true;
  launch(tuning?tuneOff:"RX",-1,true);
  note("Запрошен возврат на приём");
}
void updateDigitalGate();
void parse(const WolfFrame &f,uint32_t now){
  if(f.size<3 || f.bytes[0]!=0x10){++badCount;return;}
  const uint8_t id=f.bytes[1],len=f.size-2;
  if(id==6){
    char s[96]={};size_t n=0;
    while(n<len && n<sizeof(s)-1 && f.bytes[n+2]){
      uint8_t c=f.bytes[n+2];
      if(c<32||c>126){++badCount;return;}
      s[n++]=c;
    }
    if(!n)return;
    strlcpy(lastReply,s,sizeof(lastReply));
    if(!job.id||busStats().delivery!=job.id)return;
    if(job.meter==-2){
      if(n<3 || strncmp(s,job.cmd,2))return;
      snprintf(manualReply,sizeof(manualReply),"%s → %s",job.cmd,s);
      note("Ответ CAT получен");clearJob();nextPoll=now+30;return;
    }
    if(job.meter<0)return;
    Meter &m=meters[job.meter];size_t pre=strlen(m.prefix);
    // RX1 and RX2 both return SM3 in 16.5.1. Only one query may await an answer.
    if(job.meter==1 && !strncmp(s,"SM2",3)) pre=3;
    else if(strncmp(s,m.prefix,pre))return;
    if(n && s[n-1]==';')s[--n]=0;
    if(n<=pre || n-pre>=sizeof(m.value))return;
    for(size_t k=pre;k<n;k++)if(!isdigit((unsigned char)s[k]) && s[k]!='-'&&s[k]!='.')return;
    strlcpy(m.value,s+pre,sizeof(m.value));m.when=now;
    if(job.meter==1 && meterGeneration==tuningGeneration && activeRxHz()>0){
      char *end=nullptr; long value=strtol(m.value,&end,10);
      if(end!=m.value && !*end && value>=-180 && value<=180){
        sampleHz=activeRxHz();sampleDbm=-abs(int(value));sampleTime=now;
      }
    }
    if(job.manual){snprintf(manualReply,sizeof(manualReply),"%s → %s",job.cmd,lastReply);note("Ответ CAT получен");}
    if(job.meter==indexOf("AN")&&expectedAnt>=0){
      note(atoi(m.value)==expectedAnt?"Выбор антенны подтверждён":"WOLF сообщил другую антенну. Проверьте конфигурацию RF-Unit.");
      expectedAnt=-1;
    }
    clearJob();nextPoll=now+30;
    return;
  }
  if(id<1||id>9||(id==2||id==3 ? len!=8 : len!=1)){++badCount;return;}
  uint64_t v=0;
  for(int i=len-1;i>=0;i--)v=(v<<8)|f.bytes[i+2];
  if((id==1||id>=7)&&v>1){++badCount;return;}
  if(id!=6 && (!fresh(id)||data[id]!=v)){++tuningGeneration;sampleTime=0;}
  data[id]=v;stamp[id]=now;++frameCount;
  if(id==1||id==4||id==5||id==7)updateDigitalGate();
}
void schedule(uint32_t now) {
  if(!meters[0].when || now-meters[0].when>30000){ launch("ID",0);return; }
  if(refresh>=0){int i=refresh;refresh=-1;launch(meters[i].key,i);return;}
  if(isTS()&&!meters[indexOf("AN")].when&&(!lastAntQuery||now-lastAntQuery>5000)){
    lastAntQuery=now;launch("AN",indexOf("AN"));return;
  }
  const bool tx=fresh(7)&&data[7];
  if(!tx)txPhase=0;
  if(!backgroundTurn && now-lastSignal>220){
    backgroundTurn=true;
    lastSignal=now;
    if(tx){
      // RM1/RM4 select the meter; RM (no argument) reads the selection.
      switch(txPhase++%5){
        case 0:launch("RM4");return;
        case 1:launch("RM",N-2);return;
        case 2:launch("RM1");return;
        case 3:launch("RM",N-1);return;
        default:break;
      }
    }else{launch("SM2",1);return;}
  }
  backgroundTurn=false;
  if(slow>=N-2)slow=2;
  int i=slow++;
  if(tx && (i==1||i==2)){i=3;slow=4;}
  launch(meters[i].key,i);
}
void updateDigitalGate(){
  int key=-1;
  if(linkEnabled&&online&&fresh(1)&&data[1]<2&&fresh(7)&&data[7]==0){
    unsigned v=data[1];unsigned m=data[4+v];
    if(fresh(4+v)&&(m==9||m==10))key=int(2*m+v);
  }
  if(key==gatedMode)return;
  gatedMode=key;
  if(++sequence==0)++sequence;
  digiSession.store(key<0?0:sequence);
}
void tick(){
  if(!linkEnabled)return;
  const uint32_t now=millis();
  BusStats stats=busStats();
  if(stats.polls!=seenPoll){seenPoll=stats.polls;pollAt=now;}
  WolfFrame f;
  while(nextFrame(&f))parse(f,now);
  const bool connected=busOK && pollAt && now-pollAt<3500;
  if(online&&!connected){
    ++tuningGeneration;sampleTime=0;
    memset(stamp,0,sizeof(stamp));
    for(auto &m:meters){m.when=0;m.value[0]=0;}
    refresh=-1;expectedAnt=-1;lastAntQuery=0;queued[0]=0;
    if(ownerId || stopWanted)stopLease();else clearJob();
    note("Связь I²C потеряна");
  }
  online=connected;updateDigitalGate();
  if(ownerId && now-renewed>1600)stopLease();
  if(!online)return;
  if(job.id && stats.delivery==job.id && !job.picked){
    job.picked=true;job.since=now;
    if(job.manual && !stopWanted)note("Команда считана Волком");
  }
  if(job.id && job.picked && job.meter==-1){
    if(job.cmd[0]=='A'&&job.cmd[1]=='N'&&job.cmd[2]>='0'&&job.cmd[2]<='3'&&!job.cmd[3]){
      expectedAnt=job.cmd[2]-'0';refresh=indexOf("AN");
    }
    if(stopWanted){stopWanted=false;tuning=false;note("Команда возврата считана");}
    clearJob();nextPoll=now+60;
  }else if(job.id && now-job.since>(job.picked?1600u:2800u)){
    ++timeoutCount;
    if(stopWanted){stopLease();return;}
    if(job.manual){note("Нет подтверждения CAT");snprintf(manualReply,sizeof(manualReply),"%s → таймаут",job.cmd);}
    if(job.meter>=0){meters[job.meter].when=0;meters[job.meter].value[0]=0;}
    clearJob();nextPoll=now+150;
  }
  if(!job.id && queued[0]){
    bool allowed=(queuedVfo<0||(fresh(1)&&data[1]==uint64_t(queuedVfo)))&&
      (!queuedAntenna||(fresh(7)&&!data[7]&&fresh(8)&&!data[8]));
    if(allowed)launch(queued,queuedMeter,true);
    else {if(queuedAntenna)expectedAnt=-1;note("Состояние трансивера изменилось. Команда отменена.");}
    queuedVfo=-1;queuedAntenna=false;queued[0]=0;return;
  }
  if(!job.id && !stopWanted && due(now,nextPoll) && now-lastPanel<8000)schedule(now);
}
void worker(void *){for(;;){{Guard g;tick();}vTaskDelay(pdMS_TO_TICKS(5));}}
String quote(const char *s){
  String r="\"";for(;*s;s++){if(*s=='"'||*s=='\\')r+='\\';r+=*s;}return r+'"';
}
bool digits(const String &s,unsigned n){if(!s.length()||s.length()>n)return false;for(char c:s)if(c<'0'||c>'9')return false;return true;}
}

uint32_t digitalEpoch(){return digiSession.load();}

bool radioTransmitting(){
  Guard g;return ownerId || stopWanted || (fresh(7) && data[7]) || (fresh(8) && data[8]);
}

void startRadio(uint8_t sda, uint8_t scl){
  lock=xSemaphoreCreateMutex();
  radioSDA=sda;radioSCL=scl;
  busOK=startBus(sda,scl);job.meter=-1;
  xTaskCreatePinnedToCore(worker,"Wolf radio",6144,nullptr,4,nullptr,1);
}

String radioJson(){
  Guard g;uint32_t now=millis();lastPanel=now;BusStats b=busStats();
  String s;s.reserve(2400);
  s="{\"enabled\":"+String(linkEnabled?"true":"false")+",\"pins\":["+String(radioSDA)+","+String(radioSCL)+"],\"online\":"+String(online?"true":"false")+",\"bus\":"+(busOK?"true":"false")+",\"ts\":"+(isTS()?"true":"false")+",\"at\":"+String(now)+",\"v\":[null";
  char num[24];
  for(int i=1;i<=9;i++){s+=',';if(!fresh(i)){s+="null";continue;}snprintf(num,sizeof num,"%llu",(unsigned long long)data[i]);s+=num;}
  s+="],\"sample\":";
  if(sampleTime && now-sampleTime<3000 && activeRxHz()==(int64_t)sampleHz)
    s+="["+String((unsigned long long)sampleHz)+","+String(sampleDbm)+","+String(sampleTime)+"]";
  else s+="null";
  s+=",\"m\":{";bool first=true;
  for(auto &m:meters){
    if(!m.when || now-m.when>60000)continue;
    if(!first)s+=',';first=false;
    s+=quote(m.key)+":["+quote(m.value)+","+String(m.when)+"]";
  }
  s+="},\"stat\":["+String(frameCount)+","+String(b.polls)+","+String(b.reads)+","+String(badCount+b.dropped)+","+String(b.shortReads)+","+String(timeoutCount)+"],\"cmd\":"+quote(lastCmd)+",\"reply\":"+quote(lastReply)+",\"manual\":"+quote(manualReply)+",\"note\":"+quote(userNote)+",\"held\":"+String(ownerId?"true":"false")+"}";
  return s;
}

int control(const String &op,const String &v,uint32_t who,String &reply){
  Guard g;lastPanel=millis();
  auto fail=[&](int status,const char *s){reply=s;return status;};
  if(op=="link"){
    if(v!="0"&&v!="1")return fail(400,"Недопустимое состояние связи");
    bool enable=v=="1";
    if(linkEnabled==enable)return 200;
    if(!enable&&(ownerId||stopWanted||queued[0]||(job.id&&job.manual)||
        (online&&(!fresh(7)||!fresh(8)||data[7]||data[8]))))
      return fail(409,"Дождитесь приёма и завершения команды");
    if(enable&&!busOK){busOK=startBus(radioSDA,radioSCL);if(!busOK)return fail(503,"Ошибка запуска I²C");}
    ++tuningGeneration;sampleTime=0;setBusEnabled(enable);linkEnabled=enable;online=false;
    updateDigitalGate();clearJob();queued[0]=0;queuedVfo=-1;queuedAntenna=false;
    memset(stamp,0,sizeof stamp);
    for(auto &m:meters){m.when=0;m.value[0]=0;}
    expectedAnt=-1;lastAntQuery=0;pollAt=0;seenPoll=busStats().polls;refresh=-1;nextPoll=millis();
    lastCmd[0]=lastReply[0]=manualReply[0]=0;
    note(enable?"Ожидание опроса I²C":"Связь отключена");return 200;
  }
  if(!linkEnabled)return fail(409,"Связь с трансивером отключена");
  if(op=="renew"){
    if(!who || ownerId!=who)return fail(409,"Удержание завершено");
    renewed=millis();return 200;
  }
  if(op=="release" || op=="rx"){
    if(op=="release" && (!who||ownerId!=who))return 200;
    if(ownerId||stopWanted)stopLease();
    else if(op=="rx" && online){queued[0]=0;tuning=false;stopWanted=true;launch("RX",-1,true);}
    return 200;
  }
  if(!online)return fail(503,"Нет опроса I²C от Волка");
  if(op=="message"){
    if(v.length()>63)return fail(400,"Максимум 63 знака");
    for(char c:v)if(c<32||c>126)return fail(400,"Используйте печатные знаки ASCII");
    publishMessage(v.c_str());return 202;
  }
  bool query=op=="query";
  if(!query&&!isTS())return fail(409,"Выберите CAT Type TS-2000 на Волке и дождитесь ID019");
  if(ownerId||stopWanted||queued[0]||(job.id&&job.manual))return fail(409,"Предыдущая команда ещё в работе");
  char cmd[64]={};int target=-1;
  if(query){
    target=indexOf(v.c_str());
    if(target<0)for(const char *key:infoQueries)if(v==key){target=-2;break;}
    if(target==-1||target>=N-2)return fail(400,"Неизвестный запрос");
    v.toCharArray(cmd,sizeof(cmd));
  }else if(op=="ptt" || op=="tune"){
    if(!who||!fresh(7)||data[7]||!fresh(8)||data[8])return fail(409,"Для включения требуется подтверждённый приём");
    tuning=op=="tune";
    if(tuning){
      auto &m=meters[indexOf("AC")];
      if(!m.when||millis()-m.when>60000||strlen(m.value)!=3||m.value[0]!=m.value[1]){tuning=false;refresh=indexOf("AC");return fail(409,"Ожидание состояния тюнера AC");}
      snprintf(cmd,sizeof(cmd),"AC%c%c1",m.value[0],m.value[1]);
      strlcpy(tuneOff,cmd,sizeof(tuneOff));tuneOff[4]='0';
    }else strlcpy(cmd,"TX",sizeof(cmd));
    ownerId=who;renewed=millis();
  }else if(op=="antenna"){
    if(v.length()!=1||v[0]<'0'||v[0]>'3')return fail(400,"Выберите ANT 1–4");
    if(!fresh(7)||data[7]||!fresh(8)||data[8])return fail(409,"Для смены антенны требуется приём");
    snprintf(cmd,sizeof cmd,"AN%c",v[0]);
  }else if(op=="fa"||op=="fb"||op=="dialA"||op=="dialB"){
    bool dial=op=="dialA"||op=="dialB";int vf=(op=="fb"||op=="dialB");
    if(dial&&(!fresh(1)||data[1]!=uint64_t(vf)))return fail(412,"Активный VFO изменился");
    if(!digits(v,11)||strtoull(v.c_str(),nullptr,10)==0)return fail(400,"Введите частоту в герцах");
    snprintf(cmd,sizeof(cmd),"F%c%011llu",vf?'B':'A',strtoull(v.c_str(),nullptr,10));
  }else if(op=="vfo"||op=="split"){
    if((v!="0"&&v!="1")||!fresh(1))return fail(400,"Нет данных активного VFO");
    if(op=="vfo")snprintf(cmd,sizeof(cmd),"VS%d",v.toInt());
    else snprintf(cmd,sizeof(cmd),"FT%d",(int)data[1] ^ (int)v.toInt());
  }else if(op=="mode"){
    if(v.length()!=1||!strchr("12345689",v[0]))return fail(400,"Неизвестная мода");
    snprintf(cmd,sizeof(cmd),"MD%c",v[0]);
  }else if(op=="atten"){
    if(!digits(v,2)||v.toInt()>31)return fail(400,"Аттенюатор 0–31 дБ");
    snprintf(cmd,sizeof(cmd),"RA%02d",(int)v.toInt());refresh=indexOf("RA");
  }else if(op=="power"){
    if(!digits(v,3)||v.toInt()>100)return fail(400,"Допустимо 0–100 %");
    snprintf(cmd,sizeof(cmd),"PC%03d",(int)v.toInt());refresh=indexOf("PC");
  }else if(op=="switch"){
    String key=v.substring(0,2);
    if(v.length()!=3 || String("|PA|NB|NR|GC|NT|MT|FS|LK|SC|XT|VX|").indexOf("|"+key+"|")<0 ||
       (v[2]!='0'&&v[2]!='1'&&!(key=="NR"&&v[2]=='2')))return fail(400,"Некорректный переключатель");
    v.toCharArray(cmd,sizeof(cmd));refresh=indexOf(key.c_str());
  }else return fail(400,"Неизвестная команда");
  // Wait for the previous query's response (the protocol has no correlation ID).
  // PTT/TUNE and release have priority over the background meter poll.
  if(job.id && !ownerId){strlcpy(queued,cmd,sizeof(queued));queuedMeter=target;queuedVfo=op=="dialA"?0:op=="dialB"?1:-1;queuedAntenna=op=="antenna";}
  else launch(cmd,target,true);
  note("Команда ожидает чтения Волком");reply=cmd;return 202;
}

int64_t radioAudioDial(){
  Guard g;
  if(!online||!fresh(1)||data[1]>1)return 0;
  int v=int(data[1]);
  if(!fresh(2+v)||!fresh(4+v))return 0;
  const int64_t hz=data[2+v];
  switch(data[4+v]){
    case 1: case 10:return hz;  // USB and DIGU
    case 0: case 9:return -hz;  // LSB and DIGL
    default:return 0;
  }
}

TciRadio radioForTci(){
  Guard g;
  lastPanel=millis();
  auto val=[](int id)->int64_t{return online&&fresh(id)?int64_t(data[id]):-1;};
  int a=val(1);
  Meter &power=meters[indexOf("PC")];
  int drive=online && power.when && millis()-power.when<60000?atoi(power.value):-1;
  return {online,a,a>=0?int(val(4+a)):-1,int(val(7)),int(val(8)),int(val(9)),drive,
    {val(2),val(3)},sampleDbm,online&&sampleTime&&millis()-sampleTime<3000?sampleTime:0};
}
