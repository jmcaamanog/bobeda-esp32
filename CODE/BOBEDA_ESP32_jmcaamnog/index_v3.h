#pragma once

const char INDEX_HTML[] PROGMEM = R"rawliteral(
<!doctype html><html lang="es"><head><meta charset="utf-8"><meta name="viewport" content="width=device-width,initial-scale=1">
<title>SEC-VAULT v3.5 by jmcaamnog</title><style>
:root{color-scheme:dark}body{margin:0;background:#050705;color:#b8ffb8;font:16px ui-monospace,Consolas,monospace}main{max-width:920px;margin:24px auto;padding:22px;border:1px solid #39e75f;background:#081008;box-shadow:0 0 15px rgba(57,231,95,0.2)}h1,h2{color:#6dff86;border-bottom:1px dashed #39e75f;padding-bottom:5px}input,button,textarea,select{font:inherit;padding:9px;margin:4px 0;background:#050705;color:#d7ffd7;border:1px solid #39e75f;border-radius:3px;outline:none}input:focus,textarea:focus{box-shadow:0 0 8px #39e75f}input{box-sizing:border-box;width:100%}textarea{width:100%;resize:vertical;min-height:60px}button{cursor:pointer;width:auto;font-weight:bold;text-transform:uppercase}button:hover{background:#39e75f;color:#051005}.hidden{display:none!important}.warn{color:#ffd166;border-color:#ffd166}.danger{color:#ff6978;border-color:#ff6978}.row{display:flex;gap:8px;flex-wrap:wrap;align-items:center}.row>*{flex:1;min-width:150px}table{width:100%;border-collapse:collapse;margin-top:12px}th,td{border:1px solid #276e35;padding:7px;text-align:left;word-break:break-word}.small{font-size:.82em;color:#a0c8a0}.notice{padding:10px;border:1px solid #ffd166;margin:10px 0;color:#ffd166}.actions{white-space:nowrap}.actions button{padding:4px 7px;font-size:0.85em}.toolbar{display:flex;gap:8px;flex-wrap:wrap;align-items:center;background:#0a170a;padding:10px;border-radius:4px}.toolbar input{max-width:250px}.toolbar label{border:1px solid #39e75f;padding:8px;cursor:pointer;font-weight:bold;text-transform:uppercase}.toolbar label input{display:none}.tag{background:#11381b;color:#39e75f;padding:2px 6px;border-radius:10px;font-size:0.75em;margin-right:4px;border:1px solid #276e35}details{background:#0a170a;padding:5px;border:1px solid #276e35;margin-top:5px}summary{cursor:pointer;color:#ffd166;font-size:0.85em}
.status-bar{display:flex;justify-content:space-between;border-bottom:1px solid #276e35;padding-bottom:10px;margin-bottom:15px;font-size:0.85em;color:#a0c8a0;font-weight:bold}
</style></head><body><main>
<div class="status-bar">
    <div><span id="lockStatus" style="font-size:1.2em;vertical-align:middle">🔒</span> <span id="connIp">IP: ---.---.-.-</span> | USERS_<span id="connUsers">00</span></div>
    <div id="idleTimer" class="hidden" style="color:#ffd166;font-size:1.1em;letter-spacing:1px">⏱️ 03:00</div>
</div>
<h1>> SEC-VAULT <small>v3.5</small>_</h1><div id="notice" class="notice hidden"></div>
<section id="loading">Comprobando bóveda...</section>
<section id="setup" class="hidden"><h2>> INICIALIZACIÓN_</h2><p>Establece tus credenciales fundacionales.</p>
<input id="newMaster" type="password" autocomplete="new-password" placeholder="Contraseña Maestra (mín. 14 caracteres)">
<div style="height:4px;background:#111;margin-bottom:10px;"><div id="strengthBar" style="height:100%;width:0%;background:#ff6978;transition:0.3s"></div></div>
<input id="newMaster2" type="password" autocomplete="new-password" placeholder="Repite la Contraseña Maestra">
<div style="border:1px dashed #ff6978;padding:10px;margin-top:15px;background:#1a0505">
    <h3 style="margin-top:0;color:#ff6978">> MODO PÁNICO (Opcional)</h3>
    <p class="small">Si te obligan a desbloquear la bóveda, usa esta clave. <b>BORRARÁ TODO AL INSTANTE.</b></p>
    <input id="panicPass" type="password" placeholder="Contraseña de Autodestrucción (Distinta a la maestra)">
</div>
<h3>> RED WI-FI (Camuflaje)</h3><input id="newSsid" maxlength="32" value="Impresora_HP_Piso2" placeholder="Nombre de red (Ej: Impresora_HP)"><input id="newWifiPass" type="password" minlength="8" maxlength="63" autocomplete="new-password" placeholder="Contraseña Wi-Fi (8-63 caracteres)"><button onclick="initialize()">[ BLINDAR E INICIALIZAR ]</button></section>
<section id="login" class="hidden"><h2>> IDENTIFICACIÓN REQUERIDA_</h2><input id="master" type="password" autocomplete="current-password" placeholder="Clave Maestra o de Pánico"><button id="unlockBtn" onclick="unlock()" style="width:100%">[ DESENCRIPTAR ]</button><p id="loginMsg" class="danger"></p></section>
<section id="vault" class="hidden"><div class="toolbar"><button onclick="lock()">[ BLOQUEAR ]</button><button onclick="exportBackup()">[ BACKUP CIFRADO ]</button><label>[ INYECTAR BACKUP ]<input id="backupFile" type="file" accept=".bak,.json,application/json" onchange="importBackup(this.files[0])"></label><button class="warn" onclick="changeMaster()">[ CAMBIAR MAESTRA ]</button></div><p class="small">El portapapeles se limpia al bloquear y se intenta limpiar tras 30 segundos.</p>
<h2>> NUEVO REGISTRO_</h2><div class="row"><input id="site" maxlength="512" placeholder="Programa / Web"><input id="user" maxlength="512" placeholder="Usuario"><input id="password" type="password" maxlength="1024" placeholder="Contraseña"><input id="url" maxlength="1024" placeholder="URL (ej: https://netflix.com)"></div>
<div class="row" style="margin-top:5px;"><input id="tags" maxlength="256" placeholder="Etiquetas (separadas por comas, ej: banco, ocio)"><textarea id="notes" placeholder="Notas secretas, semillas cripto, PINs, PUKs..."></textarea></div>
<div class="toolbar" style="margin-top:10px;background:transparent;border:1px solid #276e35;padding:5px">
    <span style="font-size:0.85em;color:#a0c8a0">⚙️ Auto-Gen:</span>
    <label style="border:none;padding:0"><input type="checkbox" id="gUpper" checked> A-Z</label>
    <label style="border:none;padding:0"><input type="checkbox" id="gLower" checked> a-z</label>
    <label style="border:none;padding:0"><input type="checkbox" id="gNum" checked> 0-9</label>
    <label style="border:none;padding:0"><input type="checkbox" id="gSym" checked> !@#</label>
    <input type="number" id="gLen" value="16" min="4" max="64" style="width:60px;margin:0">
    <button onclick="generatePassword()">[ 🎲 GENERAR ]</button>
</div>
<button onclick="addEntry()" style="width:100%;margin-top:10px;background:#0a220a;border-color:#39e75f">[ GUARDAR REGISTRO ]</button>
<h2>> BASE DE DATOS_</h2><input id="search" placeholder="🔍 Buscar por nombre, usuario, url o etiqueta..." oninput="render()"><div style="overflow:auto"><table><thead><tr><th>Servicio</th><th>Usuario</th><th>Contraseña</th><th>URL</th><th>Acción</th></tr></thead><tbody id="rows"></tbody></table></div></section>
</main><script>
'use strict';
const MAX_ENTRIES=500, MAX_FIELD=2048, idleMs=180000;
let data={entries:[]}, key=null, raw='', lastActivity=Date.now(), failures=0, blockedUntil=0;
const $=id=>document.getElementById(id); const show=(id,on=true)=>$(id).classList.toggle('hidden',!on);
function note(msg){$('notice').textContent=msg;show('notice',true);setTimeout(()=>show('notice',false),5000)}

// TELEMETRÍA (IP y Usuarios Conectados)
async function fetchStatus() {
    try {
        let r = await fetch('/api/status');
        let j = await r.json();
        $('connIp').textContent = 'IP: ' + j.ip;
        $('connUsers').textContent = j.clients.toString().padStart(2, '0');
    } catch(e) {}
}
setInterval(fetchStatus, 5000);
fetchStatus();

// TEMPORIZADOR Y AUTOBLOQUEO
function touch(){lastActivity=Date.now()}
['click','keydown','mousemove'].forEach(e=>document.addEventListener(e,touch,{passive:true}));
setInterval(()=>{
    if(!$('vault').classList.contains('hidden')){
        let left = idleMs - (Date.now() - lastActivity);
        if(left <= 0) lock();
        else {
            let s = Math.ceil(left/1000);
            let m = Math.floor(s/60); s = s%60;
            $('idleTimer').textContent = `⏱️ 0${m}:${s<10?'0':''}${s}`;
            $('idleTimer').style.color = left < 60000 ? '#ff6978' : '#ffd166'; // Rojo si queda < 1 min
        }
    }
}, 1000);

// MEDIDOR DE FUERZA DE CONTRASEÑA
$('newMaster').addEventListener('input', e => {
    let v = e.target.value, s = 0;
    if(v.length > 7) s += 20; if(v.length > 13) s += 30;
    if(/[A-Z]/.test(v)) s += 15; if(/[a-z]/.test(v)) s += 10;
    if(/[0-9]/.test(v)) s += 10; if(/[^A-Za-z0-9]/.test(v)) s += 15;
    $('strengthBar').style.width = Math.min(s, 100) + '%';
    $('strengthBar').style.background = s > 80 ? '#39e75f' : s > 40 ? '#ffd166' : '#ff6978';
});

// MOTOR CRIPTOGRÁFICO RC4 NATIVO (100% Offline)
function cryptRC4(k, t) {
    let s=[], j=0, x, r='';
    for(let i=0;i<256;i++) s[i]=i;
    for(let i=0;i<256;i++) { j=(j+s[i]+k.charCodeAt(i%k.length))%256; x=s[i]; s[i]=s[j]; s[j]=x; }
    let i=0; j=0;
    for(let y=0;y<t.length;y++) { i=(i+1)%256; j=(j+s[i])%256; x=s[i]; s[i]=s[j]; s[j]=x; r+=String.fromCharCode(t.charCodeAt(y)^s[(s[i]+s[j])%256]); }
    return r;
}

async function encrypt(obj, pass, panicPass) {
    const salt = Math.random().toString(36).substring(2, 14);
    const k = pass + "|" + salt;
    const plain = encodeURIComponent(JSON.stringify(obj));
    const enc = cryptRC4(k, plain);
    
    let panicData = null;
    if(panicPass) { 
        const pSalt = Math.random().toString(36).substring(2, 14);
        const pMarker = btoa(cryptRC4(panicPass + "|" + pSalt, "BOOM"));
        panicData = { salt: pSalt, marker: pMarker };
    }
    return { payload: JSON.stringify({ version: 3.5, salt: salt, panic: panicData, data: btoa(enc) }), key: k };
}

async function encryptWithCurrentKey(obj) {
    const prev = JSON.parse(raw);
    const plain = encodeURIComponent(JSON.stringify(obj));
    const enc = cryptRC4(key, plain);
    return JSON.stringify({ version: 3.5, salt: prev.salt, panic: prev.panic, data: btoa(enc) });
}

async function decrypt(payload, pass) {
    const p = JSON.parse(payload);
    if(p.panic) {
        const testMarker = btoa(cryptRC4(pass + "|" + p.panic.salt, "BOOM"));
        if(testMarker === p.panic.marker) return { isPanic: true };
    }
    const k = pass + "|" + p.salt;
    try {
        const dec = cryptRC4(k, atob(p.data));
        const value = JSON.parse(decodeURIComponent(dec));
        if(!Array.isArray(value.entries)) throw Error();
        return { value, k };
    } catch(e) { throw Error('Clave incorrecta'); }
}

function uid(){return Math.random().toString(36).substring(2, 15);}
async function postPayload(p){const r=await fetch('/api/save',{method:'POST',headers:{'Content-Type':'application/x-www-form-urlencoded'},body:'payload='+encodeURIComponent(p)});if(!r.ok)throw Error();raw=p;}
async function save(p, panic){const e=await encrypt(data,p,panic);await postPayload(e.payload);key=e.key;}
async function saveCurrent(){if(!key)throw Error();await postPayload(await encryptWithCurrentKey(data));}
function validEntry(e){return e&&e.site&&[e.site,e.user,e.password,e.url,e.notes,e.tags].every(v=>(v||'').length<=MAX_FIELD);}

async function load(){try{const r=await fetch('/api/load',{cache:'no-store'});raw=await r.text();show('loading',false);show(raw.trim()?'login':'setup')}catch(e){$('loading').textContent='Error de conexión WiFi';}}
async function initialize(){
    const p=$('newMaster').value, p2=$('newMaster2').value, pp=$('panicPass').value, s=$('newSsid').value.trim(), w=$('newWifiPass').value;
    if(p.length<14||p!==p2) return note('La contraseña maestra debe coincidir y tener > 14 caracteres.');
    if(pp && pp===p) return note('La clave de pánico NO puede ser igual a la maestra.');
    data={entries:[]};
    try{
        await save(p, pp);
        await fetch('/api/wifi',{method:'POST',headers:{'Content-Type':'application/x-www-form-urlencoded'},body:'ssid='+encodeURIComponent(s)+'&pass='+encodeURIComponent(w)});
        note('¡BÓVEDA INICIALIZADA! Reiniciando ESP8266...');
        setTimeout(() => location.reload(), 3000);
    }catch(e){note('Error al guardar.');}
}

async function unlock(){
    if(Date.now()<blockedUntil) return note('Bloqueado temporalmente.');
    const p=$('master').value; if(!p)return;
    $('unlockBtn').disabled=true;
    try{
        const d=await decrypt(raw,p);
        if(d.isPanic) { 
            await postPayload(''); 
            data={entries:[]}; raw=''; show('login',false); show('setup');
            return note('⚠️ MODO PÁNICO ACTIVADO. BÓVEDA PURGADA.');
        }
        data=d.value; key=d.k; failures=0; $('master').value=''; 
        show('login',false); show('vault'); touch(); render();
        // UI Candado Abierto y Timer
        $('lockStatus').textContent = '🔓'; 
        $('idleTimer').classList.remove('hidden');
    }catch(e){
        failures++; const delay=Math.min(60000,1000*2**Math.min(failures,6)); blockedUntil=Date.now()+delay;
        $('loginMsg').textContent='Acceso denegado. Bloqueo de '+(delay/1000)+'s.';
    }finally{ $('unlockBtn').disabled=false; }
}

function lock(){
    data={entries:[]};key=null;show('vault',false);show('login');$('master').value='';
    if(navigator.clipboard?.writeText)navigator.clipboard.writeText('').catch(()=>{});
    note('Bóveda bloqueada.');
    // UI Candado Cerrado y Ocultar Timer
    $('lockStatus').textContent = '🔒';
    $('idleTimer').classList.add('hidden');
}

function cell(tr,text){const td=document.createElement('td');td.textContent=text;tr.appendChild(td);return td}
function safeUrl(i){try{const u=new URL(i);return ['https:','http:'].includes(u.protocol)?u.href:null}catch{return null}}

function render(){
    const body=$('rows'), t=$('search').value.toLowerCase(); body.replaceChildren();
    data.entries.filter(e=>(e.site+' '+e.user+' '+e.url+' '+(e.tags||'')).toLowerCase().includes(t)).forEach(e=>{
        const tr=document.createElement('tr');
        
        let siteHTML = `<strong>${e.site}</strong>`;
        if(e.tags) {
            siteHTML += '<br>' + e.tags.split(',').map(tag => `<span class="tag">#${tag.trim()}</span>`).join('');
        }
        if(e.notes) {
            const safeNotes = e.notes.replace(/&/g,"&amp;").replace(/</g,"&lt;");
            siteHTML += `<details><summary>Ver notas secretas</summary><pre style="white-space:pre-wrap;font-size:0.85em;color:#ffd166">${safeNotes}</pre></details>`;
        }
        const tdSite = document.createElement('td'); tdSite.innerHTML = siteHTML; tr.appendChild(tdSite);

        const u=cell(tr,e.user); copyButton(u,e.user,'Copiar usuario');
        const pw=cell(tr,'••••••••'); copyButton(pw,e.password,'Copiar contraseña');
        
        const linkCell=document.createElement('td'),href=safeUrl(e.url);
        if(href){const a=document.createElement('a');a.href=href;a.target='_blank';a.textContent=e.url;a.style.color='#39e75f';linkCell.appendChild(a)}
        else linkCell.textContent=e.url; tr.appendChild(linkCell);
        
        const act=document.createElement('td'); act.className='actions';
        const del=document.createElement('button'); del.className='danger'; del.textContent='[ X ]'; del.onclick=()=>removeEntry(e.id);
        act.appendChild(del); tr.appendChild(act); body.appendChild(tr);
    });
}

function copyButton(p,text,label){const b=document.createElement('button');b.textContent='📋';b.title=label;b.style.marginLeft='5px';b.onclick=()=>copy(text);p.appendChild(b)}
async function copy(text){try{await navigator.clipboard.writeText(text);note('Copiado (se limpia en 30s)');setTimeout(()=>navigator.clipboard?.writeText('').catch(()=>{}),30000)}catch{note('Portapapeles bloqueado por navegador')}}

function generatePassword(){
    let chars = "";
    if($('gUpper').checked) chars += "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    if($('gLower').checked) chars += "abcdefghijklmnopqrstuvwxyz";
    if($('gNum').checked) chars += "0123456789";
    if($('gSym').checked) chars += "!@#$%^&*()_+~|}{[]:;?><,./-=";
    if(chars === "") { chars = "abcdefghijklmnopqrstuvwxyz"; $('gLower').checked = true; }
    
    let p = "", len = parseInt($('gLen').value) || 16;
    for(let i=0; i<len; i++) p += chars.charAt(Math.floor(Math.random() * chars.length));
    $('password').value = p; $('password').type = 'text'; // Lo mostramos un momento
}

async function addEntry(){
    const e={id:uid(),site:$('site').value.trim(),user:$('user').value,password:$('password').value,url:$('url').value.trim(),notes:$('notes').value,tags:$('tags').value};
    if(!e.site||!e.password) return note('Falta Servicio o Contraseña.');
    data.entries.push(e);
    try{ await persistCurrent(); ['site','user','password','url','notes','tags'].forEach(id=>$(id).value=''); $('password').type='password'; render(); note('Guardado.'); }
    catch(err){ data.entries.pop(); note('Error al guardar.'); }
}

async function persistCurrent(){await saveCurrent()}
async function removeEntry(id){if(!confirm('¿Purgar este registro?'))return;const old=data.entries;data.entries=data.entries.filter(e=>e.id!==id);try{await persistCurrent();render();note('Registro purgado.')}catch(e){data.entries=old;note('Error al borrar.')}}
function download(name,content){const a=document.createElement('a');a.href=URL.createObjectURL(new Blob([content],{type:'application/json'}));a.download=name;a.click();setTimeout(()=>URL.revokeObjectURL(a.href),1000)}
function exportBackup(){if(raw)download('sec-vault-v3.5.bak',raw)}
async function importBackup(file){
    if(!file)return;
    try{
        const content=await file.text(),p=prompt('Contraseña del backup:'); if(!p)return;
        const d=await decrypt(content,p); if(d.isPanic) return note('No puedes restaurar usando la clave de pánico.');
        if(!confirm('Sustituirá la bóveda actual. ¿Continuar?'))return;
        data=d.value; key=d.k; await postPayload(content); render(); note('Backup importado.');
    }catch(e){note('Backup rechazado.')}finally{$('backupFile').value=''}
}
async function changeMaster(){
    const old=prompt('Contraseña actual:'),next=prompt('Nueva contraseña maestra (mín. 14):'),again=prompt('Repite la nueva contraseña:');
    if(!old||!next||next.length<14||next!==again)return note('Operación cancelada.');
    try{
        const prevObj = JSON.parse(raw);
        const d=await decrypt(raw,old); if(d.isPanic) return;
        await save(next, null); 
        note('Contraseña actualizada.')
    }catch(e){note('Error al cambiar contraseña.')}
}
load();
</script></body></html>
)rawliteral";