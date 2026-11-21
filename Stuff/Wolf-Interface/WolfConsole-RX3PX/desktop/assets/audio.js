
(() => {
'use strict';
const el=id=>document.getElementById(id), button=el('speaker'), label=el('soundState');
if(!button)return;
function pref(key,fallback,min,max){
 try{const s=localStorage.getItem(key),v=Number(s);return s!==null&&Number.isFinite(v)?Math.min(max,Math.max(min,v)):fallback}catch{return fallback}
}
function save(k,v){try{localStorage.setItem(k,String(v))}catch{}}
let volume=pref('wolf.volume',55,0,100), lead=pref('wolf.buffer',30,20,250)/1000;
const knob=el('volume'), select=el('latency');
let ctx=null,gain=null,control=null,wanted=false,generation=0,timeout=0;
function setVolume(n){
 volume=Math.round(Math.min(100,Math.max(0,n)));save('wolf.volume',volume);
 if(knob){knob.style.setProperty('--angle',(-135+volume*2.7)+'deg');knob.setAttribute('aria-valuenow',volume);knob.setAttribute('aria-valuetext',volume+' процентов');el('volNumber').textContent=volume+'%'}
 if(gain&&ctx)gain.gain.setTargetAtTime((volume/100)**2,ctx.currentTime,.012);
}
if(knob){
 let drag=null;
 knob.onpointerdown=e=>{knob.focus();knob.setPointerCapture(e.pointerId);drag={y:e.clientY,v:volume}};
 knob.onpointermove=e=>{if(drag)setVolume(drag.v+(drag.y-e.clientY)*0.85)};
 knob.onlostpointercapture=()=>{drag=null};
 knob.onkeydown=e=>{const steps={ArrowUp:2,ArrowRight:2,ArrowDown:-2,ArrowLeft:-2,PageUp:10,PageDown:-10,Home:-100,End:100};if(e.key in steps){e.preventDefault();setVolume(volume+steps[e.key])}};
 knob.addEventListener('wheel',e=>{if(document.activeElement===knob){e.preventDefault();setVolume(volume+(e.deltaY<0?2:-2))}},{passive:false});
}
if(select){select.value=String(Math.round(lead*1000));select.onchange=()=>{lead=Number(select.value)/1000;save('wolf.buffer',Math.round(1000*lead))}}
window.addEventListener('storage',e=>{if(e.key==='wolf.buffer')lead=pref('wolf.buffer',30,20,250)/1000;if(e.key==='wolf.volume')setVolume(pref('wolf.volume',55,0,100))});
setVolume(volume);
function stop(message){
 wanted=false;++generation;clearTimeout(timeout);if(control)control.abort();control=null;
 const old=ctx;ctx=null;gain=null;if(old)old.close().catch(()=>{});
 button.textContent='Включить звук';button.setAttribute('aria-pressed','false');label.textContent=message;
}
async function stream(token){
 if(token!==generation||!wanted)return;
 const abort=new AbortController();control=abort;
 let lastData=performance.now();
 const watchdog=setInterval(()=>{if(performance.now()-lastData>3500)abort.abort()},700);
 let planned=0,carry=-1;const sources=new Set();
 try{
  const url=new URL('/pcm',location.href);url.port='81';
  const res=await fetch(url,{cache:'no-store',signal:abort.signal});if(!res.ok||!res.body)throw Error();
  const reader=res.body.getReader();
  while(token===generation){
   const {value,done}=await reader.read();if(done)throw Error();
   if(token!==generation)return;lastData=performance.now();
   let raw=value;if(carry>=0){raw=new Uint8Array(value.length+1);raw[0]=carry;raw.set(value,1)}
   let end=raw.length&~1;carry=raw.length>end?raw[end]:-1;
   if(!end)continue;
   if(ctx.state!=='running'){planned=0;continue}
   const now=ctx.currentTime;
   // Drop accumulated network backlog instead of allowing playout delay to grow.
   const maxChunk=Math.ceil(Math.max(lead,.03)*10000)*2;
   const stale=end>Math.max(2400,maxChunk*4);
   if(planned>now+lead+.05||stale){for(const src of sources){try{src.stop()}catch{}}sources.clear();planned=0}
   const start=stale?Math.max(0,end-maxChunk):0,size=(end-start)/2;
   const buf=ctx.createBuffer(1,size,10000), dst=buf.getChannelData(0), bytes=new DataView(raw.buffer,raw.byteOffset,raw.byteLength);
   for(let i=0;i<size;i++)dst[i]=bytes.getInt16(start+2*i,true)/32768;
   if(planned<now+0.005)planned=now+lead;
   const src=ctx.createBufferSource();src.buffer=buf;src.connect(gain);
   const speed=1+Math.max(-.005,Math.min(.005,(planned-now-lead)*.07));src.playbackRate.value=speed;
   src.onended=()=>{sources.delete(src);src.disconnect()};sources.add(src);src.start(planned);planned+=buf.duration/speed;
   if(label.textContent!=='Звук в динамиках')label.textContent='Звук в динамиках';
  }
 }catch{
  if(token===generation&&wanted){label.textContent='Ожидание аудиопотока…';clearTimeout(timeout);timeout=setTimeout(()=>stream(token),950)}
 }finally{clearInterval(watchdog);abort.abort()}
}
button.onclick=async()=>{
 if(wanted){stop('Звук выключен');return}
 const token=++generation;wanted=true;button.textContent='Выключить звук';button.setAttribute('aria-pressed','true');
 try{
  const Audio=window.AudioContext||window.webkitAudioContext;
  ctx=new Audio({latencyHint:'interactive'});gain=ctx.createGain();gain.connect(ctx.destination);setVolume(volume);
  await ctx.resume();if(token!==generation)return;
  const r=await fetch('/api/audio',{cache:'no-store'});if(!r.ok)throw Error('ESP32 недоступна');
  const info=await r.json();if(token!==generation)return;
  if(info.capture!=='ok'||!info.stream)throw Error('По I²C звук не поступает. Для прослушивания нужен подключённый и включённый аудиовход ESP32.');
  label.textContent='Подключение…';stream(token);
 }catch(e){if(token===generation)stop(e.message||'Браузер не открыл звуковое устройство')}
};
window.addEventListener('pagehide',()=>stop('Звук выключен'));
})();
