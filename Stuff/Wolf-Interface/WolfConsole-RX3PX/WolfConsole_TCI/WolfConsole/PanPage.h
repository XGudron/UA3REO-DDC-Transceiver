#pragma once
static const char PAN_JS[] PROGMEM=R"WASSET((()=>{
'use strict';
const el=id=>document.getElementById(id),canvas=el('spectrum'),g=canvas.getContext('2d');
const history=new Map(),ttl=180000;
let radio={},span=48000,context='',lastStamp=null,spots=[],generation=0,busy=false,queried=0;
try{const n=Number(localStorage.wolfSpan);if([12000,48000,96000].includes(n))span=n}catch{}
el('span').value=String(span);
el('span').onchange=()=>{span=Number(el('span').value);try{localStorage.wolfSpan=span}catch{}resetSpots();draw()};
function resetSpots(){++generation;queried=0;spots=[];el('spotStatus').textContent=''}
function valid(){return radio.rx&&Number.isSafeInteger(radio.vfo)&&radio.vfo>0&&radio.mode!=null&&![9,10,13].includes(radio.mode)}
function draw(){
  if(canvas.parentElement.hidden)return;
  const w=Math.max(220,canvas.clientWidth),h=154,dpr=Math.min(window.devicePixelRatio||1,2);
  if(canvas.width!==Math.round(w*dpr)||canvas.height!==Math.round(h*dpr)){
    canvas.width=Math.round(w*dpr);canvas.height=Math.round(h*dpr);
  }
  g.setTransform(dpr,0,0,dpr,0,0);g.fillStyle='#09131c';g.fillRect(0,0,w,h);
  const left=38,right=w-8,top=32,bottom=h-15;
  const y=db=>top+(Math.max(-150,Math.min(-30,db))+30)/-120*(bottom-top);
  const x=hz=>left+(.5+(hz-radio.vfo)/span)*(right-left);
  g.font='10px monospace';
  for(const db of [-30,-60,-90,-120,-150]){
    g.fillStyle='#92abb8';g.fillText(String(db),3,y(db)+3);
    g.fillStyle='#203745';g.fillRect(left,y(db),right-left,1);
  }
  for(let i=0;i<=8;i++){g.fillStyle='#203745';g.fillRect(left+i*(right-left)/8,top,1,bottom-top)}
  const ok=valid(),low=radio.vfo-span/2,high=radio.vfo+span/2;
  [low,radio.vfo,high].forEach((hz,i)=>el(['fLow','fMid','fHigh'][i]).textContent=ok?(hz/1e6).toFixed(5)+' МГц':'—');
  if(!ok){el('panStatus').textContent=radio.mode==null?'Ожидание состояния WOLF по I²C':'Пауза · ожидание приёма';return}
  g.fillStyle='#648582';g.fillRect(x(radio.vfo),top,1,bottom-top);
  let count=0;const now=Date.now();
  for(const [hz,p] of history){
    if(now-p.t>=ttl){history.delete(hz);continue}
    if(hz<low||hz>high)continue;count++;
    g.fillStyle=hz===radio.vfo?'#b4f69e':'#68d7eb';g.beginPath();g.arc(x(hz),y(p.db),3,0,2*Math.PI);g.fill();
  }
  let end=[-1,-1];g.font='11px monospace';
  for(const [hz,call] of spots){
    if(hz<low||hz>high)continue;
    const pos=x(hz),label=Math.max(left,Math.min(pos,right-g.measureText(call).width));
    const lane=end.findIndex(n=>n<label);g.fillStyle='#ffd58c';g.fillRect(pos,top-4,1,4);
    if(lane>=0){g.fillText(call,label,12+12*lane);end[lane]=label+g.measureText(call).width+4;}
  }
  el('panStatus').textContent=count?'RX1 · dBm · измерено частот в окне: '+count:'Ожидание первого измерения RX1 · CAT TS-2000';
}
async function pollSpots(){
  if(!valid()||busy||Date.now()-queried<3000)return;
  queried=Date.now();const gen=generation,controller=new AbortController();busy=true;
  const timer=setTimeout(()=>controller.abort(),2000);
  try{
    const low=Math.max(0,radio.vfo-span/2),high=radio.vfo+span/2;
    const r=await fetch('/api/spots?lo='+low+'&hi='+high,{signal:controller.signal,cache:'no-store'});
    if(!r.ok)throw Error();const d=await r.json();if(gen!==generation)return;
    spots=d.spots.sort((a,b)=>a[0]-b[0]);
    el('spotStatus').textContent=['','Подключение к скиммеру…','Скиммер · '+spots.length+' спотов','Ошибка скиммера'][d.state]||'';draw();
  }catch{if(gen===generation){spots=[];el('spotStatus').textContent='Нет данных скиммера';draw()}}
  finally{clearTimeout(timer);busy=false}
}
window.wolfPan={reset(){context='';lastStamp=null;history.clear();resetSpots()},update(next){
  const ctx=[next.active,next.mode,next.rx].join('/');
  if(ctx!==context){history.clear();lastStamp=null;context=ctx;resetSpots()}
  if(next.vfo!==radio.vfo)resetSpots();
  radio=next;
  const p=radio.sample;
  if(valid()&&Array.isArray(p)&&p[0]===radio.vfo&&Number.isFinite(p[1])&&p[2]!==lastStamp){
    lastStamp=p[2];history.delete(p[0]);history.set(p[0],{db:p[1],t:Date.now()});
    if(history.size>1024)history.delete(history.keys().next().value);
  }
  draw();pollSpots();
}};
window.addEventListener('resize',draw);
})();)WASSET";
