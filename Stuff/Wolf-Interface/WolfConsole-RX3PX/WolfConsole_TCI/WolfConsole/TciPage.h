#pragma once
static const char TCI_JS[] PROGMEM=R"WTCI((()=>{
'use strict';
const encode=s=>btoa(Array.from(new TextEncoder().encode(s),n=>String.fromCharCode(n)).join(''));
const decode=s=>new TextDecoder().decode(Uint8Array.from(atob(s),c=>c.charCodeAt(0)));
let socket,serial=0,last=0,stopped=false,retry,timer;
const pending=new Map();
const status=msg=>new CustomEvent('wolf-link',{detail:msg});
function lost(){
  last=0;
  for(const {reject,timeout} of pending.values()){clearTimeout(timeout);reject(Error('Соединение TCI прервано'))}
  pending.clear();window.dispatchEvent(status(false));
}
function connect(){
  if(stopped)return;
  const ws=socket=new WebSocket('ws://'+location.hostname+':50001');let ready=false;
  ws.onopen=()=>{last=Date.now()};
  ws.onmessage=e=>{
    if(ws!==socket||typeof e.data!=='string')return;
    for(const frame of e.data.split(';')){
      const i=frame.indexOf(':'),key=(i<0?frame:frame.slice(0,i)).toLowerCase(),val=frame.slice(i+1);
      if(key==='ready'&&!ready){ready=true;ws.send('wolf_panel;')}
      if(key==='wolf_state'){
        try{const state=JSON.parse(decode(val));if(!Array.isArray(state.v))continue;
          last=Date.now();window.dispatchEvent(new CustomEvent('wolf-state',{detail:state}));
        }catch{}
      }
      if(key==='wolf_ack'){
        const [id,code,msg]=val.split(','),p=pending.get(id);
        if(!p)continue;pending.delete(id);clearTimeout(p.timeout);
        if(Number(code)>=400){const err=Error(decode(msg)||'Команда отклонена');err.status=Number(code);p.reject(err)}
        else p.resolve(decode(msg));
      }
    }
  };
  ws.onerror=()=>ws.close();
  const opening=Date.now();
  timer=setInterval(()=>{if(Date.now()-(last||opening)>4000)ws.close()},600);
  ws.onclose=()=>{
    if(ws!==socket)return;clearInterval(timer);socket=null;lost();
    if(!stopped)retry=setTimeout(connect,1100);
  };
}
const modes={'1':'LSB','2':'USB','3':'CW','4':'NFM','5':'SAM','6':'DIGL','8':'IQ','9':'DIGU'};
window.wolfTCI={
  run(){connect()},
  command(op,value='',owner=0){
    const ws=socket;
    if(ws?.readyState!==WebSocket.OPEN||!last||Date.now()-last>3000)return Promise.reject(Error('Нет связи TCI с ESP32'));
    const v=String(value);let cmd;
    if(op==='fa'||op==='fb')cmd='vfo:0,'+(op==='fa'?0:1)+','+v;
    else if(op==='power')cmd='drive:0,'+v;
    else if(op==='mode'&&modes[v])cmd='modulation:0,'+modes[v];
    else if(op==='split')cmd='split_enable:0,'+(v==='1'?'true':'false');
    else if(['ptt','tune','renew','release'].includes(op))cmd='wolf_hold:'+op+','+owner;
    else cmd='wolf_cmd:'+op+','+encode(v);
    const id=String(serial=serial%999999998+1);
    return new Promise((resolve,reject)=>{
      const timeout=setTimeout(()=>{pending.delete(id);reject(Error('Нет ответа TCI от ESP32'))},2500);
      pending.set(id,{resolve,reject,timeout});
      try{ws.send('wolf_id:'+id+';'+cmd+';')}catch(e){clearTimeout(timeout);pending.delete(id);reject(e)}
    });
  }
};
window.addEventListener('pagehide',()=>{stopped=true;clearTimeout(retry);socket?.close()});
window.addEventListener('pageshow',e=>{if(e.persisted)location.reload()});
})();)WTCI";
