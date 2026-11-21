const {contextBridge, ipcRenderer} = require('electron');
if (location.protocol === 'file:' && location.pathname.endsWith('/connect.html')) {
  contextBridge.exposeInMainWorld('station', {
    current: () => ipcRenderer.invoke('station-current'),
    open: host => ipcRenderer.invoke('station-open', host)
  });
}
