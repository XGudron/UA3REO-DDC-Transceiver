#pragma once
static const char PANEL[] PROGMEM = R"WOLF(<!doctype html><html lang="ru"><meta charset="utf-8"><meta name="viewport" content="width=device-width,initial-scale=1"><meta name="theme-color" content="#0c1420"><title>WOLF · Панель трансивера</title>
<style>
:root{color-scheme:dark;--ice:#68dcff;--green:#a9f59a;--text:#e3edf5;--grey:#a4b1c0;--line:#334150;--bg:#111b27;--mono:ui-monospace,"Cascadia Mono",Consolas,monospace}*{box-sizing:border-box}body{margin:0;background:radial-gradient(ellipse at 50% -20%,#203346,#0c131b 62%);color:var(--text);font:15px system-ui,Arial,sans-serif;min-height:100vh}main{max-width:1440px;margin:auto;padding:0 clamp(12px,3vw,48px) 30px}header,footer,.topline,.between,.lamps,.keys,.row,nav{display:flex;align-items:center}header,footer,.between,.topline{justify-content:space-between;gap:12px}header{min-height:78px}.identity{display:flex;align-items:center;gap:13px}.mark{width:36px;color:var(--ice);height:39px}.brand{font-size:23px;letter-spacing:.17em;font-weight:850}.sub{border-left:1px solid #466071;padding-left:14px;color:#a7b6c2;font-size:14px}.lamps{gap:12px;flex-wrap:wrap}.led{display:inline-flex;align-items:center;gap:7px;font:12px var(--mono);color:var(--grey)}.led:before{content:"";width:7px;height:7px;background:#758291;border-radius:50%}.led.good:before{background:var(--green);box-shadow:0 0 8px #91ef8b88}.led.bad:before{background:#ffa979}time{font:14px var(--mono);color:#adbfca}.shell{border:1px solid #384553;border-radius:14px;background:linear-gradient(115deg,#1b2734,#121d29 70%);box-shadow:0 14px 46px #0005,inset 0 1px #50606c50;padding:clamp(10px,1.3vw,19px)}.rxgrid{display:grid;grid-template-columns:minmax(203px,.85fr) minmax(260px,1.45fr) minmax(210px,1fr);gap:9px}.screen{background:linear-gradient(160deg,#101d29,#08131c);border:1px solid #354856;border-radius:7px;box-shadow:inset 0 3px 10px #0006}.meterbox{position:relative;padding:10px 12px;min-height:132px}.small,.scale,.eyebrow{font:11px var(--mono);letter-spacing:.11em;text-transform:uppercase;color:var(--grey)}.eyebrow{color:var(--ice)}.dial{width:100%;height:77px;display:block}.strong{font:700 16px var(--mono);color:var(--green)}.vfo{padding:15px clamp(11px,1.3vw,20px);display:flex;flex-direction:column;justify-content:space-between;gap:7px;border-top:2px solid #41525f}.vfo.active{border-top-color:var(--green)}.vfo.active .freq{color:var(--green);text-shadow:0 0 15px #75ee5d33}.freq{font:600 clamp(24px,3.25vw,59px)/1.2 var(--mono);letter-spacing:-.075em;color:#c8d6e1;white-space:nowrap}.freq span{color:#607a70}.vfo small{font:12px var(--mono);color:#b5c2ce}.rxlabel,.mode{font:700 12px var(--mono);border-radius:4px;padding:3px 7px;color:var(--ice);background:#183647}.rxlabel.live{color:#172b18;background:var(--green)}.rxlabel.tx{background:#ff9078;color:#361a12}.row,nav,.keys{gap:6px;flex-wrap:wrap}button,select,input{font:inherit}button,select{color:#dce6f1;background:linear-gradient(#354656,#273642 48%,#202b37 52%,#253240);border:1px solid #435463;border-bottom-color:#101a24;border-radius:5px;box-shadow:inset 0 1px #60738570,0 2px 0 #081019;padding:7px 12px;cursor:pointer}button{font-size:13px;font-weight:660}button:enabled:hover{border-color:#74bace}button:focus-visible,select:focus-visible,input:focus-visible,summary:focus-visible{outline:2px solid var(--ice);outline-offset:2px}button:active:enabled{transform:translateY(1px)}button.on,button[aria-pressed=true]{color:#b8f7a3;background:linear-gradient(#3e574b,#23382f);box-shadow:inset 0 1px #a6e7a677,0 2px #0b150e;border-color:#588463}button:disabled{cursor:default;color:#81909d}.keys button{min-width:54px;flex:1}.bandbar{margin:12px 0}.overview{display:grid;grid-template-columns:minmax(0,2.5fr) minmax(238px,.9fr);gap:10px}.plot{padding:16px 17px 10px;border-radius:7px;background:#0b1723;border:1px solid var(--line)}h2{font-size:16px;line-height:1.2;margin:0;font-weight:700}.plot .between{align-items:baseline}canvas{width:100%;display:block;height:clamp(160px,22vh,250px)}.scale{display:flex;justify-content:space-between;letter-spacing:0;font-size:10px;margin:0 3px 0 35px}.tune{padding:14px;background:#182431;border:1px solid var(--line);border-radius:7px;display:flex;flex-direction:column;justify-content:space-between;gap:13px}.tune label{font-size:12px;color:#b2bdc9;display:block;margin-bottom:5px}.field{display:flex;gap:5px}.field input{min-width:0;width:100%}input{color:var(--text);background:#0a1520;border:1px solid #455462;border-radius:4px;padding:7px 9px;font-family:var(--mono)}.tune .row{flex-wrap:nowrap}.tune select{min-width:0;flex:1;font-size:13px}select option{background:#192b3a}.groups{margin:12px 0;display:grid;grid-template-columns:1fr 1fr;gap:10px}.block{border:1px solid var(--line);border-radius:7px;padding:12px;background:#15212c}.block h2{margin-bottom:10px}.block .keys{gap:5px}.keys button:before{display:block;content:"";height:2px;width:13px;border-radius:4px;background:#647483;margin:0 auto 5px}.keys .on:before{background:var(--green);box-shadow:0 0 6px #a5eab2}.strip{display:flex;align-items:center;flex-wrap:wrap;gap:9px;border-top:1px solid #384753;padding-top:12px}.strip button{min-height:40px}.ptt{border-color:#895649;color:#ffb1a1;background:linear-gradient(#553a34,#382b2a);touch-action:none;user-select:none}.ptt.held{background:#c14e39;color:white}.hold{touch-action:none;user-select:none}.strip #note{font-size:12px;color:#b2c4d2;flex:1;text-align:right}.panel{margin-top:14px;padding:0 2px}details{border-bottom:1px solid #344451}summary{cursor:pointer;font-weight:700;padding:14px 1px}.readouts{display:grid;grid-template-columns:repeat(auto-fit,minmax(143px,1fr));gap:7px;padding:4px 0 12px}.tile{padding:10px;background:#14202c;border-radius:5px}.tile small{display:block;color:#a8b7c5;font-size:11px}.tile b{font:600 17px var(--mono);display:block;margin:6px 0 0}.diag{font:12px/1.55 var(--mono);color:#bbd2df;overflow-wrap:anywhere}.panel .row{margin-bottom:12px}.panel input{max-width:100%}footer{margin-top:14px;color:#8597a5;font:11px var(--mono);flex-wrap:wrap}.unit{color:#adc4d4;font:12px var(--mono)}[hidden]{display:none!important}@media(min-width:1000px){.vfo:nth-child(3) .freq{font-size:clamp(27px,2.6vw,44px)}}@media(max-width:850px){.rxgrid{grid-template-columns:1fr 1fr}.meterbox{grid-column:1/-1;min-height:0;display:grid;grid-template-columns:100px 1fr 95px;align-items:center}.dial{height:60px}.overview{grid-template-columns:1fr}.tune{display:grid;grid-template-columns:1fr 1fr;align-items:end}.tune h2{grid-column:1/-1}.groups{grid-template-columns:1fr}.sub{display:none}}@media(max-width:530px){main{padding:0 8px 18px}header{min-height:62px}.brand{font-size:19px}.mark{width:27px}.lamps{gap:7px}time{display:none}.rxgrid{gap:5px}.freq{font-size:clamp(19px,5.2vw,30px)}.vfo{padding:9px 7px}.meterbox{grid-template-columns:55px 1fr 70px;padding:4px 8px}.small,.eyebrow{font-size:10px}.tune{display:flex;align-items:stretch}.keys button{min-width:47px;padding:6px}.strip #note{flex-basis:100%;text-align:left}.plot{padding:12px 8px}.plot h2{font-size:14px}.brand+span{display:none}.shell{padding:8px}.led{font-size:10px}}
.wordmark{display:flex;flex-direction:column;align-items:flex-start;line-height:1.05}.wordmark small{font:8px var(--mono);letter-spacing:.22em;color:#a1b5c7;margin-top:3px}.settingsLink{font-size:13px;color:#d5edf7;text-decoration:none;border:1px solid #445565;border-radius:5px;padding:7px 10px;background:#233342}.settingsLink:hover{border-color:var(--ice)}@media(max-width:530px){.settingsLink{font-size:12px;padding:6px}.identity{gap:6px}}
#digital{margin:12px 0 0;background:#101f2b;border:1px solid var(--line);border-radius:7px;padding:0 14px}#digital summary small{margin-left:8px;color:var(--ice);font:12px var(--mono)}#digital p{font-size:13px;color:var(--grey);margin:0 0 10px}#digital a{color:var(--ice)}.decodes{width:100%;max-height:310px;overflow:auto;margin-bottom:12px}.decodes table{border-collapse:separate;border-spacing:0;width:100%;text-align:left;font:13px/1.5 var(--mono)}.decodes th,.decodes td{white-space:nowrap;border-bottom:1px solid #2a3d4b;padding:5px 9px}.decodes th{position:sticky;top:0;background:#1e303f;color:#bdccda;font-size:11px}.decodes td:nth-child(3){color:var(--green);font-weight:650}
</style>
<main><header><div class="identity"><svg class="mark" viewBox="0 0 44 46" aria-hidden="true"><path fill="currentColor" d="M2 4l14 7 6-4 6 4 14-7-4 22-16 17L6 26zm6 6 4 15 10 11 10-11 4-15-10 7-4-4-4 4z"/><path fill="currentColor" d="m13 22 7 2-2 3zm18 0-7 2 2 3z"/></svg><div class="wordmark"><b class="brand">WOLF</b><small>UA3REO</small></div><span class="sub">Панель трансивера</span></div><div class="lamps"><span class="led" id="link">I²C</span><span class="led" id="cat">CAT</span><time id="clock"></time><a class="settingsLink" href="/settings">Настройки</a></div></header>
<section class="shell"><div class="rxgrid"><div class="screen meterbox"><div class="eyebrow">S-метр</div><svg id="dial" class="dial" viewBox="0 0 224 82" role="img" aria-label="Шкала S-метра"></svg><div class="between"><b class="strong" id="dbm">—</b><span class="unit">dBm</span></div></div>
<div class="screen vfo" id="card0"><div class="between"><b class="eyebrow">VFO A</b><span class="mode" id="mode0">—</span></div><strong class="freq" id="hz0">—</strong><div class="between"><small>МГц</small><span class="rxlabel" id="role0">—</span></div></div>
<div class="screen vfo" id="card1"><div class="between"><b class="eyebrow">VFO B</b><span class="mode" id="mode1">—</span></div><strong class="freq" id="hz1">—</strong><div class="between"><small>МГц</small><span class="rxlabel" id="role1">—</span></div></div></div>
<nav class="keys bandbar" id="bands" aria-label="Диапазоны"></nav>
<div class="overview"><div class="plot"><div class="between"><h2>Уровень сигнала RX1</h2><span class="small" id="span">60 с · dBm</span></div><canvas id="history" aria-label="График уровня RX1 за 60 секунд"></canvas><div class="scale"><span>−60 с</span><span>−45</span><span>−30</span><span>−15</span><span>сейчас</span></div></div>
<div class="tune"><div class="between"><h2>Настройка частоты</h2><span id="target" class="eyebrow">VFO —</span></div><div><label for="freqInput">Прямой ввод, Гц</label><form id="freqForm" class="field"><input id="freqInput" inputmode="numeric" autocomplete="off" placeholder="14 074 000" aria-label="Частота в герцах"><button class="write">Ввод</button></form></div><div><label for="step">Шаг перестройки</label><div class="row"><button class="write" id="down" aria-label="Уменьшить частоту">−</button><select id="step" aria-label="Шаг перестройки"></select><button class="write" id="up" aria-label="Увеличить частоту">+</button></div></div></div></div>
<details id="digital"><summary>Цифровой приём <small id="digiCount"></small></summary>
<p><span id="digiStatus" role="status">Ожидание WSJT-X</span> · <a href="/settings#digital-net">Подключение WSJT-X</a></p>
<div class="decodes" id="digiTable" tabindex="0" aria-label="Журнал цифрового приёма" hidden><table><thead><tr><th>UTC</th><th>Режим</th><th>Позывные</th><th>S/N, дБ</th><th>Δt, с</th><th>DF, Гц</th><th>RF, МГц</th><th>Сообщение</th></tr></thead><tbody id="digiRows"></tbody></table></div></details>
<div class="groups"><div class="block"><h2>Вид работы</h2><div class="keys" id="modes"></div></div><div class="block"><h2>Приёмник</h2><div class="keys" id="switches"></div></div></div>
<div class="strip"><button class="write" id="va">VFO A</button><button class="write" id="vb">VFO B</button><button class="write" id="copy">A → B</button><button class="write" id="split">SPLIT</button><button class="write ptt" id="ptt">● PTT · удерживать</button><button class="write hold" id="tune">TUNE · удерживать</button><button id="rx">RX</button><span id="note" role="status">Ожидание трансивера</span></div></section>
<section class="panel"><details open><summary>Показания трансивера</summary><div class="readouts" id="mainTiles"></div></details>
<details><summary>Расширенные параметры и управление</summary><div id="moreTiles" class="readouts"></div><div class="row" id="extraKeys"></div><form class="row" id="attForm"><label for="att">Аттенюатор, дБ</label><input id="att" type="number" min="0" max="31" required><button class="write">Применить</button></form><form class="row" id="powerForm"><label for="power">Мощность возбуждения, %</label><input id="power" type="number" min="0" max="100" required><button class="write">Применить</button></form></details>
<details><summary>Диагностика I²C и CAT</summary><p class="diag" id="diag"></p><form id="queryForm" class="row"><label for="query">Запрос CAT</label><select id="query"></select><button id="ask">Прочитать</button></form><p class="diag" id="manual"></p><form class="row" id="msgForm"><label for="msg">Сообщение на дисплей Волка</label><input id="msg" maxlength="63" placeholder="До 63 знаков ASCII"><button id="sendMsg">Передать</button></form></details></section>
<footer><span>Wolf Interface · YD-ESP32-S3</span><span id="pinInfo">I²C · 0x76</span></footer></main>
<script>
'use strict';
const $=id=>document.getElementById(id), text=(id,s)=>$(id).textContent=s, modeNames=['LSB','USB','CW','NFM','WFM','AM','SAM','SAM-L','SAM-U','DIGL','DIGU','IQ','LOOP','RTTY','DSB'];
let previousVfo=null,state=null, buffer=[], lastSample=0, localNote='', noteUntil=0, browserOnline=false, held=null, busy=false, bootAt=0;
const metrics={
 SM2:['RX1','dBm'],SM3:['RX2','dBm'],FW:['Полоса RX','Гц'],AN:['Антенна',''],PC:['Возбуждение','%'],'RM.W':['Мощность TX','Вт'],'RM.S':['КСВ · шкала CAT','дел.'],
 ID:['CAT ID',''],GC:['АРУ',''],NB:['Бланкер',''],NR:['Шумоподавление',''],NT:['Режектор',''],FS:['Быстрый шаг',''],PA:['Предусилитель',''],RA:['Аттенюатор','дБ'],MT:['Mute',''],LK:['Блокировка',''],SC:['Сканирование',''],AC:['Тюнер · код CAT',''],RG:['RF Gain · CAT',''],AG0:['AF RX1 · CAT',''],AG1:['AF RX2 · CAT',''],MG:['Mic Gain · CAT',''],KS:['Скорость CW','WPM'],GT:['АРУ · код CAT',''],XT:['XIT',''],VX:['VOX',''],NL:['Уровень NB · CAT',''],RL:['Уровень NR · CAT',''],SL:['Срез НЧ · CAT',''],SH:['Срез ВЧ · CAT',''],VD:['Задержка VOX · CAT',''],SM0:['Индикатор 0 · CAT',''],SM1:['Индикатор 1 · CAT','']};
const switchInfo={PA:'PRE',GC:'AGC',NB:'NB',NR:'DNR',NT:'NOTCH',FS:'FAST',MT:'MUTE',LK:'LOCK',SC:'SCAN',XT:'XIT',VX:'VOX'};
function known(k,limit=60000){const m=state?.m?.[k];return m&&browserOnline&&state.online&&((state.at-m[1])>>>0)<limit?m[0]:null}
function say(s){localNote=s;noteUntil=Date.now()+3200;text('note',s)}
function on(id,yes){$(id).classList.toggle('on',!!yes);$(id).setAttribute('aria-pressed',!!yes)}
function newBtn(parent,id,label,act){const b=document.createElement('button');b.id=id;b.textContent=label;b.className='write';b.disabled=true;b.onclick=act;$(parent).append(b)}
async function post(op,v='',owner=0,keepalive=false){
 const res=await fetch('/api/control',{method:'POST',headers:{'X-Wolf-Panel':'1'},body:new URLSearchParams({op,v,owner}),keepalive});
 const s=await res.text();if(!res.ok)throw Error(s||'Запрос отклонён');return s;
}
async function send(op,v=''){
 if(busy)return;busy=true;
 try{const cmd=await post(op,v);say(cmd?'Ожидаем ответ Волка · '+cmd:'Запрос принят')}catch(e){say(e.message)}finally{busy=false}
}
function active(){return state?.v?.[1]}
function freq(i){return state?.v?.[2+i]}
function setHz(n){const i=active();if(i!==0&&i!==1)return say('Нет активного VFO');if(!Number.isSafeInteger(n)||n<=0||n>99999999999)return say('Проверьте частоту в герцах');send(i?'fb':'fa',String(n))}
const bands=[['160 м',1850000,1800000,2000000],['80 м',3650000,3500000,4000000],['60 м',5357000,5250000,5450000],['40 м',7100000,7000000,7300000],['30 м',10120000,10100000,10150000],['20 м',14200000,14000000,14350000],['17 м',18100000,18068000,18168000],['15 м',21200000,21000000,21450000],['12 м',24915000,24890000,24990000],['10 м',28400000,28000000,29700000],['6 м',50125000,50000000,54000000],['2 м',145500000,144000000,148000000]];
bands.forEach(([name,hz],i)=>newBtn('bands','band'+i,name,()=>setHz(hz)));
const modes={'1':0,'2':1,'3':2,'4':3,'5':6,'6':9,'8':11,'9':10};
for(const [cmd,num] of Object.entries(modes))newBtn('modes','m'+cmd,modeNames[num],()=>send('mode',cmd));
function switchVal(k){const raw=known(k);if(raw===null)return null;if(k==='PA')return raw[0]==='1'?1:0;return Number(raw)}
for(const [key,label] of Object.entries(switchInfo)){
 newBtn(['PA','GC','NB','NR','NT','FS'].includes(key)?'switches':'extraKeys','sw'+key,label,()=>{
 const val=switchVal(key);if(val===null)return send('query',key);
 send('switch',key+(key==='NR'?((val+1)%3):Number(!val)));
 });
}
for(const n of [1,10,50,100,500,1000,5000,10000,100000,1000000])$('step').add(new Option(n<1000?n+' Гц':n<1e6?n/1000+' кГц':'1 МГц',n));
$('step').value='100';$('up').onclick=()=>setHz(freq(active())+Number($('step').value));$('down').onclick=()=>setHz(freq(active())-Number($('step').value));
$('freqForm').onsubmit=e=>{e.preventDefault();const v=$('freqInput').value.replace(/\s/g,'');if(!/^\d{1,11}$/.test(v))return say('Введите частоту в герцах');setHz(Number(v))};
$('va').onclick=()=>send('vfo','0');$('vb').onclick=()=>send('vfo','1');$('copy').onclick=()=>send('fb',String(freq(0)));$('split').onclick=()=>send('split',String(Number(!state.v[9])));
$('attForm').onsubmit=e=>{e.preventDefault();send('atten',$('att').value)};
$('powerForm').onsubmit=e=>{e.preventDefault();send('power',$('power').value)};
$('queryForm').onsubmit=e=>{e.preventDefault();send('query',$('query').value)};$('msgForm').onsubmit=e=>{e.preventDefault();send('message',$('msg').value)};
for(const [key,[name]] of Object.entries(metrics)){
 const el=document.createElement('div');el.className='tile';const title=document.createElement('small'),v=document.createElement('b');
 title.textContent=name;v.id='val'+key;v.textContent='—';el.append(title,v);
 $(['SM2','SM3','FW','AN','PC','RM.W','RM.S'].includes(key)?'mainTiles':'moreTiles').append(el);
 if(!key.startsWith('RM.'))$('query').add(new Option(key+' · '+name,key));
}
for(const k of ['FA','FB','VS','FT','MD','IF','OI'])$('query').add(new Option(k,k));
$('query').value='SM2';
function releaseHold(){
 const h=held;if(!h)return;held=null;h.up=true;clearInterval(h.timer);$(h.op).classList.remove('held');
 // If a release races the initial POST, release once more after it resolves.
 post('release','',h.id,true).catch(()=>{});if(!h.started)h.request.finally(()=>post('release','',h.id,true).catch(()=>{})).catch(()=>{});
}
async function press(op,e){
 if(held||$(op).disabled||e.repeat)return;e.preventDefault();
 if(e.pointerId!==undefined){if(e.button!==0)return;$(op).setPointerCapture(e.pointerId)}
 const h={op,id:crypto.getRandomValues(new Uint32Array(1))[0]||1,up:false,started:false,renewing:false};held=h;
 h.request=post(op,'',h.id);
 try{await h.request;h.started=true;if(h.up)return;$(op).classList.add('held');
 h.timer=setInterval(async()=>{if(h.renewing||h.up)return;h.renewing=true;try{await post('renew','',h.id)}catch(e){releaseHold();say(e.message)}finally{h.renewing=false}},350);
 }catch(e){if(held===h)held=null;say(e.message)}
}
for(const op of ['ptt','tune']){
 const b=$(op);b.onpointerdown=e=>press(op,e);b.onpointerup=releaseHold;b.onpointercancel=releaseHold;b.onlostpointercapture=releaseHold;b.oncontextmenu=e=>e.preventDefault();
 b.onkeydown=e=>{if(e.key===' '||e.key==='Enter')press(op,e)};b.onkeyup=e=>{if(e.key===' '||e.key==='Enter')releaseHold()};
}
window.addEventListener('blur',releaseHold);window.addEventListener('pagehide',releaseHold);document.addEventListener('visibilitychange',()=>{if(document.hidden)releaseHold()});
$('rx').onclick=()=>{releaseHold();post('rx').then(()=>say('Запрошен приём')).catch(e=>say(e.message))};
function formatHz(h){return h==null?'—':Math.floor(h/1e6)+'.'+String(Math.floor(h/1000)%1000).padStart(3,'0')+'.'+String(h%1000).padStart(3,'0')}
function drawDial(value,base){
 const svg=$('dial'),point=(n,r)=>{const a=(-33+66*n/15)*Math.PI/180;return [112+r*Math.sin(a),204-r*Math.cos(a)]};
 const p=(n,r)=>point(n,r).join(' ');let html='';
 for(let i=0;i<15;i++){html+='<path d="M '+p(i,176)+' L '+p(i+1,176)+'" stroke="'+(i>=9?'#f5aa71':'#a8de97')+'" stroke-width="2.3"/>'}
 for(let i=0;i<=15;i++){const major=[1,3,5,7,9,11,13,15].includes(i);html+='<path d="M '+p(i,176)+' L '+p(i,major?183:180)+'" stroke="#aebcc5"/>';if(major){const [x,y]=point(i,191);html+='<text x="'+x+'" y="'+y+'" fill="'+(i>9?'#f1aa88':'#c6d2d8')+'" font-family="monospace" text-anchor="middle" font-size="9">'+(i<=9?i:'+'+(i-9)*10)+'</text>'}}
 if(value!==null){const n=Math.max(0,Math.min(15,value<=base?9+(value-base)/6:9+(value-base)/10));html+='<path d="M '+p(n,120)+' L '+p(n,184)+'" stroke="#a8f791" stroke-width="2.4"/>'}
 svg.innerHTML=html;
}
function chart(){
 const c=$('history'),r=c.getBoundingClientRect(),d=devicePixelRatio||1;if(!r.width)return;
 c.width=Math.round(r.width*d);c.height=Math.round(r.height*d);const x=c.getContext('2d');x.scale(d,d);
 const l=32,t=14,w=r.width-l-5,h=r.height-t-9,now=performance.now();x.font='10px monospace';x.textBaseline='middle';
 for(let i=0;i<=5;i++){const y=t+h*i/5;x.fillStyle='#91a1af';x.fillText(String(-30-i*20),0,y);x.strokeStyle='#273e4f';x.beginPath();x.moveTo(l,y);x.lineTo(l+w,y);x.stroke()}
 for(let i=0;i<=8;i++){const xx=l+w*i/8;x.strokeStyle='#203443';x.beginPath();x.moveTo(xx,t);x.lineTo(xx,t+h);x.stroke()}
 buffer=buffer.filter(o=>now-o.t<60000);let paths=[],seg=[];for(const o of buffer){if(o.gap||(seg.length&&o.t-seg.at(-1).t>4000)){if(seg.length)paths.push(seg);seg=[]}if(!o.gap)seg.push(o)}if(seg.length)paths.push(seg);
 const coord=o=>[l+w*(1-(now-o.t)/60000),t+h*(Math.min(130,Math.max(30,-o.v))-30)/100];
 const grad=x.createLinearGradient(0,t,0,t+h);grad.addColorStop(0,'#88db90aa');grad.addColorStop(1,'#28aab81a');
 for(const seg of paths){const pts=seg.map(coord);x.beginPath();pts.forEach(([a,b],i)=>i?x.lineTo(a,b):x.moveTo(a,b));x.strokeStyle='#baf18b';x.lineWidth=2;x.stroke();const last=pts.at(-1);x.lineTo(last[0],t+h);x.lineTo(pts[0][0],t+h);x.closePath();x.fillStyle=grad;x.fill();x.fillStyle='#bcf68b';x.fillRect(last[0]-1,last[1]-1,3,3)}
 if(!paths.length){x.fillStyle='#aab9c7';x.textAlign='center';x.font='13px system-ui';x.fillText('Ожидание отсчётов RX1',l+w/2,t+h/2)}
}
let digiRevision=-1, digiActive=false, digiWasActive=false;
function openDigital(){
 const v=browserOnline&&state?.online?state.v:[];
 const onAir=(v[1]===0||v[1]===1)&&[9,10,13].includes(v[4+v[1]]);
 const active=digiActive||onAir;
 if(active&&!digiWasActive)$('digital').open=true;
 digiWasActive=active;
}
function callsIn(row){
 if(row.call)return row.call;
 const words=row.msg.toUpperCase().replace(/[<>;]/g,' ').split(/\s+/);
 const calls=words.filter(w=>/^[A-Z0-9/]{3,20}$/.test(w)&&/[0-9][A-Z]/.test(w)&&
   !/^[A-R]{2}\d{2}([A-X]{2})?$/.test(w)&&!/^R?[+-]?\d+$/.test(w)&&w!=='RR73');
 return [...new Set(calls)].join(' · ')||'—';
}
function showDecodes(rows){
 const body=$('digiRows');body.replaceChildren();
 for(const r of rows){
  const tr=body.insertRow();tr.title='Источник: '+r.id;
  const utc=new Date(r.utc).toISOString().slice(11,19);
  for(const value of [utc,r.mode,callsIn(r),r.snr,r.dt.toFixed(1),r.df??'—',r.rf==null?'—':(r.rf/1e6).toFixed(6),r.msg+(r.low?' · низкая достоверность':'')]){
   tr.insertCell().textContent=value;
  }
 }
 $('digiTable').hidden=!rows.length;text('digiCount',rows.length?String(rows.length):'');
}
async function digitalPoll(){
 const ctr=new AbortController(), timer=setTimeout(()=>ctr.abort(),2500);
 try{
  const response=await fetch('/api/digital?v='+digiRevision,{cache:'no-store',signal:ctr.signal});
  if(!response.ok)throw Error();
  const data=await response.json();
  if(Array.isArray(data.rows))showDecodes(data.rows);
  digiRevision=data.rev;digiActive=data.active;
  text('digiStatus',!data.listening?'Приём UDP недоступен':data.active?'WSJT-X на связи':'Ожидание данных WSJT-X на UDP 2237');
 }catch{digiActive=false;digiRevision=-1;text('digiStatus','Связь с панелью прервана')}
 finally{clearTimeout(timer);openDigital();setTimeout(digitalPoll,2000)}
}
function render(){
 openDigital();
 if(state?.pins)text('pinInfo','GPIO '+state.pins[0]+' SDA / GPIO '+state.pins[1]+' SCL / 0x76');
 const live=browserOnline&&!!state?.online, v=live?state.v:[],ts=live&&state.ts,active=v[1], tx=v[7];
 text('link',live?'I²C на связи':browserOnline?'I²C ожидание':'ESP вне сети');$('link').className='led '+(live?'good':'bad');
 text('cat',ts?'TS-2000':known('ID')==='0241'?'FT-450':'CAT —');$('cat').className='led '+(ts?'good':'');
 document.querySelectorAll('.write').forEach(b=>b.disabled=!ts);$('rx').disabled=!live;$('ask').disabled=$('sendMsg').disabled=!live;
 for(let i=0;i<2;i++){const isA=active===i,selectedTx=(v[9]===1?1-active:active)===i;
 $('card'+i).classList.toggle('active',isA);text('hz'+i,formatHz(v[2+i]));text('mode'+i,modeNames[v[4+i]]??(v[4+i]!=null?'#'+v[4+i]:'—'));
 const role=tx===1&&selectedTx?'TX':tx===0&&isA?'RX':selectedTx&&v[9]===1?'TX · SPLIT':isA?'VFO':'—';text('role'+i,role);$('role'+i).className='rxlabel '+(role==='RX'?'live':role==='TX'?'tx':'')}
 on('va',active===0);on('vb',active===1);on('split',v[9]===1);on('tune',v[8]===1);
 $('split').disabled=!ts||v[9]==null; $('copy').disabled=!ts||v[2]==null;
 for(const op of ['ptt','tune'])$(op).disabled=!ts||((tx!==0||v[8]!==0||state.held)&&held?.op!==op);
 if(active!==previousVfo){$('freqInput').value='';previousVfo=active}
 text('target',active==null?'VFO —':'VFO '+(active?'B':'A'));text('span','60 с · dBm');
 bands.forEach((b,i)=>on('band'+i,active!=null&&freq(active)>=b[2]&&freq(active)<=b[3]));for(const [c,n] of Object.entries(modes))on('m'+c,active!=null&&v[4+active]===n);
 for(const [k,label] of Object.entries(switchInfo)){const val=switchVal(k);on('sw'+k,val!==null&&val!==0);$('sw'+k).title=val===null?'Нажмите для опроса CAT':label+' · '+val;if(k==='NR')text('sw'+k,val?label+' '+val:label)}
 for(const [k,[,unit]] of Object.entries(metrics)){let raw=known(k,k.startsWith('SM')?10000:60000);if(k.startsWith('RM.')&&(tx!==1||known(k,12000)===null))raw=null;if((k==='SM2'||k==='SM3')&&tx!==0)raw=null;
 const val=raw===null?'—':k==='SM2'||k==='SM3'?-Math.abs(Number(raw)):['ID','PA','AC'].includes(k)?raw:Number(raw);
 text('val'+k,String(val)+(raw!==null&&unit?' '+unit:''))}
 const sm=known('SM2',5000),db=tx===0&&sm!==null?-Math.abs(Number(sm)):null;text('dbm',db===null?'—':db);drawDial(db,active!=null&&freq(active)>=30000000?-93:-73);
 if(db!==null&&lastSample!==state.m.SM2[1]){lastSample=state.m.SM2[1];buffer.push({t:performance.now()-((state.at-lastSample)>>>0),v:db})}
 if(db===null&&buffer.length&&!buffer.at(-1).gap)buffer.push({t:performance.now(),gap:true});chart();
 text('manual',state?.manual?'Ручной запрос · '+state.manual:'');const a=state?.stat||[];text('diag','Кадры состояния '+(a[0]??'—')+' · Опросы 20h '+(a[1]??'—')+' · Чтения CAT '+(a[2]??'—')+' · Ошибки кадров '+(a[3]??'—')+' · Короткие чтения '+(a[4]??'—')+' · Таймауты '+(a[5]??'—')+' | Последняя команда: '+(state?.cmd||'—')+' | Последний ответ: '+(state?.reply||'—'));
 text('note',Date.now()<noteUntil?localNote:!browserOnline?'Нет связи с ESP32':!state.bus?'Ошибка запуска I²C':!live?'Ожидание опроса от Волка':!ts?'Ожидание ID019 · CAT Type TS-2000':state.note||'Управление готово');
}
async function poll(){
 const ctr=new AbortController(),timer=setTimeout(()=>ctr.abort(),1800);
 try{const r=await fetch('/api/state',{cache:'no-store',signal:ctr.signal});if(!r.ok)throw Error();const s=await r.json();if(bootAt>s.at){buffer=[];lastSample=0;digiRevision=-1}bootAt=s.at;state=s;browserOnline=true}
 catch{browserOnline=false;releaseHold()}finally{clearTimeout(timer);render();setTimeout(poll,300)}
}
window.addEventListener('resize',chart);setInterval(()=>text('clock',new Date().toLocaleTimeString('ru-RU')),1000);render();poll();digitalPoll();
</script></html>
)WOLF";
