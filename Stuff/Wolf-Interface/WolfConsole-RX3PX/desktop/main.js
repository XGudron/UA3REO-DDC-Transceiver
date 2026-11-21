'use strict';
const { app, BrowserWindow, Menu, ipcMain, session } = require('electron');
const fs = require('node:fs');
const path = require('node:path');
const { isIPv4 } = require('node:net');
const { pathToFileURL } = require('node:url');

const connectPage = pathToFileURL(path.join(__dirname, 'connect.html')).href;
const assets = new Map([['/', 'panel.html'], ['/tci.js', 'tci.js'], ['/pan.js', 'pan.js'], ['/audio.js', 'audio.js']]);
let win, address = '', origin = '';
function normalizeAddress(raw) {
  const input = String(raw).trim();
  if (!input || input.length > 128) throw Error('Введите IP-адрес ESP32.');
  const u = new URL(input.includes('://') ? input : 'http://' + input);
  if (u.protocol !== 'http:' || u.username || u.password || u.pathname !== '/' ||
      u.search || u.hash || u.port || !(isIPv4(u.hostname) || ['wolf-console.local', 'wolf-console'].includes(u.hostname)))
    throw Error('Укажите IPv4 платы или wolf-console.local, без порта и пути.');
  return u.hostname;
}
const settingsPath = () => path.join(app.getPath('userData'), 'station.json');
async function panel(host) {
  address = normalizeAddress(host);
  origin = 'http://' + address;
  await win.loadURL(origin + '/');
}
function showAddress() { return win.loadURL(connectPage); }
function menu() {
  const items = [
    {label: 'Станция', submenu: [
      {label: 'Адрес ESP32…', accelerator: 'CmdOrCtrl+K', click: showAddress},
      {label: 'Пульт', accelerator: 'CmdOrCtrl+1', click: () => address ? panel(address) : showAddress()},
      {label: 'Настройки ESP32', click: () => address ? win.loadURL(origin + '/settings') : showAddress()},
      {type:'separator'}, {role:'quit', label:'Выход'}
    ]},
    {label:'Вид', submenu:[{role:'reload',label:'Обновить окно'},{role:'resetZoom',label:'Исходный масштаб'},
      {role:'zoomIn',label:'Крупнее'},{role:'zoomOut',label:'Мельче'},{role:'togglefullscreen',label:'Полный экран'}]}
  ];
  if (process.platform === 'darwin') items.unshift({role:'appMenu'});
  items.splice(items.length-1, 0, {label:'Правка',submenu:[{role:'undo'},{role:'cut'},{role:'copy'},{role:'paste'},{role:'selectAll'}]});
  Menu.setApplicationMenu(Menu.buildFromTemplate(items));
}
function trustedDialog(e) { return e.sender === win?.webContents && e.senderFrame?.url === connectPage; }
app.whenReady().then(async () => {
  try { address = normalizeAddress(JSON.parse(fs.readFileSync(settingsPath(), 'utf8')).host); } catch {}
  ipcMain.handle('station-current', e => trustedDialog(e) ? address : '');
  ipcMain.handle('station-open', async (e, host) => {
    if (!trustedDialog(e)) return;
    const selected = normalizeAddress(host);
    await fs.promises.writeFile(settingsPath(), JSON.stringify({host:selected}), {mode:0o600});
    await panel(selected);
  });
  // Bundled controls share the board's origin. Settings, PCM and decoder results use its live endpoints.
  const ses = session.defaultSession;
  ses.protocol.handle('http', async request => {
    const u = new URL(request.url);
    if (!origin || u.hostname !== address || !['','81'].includes(u.port)) return new Response('', {status:403});
    const asset = u.origin === origin && request.method === 'GET' && assets.get(u.pathname);
    if (asset) return new Response(await fs.promises.readFile(path.join(__dirname,'assets',asset)), {
      headers:{'Content-Type':asset.endsWith('.js')?'application/javascript; charset=utf-8':'text/html; charset=utf-8',
        'Cache-Control':'no-store'}
    });
    return ses.fetch(request, {bypassCustomProtocolHandlers:true});
  });
  ses.setPermissionRequestHandler((_wc, permission, callback, details) => {
    // Chromium may prompt for local-network access to the selected station.
    const same = details.requestingUrl && new URL(details.requestingUrl).origin === origin;
    callback(Boolean(same && ['local-network-access','local-network'].includes(permission)));
  });
  function create() {
    win = new BrowserWindow({width:1330,height:900,minWidth:760,minHeight:590,backgroundColor:'#0c131b',
      title:'WOLF Console',webPreferences:{preload:path.join(__dirname,'preload.js'),sandbox:true,
        nodeIntegration:false,contextIsolation:true}});
    win.webContents.setWindowOpenHandler(() => ({action:'deny'}));
    win.webContents.on('will-navigate',(e,url)=>{
      if (url !== connectPage && (!origin || new URL(url).origin !== origin)) e.preventDefault();
    });
    win.on('closed',()=>{ win=null; });
    menu();
    if (address) panel(address); else showAddress();
  }
  create();
  app.on('activate',()=>{if (!win) create()});
});
app.on('window-all-closed',()=>{if(process.platform!=='darwin')app.quit()});
