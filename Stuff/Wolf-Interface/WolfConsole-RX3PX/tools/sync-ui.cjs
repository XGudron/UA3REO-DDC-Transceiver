// Refresh the bundled desktop copy after editing the UI in the sketch.
const fs=require('node:fs'), path=require('node:path');
const root=path.resolve(__dirname,'..'), dest=path.join(root,'desktop','assets');
fs.mkdirSync(dest,{recursive:true});
for (const [source,name] of Object.entries({'Panel.h':'panel.html','PanPage.h':'pan.js','TciPage.h':'tci.js','Player.h':'audio.js'})){
  const header=fs.readFileSync(path.join(root,'WolfConsole',source),'utf8');
  const match=/R"([A-Z]+)\(([\s\S]*?)\)\1";/.exec(header);
  if(!match)throw Error('Missing asset in '+source);
  fs.writeFileSync(path.join(dest,name),match[2]);
}
