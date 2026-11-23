#include "PcmStream.h"
#include <Arduino.h>
#include <lwip/sockets.h>
#include <fcntl.h>
#include <errno.h>
#include <atomic>
namespace {
QueueHandle_t frames;
std::atomic<unsigned> listeners{0};
struct Peer { int fd=-1; bool ready=false; uint32_t since; unsigned used=0; char header[1850]; };
Peer peers[3];
void shut(Peer &p) {
  if(p.ready)--listeners;
  close(p.fd);p.fd=-1;p.ready=false;p.used=0;
}
bool approved(char *header) {
  if(strncmp(header,"GET /pcm HTTP/1.",16))return false;
  char *origin=nullptr,*host=nullptr,*save=nullptr;
  for(char *line=strtok_r(header,"\r\n",&save);line;line=strtok_r(nullptr,"\r\n",&save)) {
    if(!strncasecmp(line,"Origin: ",8))origin=line+8;
    if(!strncasecmp(line,"Host: ",6))host=line+6;
  }
  if(!host)return false;
  char *port=strrchr(host,':');
  if(port&&!strcmp(port,":81"))*port=0;
  return !origin || (!strncasecmp(origin,"http://",7)&&!strcasecmp(origin+7,host));
}
void handshake(Peer &p) {
  if(p.used+1>=sizeof p.header||millis()-p.since>2500){shut(p);return;}
  int n=recv(p.fd,p.header+p.used,sizeof(p.header)-1-p.used,MSG_DONTWAIT);
  if(n<0&&(errno==EWOULDBLOCK||errno==EAGAIN))return;
  if(n<=0){shut(p);return;}
  p.used+=n;p.header[p.used]=0;
  if(!strstr(p.header,"\r\n\r\n"))return;
  if(!approved(p.header)){shut(p);return;}
  static const char reply[]="HTTP/1.0 200 OK\r\nContent-Type: application/octet-stream\r\nAccess-Control-Allow-Origin: *\r\nCache-Control: no-store\r\nX-Content-Type-Options: nosniff\r\nConnection: close\r\n\r\n";
  if(send(p.fd,reply,sizeof(reply)-1,MSG_DONTWAIT)!=sizeof(reply)-1){shut(p);return;}
  p.ready=true;++listeners;
}
void serve(void *arg) {
  int server=(intptr_t)arg;
  for(;;) {
    int fd=accept(server,nullptr,nullptr);
    if(fd>=0){
      bool joined=false;
      for(auto &p:peers)if(p.fd<0){
        p.fd=fd;p.since=millis();int yes=1;setsockopt(fd,IPPROTO_TCP,TCP_NODELAY,&yes,sizeof yes);
        fcntl(fd,F_SETFL,O_NONBLOCK);joined=true;break;
      }
      if(!joined)close(fd);
    }
    for(auto &p:peers)if(p.fd>=0&&!p.ready)handshake(p);
    AudioFrame frame;
    if(xQueueReceive(frames,&frame,pdMS_TO_TICKS(10))!=pdTRUE)continue;
    for(auto &p:peers)if(p.ready && send(p.fd,&frame,sizeof frame,MSG_DONTWAIT)!=sizeof frame)shut(p);
  }
}
}
bool startPcmStream(){
  int server=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
  if(server<0)return false;
  sockaddr_in addr{};addr.sin_family=AF_INET;addr.sin_port=htons(81);addr.sin_addr.s_addr=INADDR_ANY;
  if(bind(server,(sockaddr*)&addr,sizeof addr)||listen(server,3)||fcntl(server,F_SETFL,O_NONBLOCK)<0){
    close(server);return false;
  }
  frames=xQueueCreate(1,sizeof(AudioFrame));
  if(!frames||xTaskCreatePinnedToCore(serve,"wolf-stream",4096,(void*)(intptr_t)server,2,nullptr,0)!=pdPASS){
    if(frames)vQueueDelete(frames);frames=nullptr;close(server);return false;
  }
  return true;
}
unsigned audioListeners(){return listeners.load();}
void sendPcmFrame(const AudioFrame &frame){if(frames&&listeners)xQueueOverwrite(frames,&frame);}
