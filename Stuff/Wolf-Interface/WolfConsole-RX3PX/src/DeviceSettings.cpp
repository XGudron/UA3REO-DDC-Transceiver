#include "DeviceSettings.h"
#include "Config.h"
#include "Radio.h"
#include "SettingsPage.h"
#include <WiFi.h>
#include <ESPmDNS.h>
#include <Preferences.h>
#include <Update.h>
#include <esp_ota_ops.h>
#include <esp_random.h>

namespace {
constexpr char HOST[] = "wolf-console";
constexpr char RELEASE[] = "1.2";
constexpr char FAMILY[] = "WOLFCONSOLE-YDS3-N16R8-OTA";
constexpr uint8_t pins[] = {1,2,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,21,39,40,41,42,47};
WebServer *web;
Preferences prefs;
bool storage;
String admin, apPassword;
uint8_t sda=5, scl=18;
uint32_t rebootAt;
struct Network { char ssid[33]; char pass[65]; };
Network saved{}, candidate{};
enum class Join { idle, disconnecting, connecting };
Join join=Join::idle;
bool trial, scanning;
uint32_t nextAction, lastAttempt;
String networkNote;
bool mdnsReady;
bool due(uint32_t now,uint32_t t){return int32_t(now-t)>=0;}
bool pinAllowed(int n){for(auto p:pins)if(n==p)return true;return false;}
String json(const String &v){
  String out="\"";
  for(size_t i=0;i<v.length();i++){
    char c=v[i];
    if(c=='"'||c=='\\'){out+='\\';out+=c;}
    else if((uint8_t)c<32){char buf[7];snprintf(buf,sizeof(buf),"\\u%04x",(uint8_t)c);out+=buf;}
    else out+=c;
  }
  out+='"';return out;
}
void sendText(int code,const String &s){web->send(code,"text/plain; charset=utf-8",s);}
bool linked(){return WiFi.status()==WL_CONNECTED && uint32_t(WiFi.localIP())!=0;}
bool viaAP(){
  const IPAddress local=web->client().localIP(), peer=web->client().remoteIP(), ap=WiFi.softAPIP();
  return uint32_t(ap)!=0 && local==ap && peer[0]==ap[0] && peer[1]==ap[1] && peer[2]==ap[2];
}
bool knownHost(){
  String h=web->hostHeader();h.toLowerCase();if(h.endsWith(":80"))h.remove(h.length()-3);
  return h==WiFi.softAPIP().toString() || (linked()&&h==WiFi.localIP().toString()) ||
         h==HOST || h==String(HOST)+".local" || h==String(HOST)+".local.";
}
bool access(){
  if(!knownHost()||web->header("X-Wolf-Panel")!="1")return false;
  if(viaAP())return true;
  return admin.length() && web->header("X-Wolf-Key")==admin;
}
bool permit(){
  if(!access()){sendText(403,"Введите код управления из настроек точки доступа WOLF-Console.");return false;}
  if(rebootAt){sendText(409,"Плата перезапускается.");return false;}
  return true;
}
bool writable(){
  if(!permit())return false;
  if(!storage){sendText(503,"Память настроек NVS недоступна.");return false;}
  if(radioTransmitting()){sendText(409,"Для изменения настроек переведите трансивер на приём.");return false;}
  return true;
}
void status(){
  web->sendHeader("Cache-Control","no-store");
  if(!knownHost() || web->header("X-Wolf-Panel")!="1"){sendText(403,"Недопустимый запрос.");return;}
  const bool local=viaAP(), online=linked();
  String s="{\"version\":"+json(RELEASE)+",\"connected\":"+(online?"true":"false");
  s+=",\"ssid\":"+json(online?WiFi.SSID():String())+",\"saved\":"+json(saved.ssid);
  s+=",\"ip\":"+json(online?WiFi.localIP().toString():String())+",\"ap\":"+json(WiFi.softAPIP().toString());
  s+=",\"mdns\":"+json(mdnsReady?String(HOST)+".local":String())+",\"note\":"+json(networkNote);
  s+=",\"trial\":"+String(join!=Join::idle&&trial?"true":"false")+",\"local\":"+(local?"true":"false");
  s+=",\"admin\":"+json(local?admin:String())+",\"sda\":"+String(sda)+",\"scl\":"+String(scl)+",\"pins\":[";
  for(size_t i=0;i<sizeof pins;i++){if(i)s+=',';s+=String(pins[i]);}
  s+="]}";web->send(200,"application/json",s);
}
void startJoin(const Network &n,bool isTrial){
  candidate=n;trial=isTrial;
  WiFi.disconnect(false,false);
  join=Join::disconnecting;nextAction=millis()+450;lastAttempt=millis();
}
void submitWifi(){
  if(!writable())return;
  if(join!=Join::idle && trial){sendText(409,"Проверка сети уже идёт.");return;}
  String name=web->arg("ssid"),psk=web->arg("pass");
  if(!name.length() || name.length()>32 || !web->hasArg("pass") || psk.length()>64 ||
     (psk.length()>0 && psk.length()<8)){sendText(400,"SSID до 32 байт, пароль 8–63 байта или пустое поле для открытой сети.");return;}
  if(psk.length()==64)for(char c:psk)if(!isxdigit((unsigned char)c)){sendText(400,"Ключ из 64 знаков должен быть шестнадцатеричным.");return;}
  if(name==AP_NAME){sendText(400,"Выберите домашнюю сеть.");return;}
  if(scanning && WiFi.scanComplete()==WIFI_SCAN_RUNNING){sendText(409,"Подождите завершения поиска сетей.");return;}
  Network n{};name.toCharArray(n.ssid,sizeof n.ssid);psk.toCharArray(n.pass,sizeof n.pass);
  // Reply before changing the radio channel shared by the AP and station.
  sendText(202,"Проверяю подключение. Точка доступа останется доступной.");
  networkNote="Получение адреса от роутера…";startJoin(n,true);
}
void updateNetwork(uint32_t now){
  if(join==Join::disconnecting && due(now,nextAction)){
    WiFi.begin(candidate.ssid,candidate.pass[0]?candidate.pass:nullptr);
    join=Join::connecting;nextAction=now+25000;return;
  }
  if(join==Join::connecting){
    if(linked() && WiFi.SSID()==candidate.ssid){
      if(trial){
        if(prefs.putBytes("home",&candidate,sizeof candidate)==sizeof candidate){
          saved=candidate;networkNote="Подключение проверено и сохранено.";
        }else{networkNote="Сеть доступна, запись в NVS не удалась.";}
      }
      Serial.print("WOLF LAN: http://");Serial.println(WiFi.localIP());
      candidate={};join=Join::idle;trial=false;
    }else if(due(now,nextAction)){
      const bool restore=trial && saved.ssid[0];
      if(trial)networkNote="Не удалось получить адрес. Проверьте сеть и пароль.";
      WiFi.disconnect(false,false);candidate={};join=Join::idle;trial=false;lastAttempt=now;
      if(restore)startJoin(saved,false);
    }
  }else if(join==Join::idle && !linked() && saved.ssid[0] && now-lastAttempt>35000 &&
           !(scanning&&WiFi.scanComplete()==WIFI_SCAN_RUNNING)){
    startJoin(saved,false);
  }
}
void scanStart(){
  if(!permit())return;
  if(join!=Join::idle && trial){sendText(409,"Проверяется домашняя сеть.");return;}
  if(WiFi.scanComplete()!=WIFI_SCAN_RUNNING){WiFi.scanDelete();int n=WiFi.scanNetworks(true,false,false,160);
    if(n==WIFI_SCAN_FAILED){sendText(503,"Поиск пока недоступен. Можно ввести SSID вручную.");return;}}
  scanning=true;sendText(202,"Поиск запущен.");
}
void scanResults(){
  if(!permit())return;
  web->sendHeader("Cache-Control","no-store");
  int n=WiFi.scanComplete();
  if(n==WIFI_SCAN_RUNNING){web->send(202,"application/json","[]");return;}
  scanning=false;
  if(n<0){sendText(503,"Поиск не завершён. Повторите или введите SSID вручную.");return;}
  String s="[";bool first=true;
  for(int i=0;i<n && i<40;i++){
    String name=WiFi.SSID(i);if(!name.length()||name==AP_NAME)continue;
    if(!first)s+=',';first=false;
    s+="{\"ssid\":"+json(name)+",\"db\":"+String(WiFi.RSSI(i))+",\"open\":"+(WiFi.encryptionType(i)==WIFI_AUTH_OPEN?"true":"false")+"}";
  }
  s+=']';web->send(200,"application/json",s);
}
bool pinNumber(const String &v,int &n){
  if(v.isEmpty()||v.length()>2)return false;
  for(char c:v)if(c<'0'||c>'9')return false;
  n=v.toInt();return pinAllowed(n);
}
void changePins(){
  if(!writable())return;int a,b;
  if(!pinNumber(web->arg("sda"),a)||!pinNumber(web->arg("scl"),b)||a==b){sendText(400,"Выберите два разных доступных GPIO.");return;}
  if(a==sda && b==scl){sendText(200,"Эти GPIO уже используются.");return;}
  if(prefs.putUShort("pins",uint16_t(a<<8|b))!=sizeof(uint16_t)){sendText(500,"GPIO не сохранены.");return;}
  sendText(202,"GPIO сохранены. Плата перезапускается.");rebootAt=millis()+900;
}
void changeAP(){
  if(!writable())return;
  String v=web->arg("pass");
  if(v.length()<8||v.length()>63){sendText(400,"Пароль точки доступа должен содержать 8–63 байта.");return;}
  if(prefs.putString("ap-pass",v)!=v.length()){sendText(500,"Пароль не сохранён.");return;}
  sendText(202,"Пароль сохранён. После перезапуска войдите с новым паролем.");rebootAt=millis()+900;
}

enum class UploadPhase { idle, receiving, ready, failed };
UploadPhase uploadPhase=UploadPhase::idle;
size_t expected, written, markerPos;
bool foundMarker;
int errorCode;
String uploadError;
void rejectUpload(int code,const String &reason){
  if(Update.isRunning())Update.abort();
  uploadPhase=UploadPhase::failed;errorCode=code;uploadError=reason;
}
bool appHeader(const uint8_t *b,size_t n){
  // ESP image header (24), segment header (8), app descriptor magic.
  return n>=36 && b[0]==0xe9 && b[12]==9 && b[13]==0 &&
    b[32]==0x32 && b[33]==0x54 && b[34]==0xcd && b[35]==0xab;
}
void receiveUpload(){
  HTTPUpload &u=web->upload();
  if(u.status==UPLOAD_FILE_ABORTED){rejectUpload(400,"Передача файла прервана.");return;}
  if(u.status==UPLOAD_FILE_START){
    if(uploadPhase==UploadPhase::ready || Update.isRunning()){rejectUpload(400,"Выберите один файл.");return;}
    uploadPhase=UploadPhase::idle;expected=written=markerPos=0;foundMarker=false;uploadError="";
    if(!access()){rejectUpload(403,"Требуется код управления.");return;}
    if(rebootAt||radioTransmitting()||trial){rejectUpload(409,"Дождитесь приёма и окончания настройки Wi-Fi.");return;}
    String len=web->header("X-Wolf-Size");
    const esp_partition_t *part=esp_ota_get_next_update_partition(nullptr);
    if(!part||u.name!="firmware"||!len.length()||len.length()>8){rejectUpload(400,"Нужен файл OTA приложения.");return;}
    for(char c:len)if(c<'0'||c>'9'){rejectUpload(400,"Некорректный размер.");return;}
    expected=len.toInt();
    if(expected<256||expected>part->size){rejectUpload(400,"Размер файла не подходит для OTA-раздела.");return;}
    uploadPhase=UploadPhase::receiving;return;
  }
  if(uploadPhase!=UploadPhase::receiving)return;
  if(u.status==UPLOAD_FILE_WRITE && u.currentSize){
    if(written+u.currentSize>expected){rejectUpload(400,"Размер не совпадает.");return;}
    if(!written){
      if(!appHeader(u.buf,u.currentSize)){rejectUpload(400,"Выберите файл _OTA.bin для WOLF ESP32-S3.");return;}
      if(!Update.begin(expected,U_FLASH)){rejectUpload(500,Update.errorString());return;}
    }
    for(size_t i=0;i<u.currentSize&&!foundMarker;i++){
      char c=(char)u.buf[i];
      if(c==FAMILY[markerPos])++markerPos;else markerPos=(c==FAMILY[0])?1:0;
      foundMarker=markerPos==sizeof(FAMILY)-1;
    }
    if(Update.write(u.buf,u.currentSize)!=u.currentSize){rejectUpload(500,Update.errorString());return;}
    written+=u.currentSize;
  }else if(u.status==UPLOAD_FILE_END){
    if(written!=expected||!foundMarker){rejectUpload(400,"Неполный файл или прошивка другого проекта.");return;}
    uploadPhase=UploadPhase::ready;
  }
}
void finishUpload(){
  if(uploadPhase==UploadPhase::ready){
    if(!access() || radioTransmitting())rejectUpload(409,"Условия обновления изменились.");
    else if(!Update.end(false))rejectUpload(400,String("Образ отклонён: ")+Update.errorString());
  }
  if(uploadPhase==UploadPhase::ready){
    sendText(200,"Прошивка проверена. ESP32 перезапускается.");rebootAt=millis()+900;
  }else if(uploadPhase==UploadPhase::failed)sendText(errorCode,uploadError);
  else sendText(400,"Файл прошивки не получен.");
  uploadPhase=UploadPhase::idle;
}
}

uint8_t activeSDA(){return sda;}
uint8_t activeSCL(){return scl;}
bool restartingDevice(){return rebootAt!=0;}
void beginDevice(WebServer &server){
  web=&server;storage=prefs.begin("wolf-panel",false);
  if(storage){
    if(prefs.getBytesLength("home")==sizeof saved)prefs.getBytes("home",&saved,sizeof saved);
    saved.ssid[32]=0;saved.pass[64]=0;
    uint16_t v=prefs.getUShort("pins",0x0512);
    if(pinAllowed(v>>8)&&pinAllowed(v&255)&&(v>>8)!=(v&255)){sda=v>>8;scl=v&255;}
    apPassword=prefs.getString("ap-pass",AP_PASSWORD);
    admin=prefs.getString("admin","");
  }
  if(apPassword.length()<8)apPassword=AP_PASSWORD;
  WiFi.persistent(false);WiFi.setHostname(HOST);WiFi.mode(WIFI_AP_STA);WiFi.setAutoReconnect(false);
  if(!admin.length()){
    char code[25];snprintf(code,sizeof code,"%08lx%08lx%08lx",(unsigned long)esp_random(),(unsigned long)esp_random(),(unsigned long)esp_random());
    admin=code;if(storage)prefs.putString("admin",admin);
  }
  WiFi.disconnect(false,false);
  if(!WiFi.softAP(AP_NAME,apPassword.c_str()))Serial.println("Wi-Fi AP error");
  Serial.print("WOLF setup: http://");Serial.println(WiFi.softAPIP());
  mdnsReady=MDNS.begin(HOST);if(mdnsReady)MDNS.addService("http","tcp",80);
  if(saved.ssid[0])startJoin(saved,false);
  server.on("/settings",HTTP_GET,[]{
    web->sendHeader("Cache-Control","no-store");web->sendHeader("X-Frame-Options","DENY");
    web->send_P(200,"text/html; charset=utf-8",SETTINGS_PAGE);
  });
  server.on("/api/settings",HTTP_GET,status);
  server.on("/api/wifi",HTTP_POST,submitWifi);
  server.on("/api/scan",HTTP_POST,scanStart);server.on("/api/scan",HTTP_GET,scanResults);
  server.on("/api/pins",HTTP_POST,changePins);
  server.on("/api/ap-password",HTTP_POST,changeAP);
  server.on("/api/update",HTTP_POST,finishUpload,receiveUpload);
}
void serviceDevice(){
  const uint32_t now=millis();
  if(rebootAt){if(due(now,rebootAt))ESP.restart();return;}
  updateNetwork(now);
}
