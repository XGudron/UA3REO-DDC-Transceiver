'use strict';
const host=document.getElementById('host'), output=document.querySelector('output');
window.station.current().then(saved=>{if(saved)host.value=saved});
document.querySelector('form').onsubmit=async event=>{
  event.preventDefault();
  try{await window.station.open(host.value)}
  catch(e){output.textContent=e.message.replace(/^Error invoking remote method .*?Error: /,'')}
};
