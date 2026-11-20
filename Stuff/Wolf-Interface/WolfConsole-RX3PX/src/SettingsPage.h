#pragma once
static const char SETTINGS_PAGE[] PROGMEM = R"WSET(<!doctype html>
<html lang="ru"><meta charset="utf-8"><meta name="viewport" content="width=device-width, initial-scale=1"><title>WOLF · Настройки</title>
<style>
:root{color-scheme:dark;--line:#344654;--ice:#7cdaef;--dim:#abbac7}*{box-sizing:border-box}body{background:#0e1822;color:#e4edf4;font:15px system-ui,sans-serif;margin:0}main{max-width:860px;margin:auto;padding:20px 16px 38px}header,.row{display:flex;align-items:center;gap:10px;flex-wrap:wrap}header{justify-content:space-between;margin:4px 0 22px}.logo{font-weight:800;letter-spacing:.15em;font-size:24px;display:inline-flex;flex-direction:column;line-height:1}.logo small{font:8px monospace;letter-spacing:.22em;color:var(--dim);margin-top:3px}a{color:var(--ice)}h1{margin:0;font-size:25px}h2{font-size:18px;margin:0 0 12px}section{background:linear-gradient(140deg,#1c2a37,#14202b);border:1px solid var(--line);border-radius:10px;padding:17px 19px;margin:12px 0}label{display:grid;gap:5px;font-size:13px;color:#c9dae5;margin:8px 0}.row{align-items:end}input,select,button{font:inherit;border-radius:5px;padding:8px 10px;max-width:100%}input,select{background:#0c1721;color:#eef4f9;border:1px solid #425765}button{color:#e8f6ff;background:linear-gradient(#354b5b,#233442);box-shadow:inset 0 1px #70889955,0 2px #0b1319;border:1px solid #486173;cursor:pointer}button:disabled{opacity:.55;cursor:default}button:hover:enabled{border-color:var(--ice)}p{line-height:1.5;margin:7px 0;color:var(--dim)}.status{color:#b1edaa;font-size:14px;min-height:1.2em}.err{color:#ffac93}code{font-size:14px;overflow-wrap:anywhere}progress{width:100%;margin-top:8px}small{color:var(--dim)}[hidden]{display:none!important}#ip{font:650 17px monospace}details{margin:10px 0}summary{cursor:pointer}input[type=file]{width:100%}@media(max-width:500px){main{padding:10px}section{padding:14px}.row label{width:100%}input,select{width:100%}header{gap:6px}h1{font-size:21px}}
</style><main><header><div class="logo">WOLF<small>UA3REO</small></div><h1>Настройки</h1><a href="/">К панели</a></header>
<section><h2>Домашняя сеть Wi-Fi</h2><p id="linkState">Получение адреса…</p><div id="links" hidden><a id="ip"></a> &nbsp; <a id="mdns"></a></div>
<p>Точка настройки <b>WOLF-Console</b> · <span id="apIp">192.168.4.1</span></p>
<form id="wifi"><div class="row"><button type="button" id="scan">Найти сети</button><label id="listBox" hidden>Найденные сети<select id="list"></select></label></div>
<label>Имя сети (SSID)<input id="ssid" required maxlength="32" autocomplete="off"></label><label>Пароль домашней сети<input id="pass" type="password" maxlength="64" autocomplete="new-password" placeholder="Оставьте пустым для открытой сети"></label>
<button>Подключить и сохранить</button></form><p class="status" id="wifiResult" role="status"></p><p id="networkNote"></p></section>
<section><h2>Управление настройками</h2><div id="remote" hidden><label>Код управления<input id="key" type="password" maxlength="24" autocomplete="off"></label><p>Код можно посмотреть на этой странице через точку WOLF-Console. Он действует для настроек и обновления.</p></div>
<details id="local" hidden><summary>Показать код для управления из домашней сети</summary><p><code id="code"></code></p></details>
<details><summary>Изменить пароль точки WOLF-Console</summary><form id="apForm"><label>Новый пароль точки доступа<input type="password" id="apPass" minlength="8" maxlength="63" autocomplete="new-password" required></label><button>Сохранить и перезапустить</button></form></details><p class="status" id="apResult" role="status"></p></section>
<section><h2>Выводы шины I²C</h2><form id="pins"><div class="row"><label>SDA<select id="sda"></select></label><label>SCL<select id="scl"></select></label><button>Применить и перезапустить</button></div></form><p>По умолчанию SDA — GPIO5, SCL — GPIO18. Переставляйте провода при отключённом питании.</p><p class="status" id="pinResult" role="status"></p></section>
<section><h2>Обновление прошивки ESP32</h2><p>Версия панели <b id="version">—</b> · YD-ESP32-S3 N16R8</p><form id="ota"><label>Файл приложения <code>_OTA.bin</code><input id="bin" type="file" accept=".bin" required></label><button>Обновить по Wi-Fi</button></form>
<progress id="progress" value="0" max="100" hidden></progress><p class="status" id="otaResult" role="status"></p></section>
<section id="digital-net"><h2>Цифровой приём · WSJT-X</h2><p>Подайте звук приёмника WOLF на компьютер и выберите этот вход в WSJT-X. В разделе Settings → Reporting укажите UDP Server <b id="digiIp">—</b> и порт <b>2237</b>. Для приёма сообщений включать управление по UDP не требуется.</p><p>Декодированные сообщения появятся в журнале на главной панели. Шина Wolf Interface передаёт состояние трансивера. Текст декодов поступает по Wi-Fi от WSJT-X.</p></section>
<section><h2>О программе</h2><p>Программное обеспечение веб-панели сгенерировано с помощью искусственного интеллекта (ИИ).</p><p>Позывной проекта веб-панели · <b>RX3PX</b></p></section></main>
<script>
'use strict';
const $=id=>document.getElementById(id);
let state=null,configured=false,looking=false,uploading=false;
function say(id,msg,error=false){const e=$(id);e.textContent=msg;e.classList.toggle('err',error)}
function headers(){return {'X-Wolf-Panel':'1','X-Wolf-Key':$('key').value.trim()}}
async function request(url,method='GET',data){
 const r=await fetch(url,{method,headers:headers(),body:data?new URLSearchParams(data):undefined,cache:'no-store'});
 if(!r.ok)throw Error(await r.text()||'Ошибка запроса');return r;
}
async function refresh(){
 if(uploading)return;
 try{const s=await (await request('/api/settings')).json();state=s;
 $('local').hidden=!s.local;$('remote').hidden=s.local;$('code').textContent=s.admin;$('version').textContent=s.version;
 $('digiIp').textContent=s.connected?s.ip:s.ap;$('apIp').textContent=s.ap;$('linkState').textContent=s.connected?'Подключено к '+s.ssid:'Домашняя сеть не подключена';
 $('links').hidden=!s.connected;
 for(const [id,value] of [['ip',s.ip],['mdns',s.mdns]]){const el=$(id);el.hidden=!value;el.href=value?'http://'+value+'/':'/';el.textContent=value}
 $('networkNote').textContent=s.note;
 if(!configured){
 for(const id of ['sda','scl']){const el=$(id);for(const p of s.pins)el.add(new Option('GPIO '+p,p));el.value=s[id]}
 $('ssid').value=s.saved||s.ssid;configured=true;
 }
 }catch(e){$('linkState').textContent='Панель недоступна. После смены сети откройте её локальный адрес.'}
}
async function action(form,endpoint,data,out){
 const b=$(form).querySelector('button:not([type=button])');b.disabled=true;
 try{const r=await request(endpoint,'POST',data);say(out,await r.text());await refresh();return true}
 catch(e){say(out,e.message,true);return false}finally{b.disabled=false}
}
$('wifi').onsubmit=async e=>{e.preventDefault();if(await action('wifi','/api/wifi',{ssid:$('ssid').value,pass:$('pass').value},'wifiResult'))$('pass').value=''};
$('apForm').onsubmit=async e=>{e.preventDefault();if(await action('apForm','/api/ap-password',{pass:$('apPass').value},'apResult'))$('apPass').value=''};
$('pins').onsubmit=e=>{e.preventDefault();if($('sda').value===$('scl').value)return say('pinResult','SDA и SCL должны быть разными.',true);action('pins','/api/pins',{sda:$('sda').value,scl:$('scl').value},'pinResult')};
$('scan').onclick=async()=>{
 if(looking)return;looking=true;$('scan').disabled=true;say('wifiResult','Идёт поиск сетей 2,4 ГГц…');
 try{await request('/api/scan','POST');for(let t=0;t<20;t++){
 await new Promise(f=>setTimeout(f,650));const r=await request('/api/scan');
 if(r.status===202)continue;const nets=await r.json(),sel=$('list');sel.replaceChildren(new Option('Выберите сеть',''));
 const seen=new Set();for(const n of nets){if(seen.has(n.ssid))continue;seen.add(n.ssid);sel.add(new Option(n.ssid+' · '+(n.open?'открытая':'с паролем')+' · '+n.db+' dBm',n.ssid))}
 $('listBox').hidden=!seen.size;say('wifiResult',seen.size?'Выберите сеть или введите SSID вручную.':'Введите имя сети вручную.');return;
 }throw Error('Поиск занял слишком много времени.')}
 catch(e){say('wifiResult',e.message,true)}finally{looking=false;$('scan').disabled=false}
};
$('list').onchange=()=>{if($('list').value){$('ssid').value=$('list').value;$('pass').value=''}};
$('ota').onsubmit=async e=>{
 e.preventDefault();const file=$('bin').files[0];if(!file||uploading)return;
 const button=$('ota').querySelector('button');
 try{
 const b=new Uint8Array(await file.slice(0,36).arrayBuffer());
 if(b.length<36||b[0]!==233||b[12]!==9||b[13]!==0||b[32]!==50||b[33]!==84||b[34]!==205||b[35]!==171)throw Error('Выберите файл приложения _OTA.bin. Полный USB-образ здесь не подходит.');
 uploading=true;button.disabled=true;$('progress').hidden=false;$('progress').value=0;say('otaResult','Передача файла…');
 const result=await new Promise((resolve,reject)=>{const x=new XMLHttpRequest();x.open('POST','/api/update');x.timeout=180000;
 for(const [k,v] of Object.entries(headers()))x.setRequestHeader(k,v);x.setRequestHeader('X-Wolf-Size',String(file.size));
 x.upload.onprogress=p=>{if(p.lengthComputable){$('progress').value=100*p.loaded/p.total;if(p.loaded===p.total)say('otaResult','Проверка образа на ESP32…')}};
 x.onload=()=>x.status===200?resolve(x.responseText):reject(Error(x.responseText||'Обновление отклонено'));
 x.onerror=x.ontimeout=()=>reject(Error('Соединение прервано. Проверьте версию панели после подключения.'));
 const body=new FormData();body.append('firmware',file);x.send(body)});
 say('otaResult',result);$('bin').value='';
 }catch(e){say('otaResult',e.message,true)}finally{uploading=false;button.disabled=false}
};
refresh();setInterval(refresh,3000);
</script></html>)WSET";
