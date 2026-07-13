/*

       ██╗███╗   ███╗ ██████╗
       ██║████╗ ████║██╔════╝
       ██║██╔████╔██║██║
  ██   ██║██║╚██╔╝██║██║
  █████╔╝ ██║ ╚═╝ ██║╚██████╗
  ╚════╝  ╚═╝     ╚═╝ ╚═════╝
  PROYECTO: Bóveda de contraseñas offline
  Librerías necesarias en Arduino IDE:
  - ESP8266WiFi
  - ESPAsyncTCP
  - ESPAsyncWebServer
  - LittleFS
*/

#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <LittleFS.h>

// Quitamos las constantes fijas y ponemos variables dinámicas y flags de
// reinicio
String currentSSID = "FortKnox_D1";
String currentWifiPass = "123456789";
bool needsReboot = false;
unsigned long rebootTimer = 0;

AsyncWebServer server(80);

// =======================================================================
// INTERFAZ WEB Y LÓGICA DE ENCRIPTACIÓN NATIVA (CLIENT-SIDE)
// =======================================================================
const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="es">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>SEC-VAULT - jmcaamnog</title>
    <style>
        /* ESTÉTICA RETRO TERMINAL CYBERPUNK / NOSTROMO */
        body { font-family: 'Courier New', Courier, monospace; background-color: #050505; color: #0f0; margin: 0; padding: 20px; text-shadow: 0 0 2px #0f0; }
        .container { max-width: 900px; margin: 0 auto; background: #000; padding: 20px; border: 1px solid #0f0; box-shadow: 0 0 10px #0f0; border-radius: 2px; }
        .container.red-alert { border-color: #f00; box-shadow: 0 0 15px #f00; color: #f00; text-shadow: 0 0 2px #f00; }
        
        h1, h2 { text-align: center; border-bottom: 1px dashed #0f0; padding-bottom: 10px; margin-top: 0; text-transform: uppercase; letter-spacing: 2px; }
        .red-alert h1 { border-bottom-color: #f00; color: #f00; }
        
        input[type="text"], input[type="password"], input[type="file"] { width: 100%; box-sizing: border-box; padding: 10px; margin: 5px 0 15px; border: 1px solid #0f0; background: #050505; color: #0f0; font-family: 'Courier New', Courier, monospace; outline: none; }
        .red-alert input { border-color: #f00; color: #f00; }
        input:focus { box-shadow: 0 0 8px #0f0; }
        .red-alert input:focus { box-shadow: 0 0 8px #f00; }
        
        button { background-color: #000; color: #0f0; border: 1px solid #0f0; padding: 10px 20px; cursor: pointer; font-weight: bold; margin-bottom: 10px; font-family: 'Courier New', monospace; transition: all 0.2s; text-transform: uppercase; }
        button:hover { background-color: #0f0; color: #000; box-shadow: 0 0 10px #0f0; }
        button:disabled { border-color: #050; color: #050; cursor: not-allowed; box-shadow: none; text-shadow: none; }
        
        button.btn-red { border-color: #f00; color: #f00; }
        button.btn-red:hover { background-color: #f00; color: #000; box-shadow: 0 0 10px #f00; }
        
        button.action-btn { background: transparent; border: none; font-size: 18px; padding: 0; margin-left: 5px; box-shadow: none; min-width: auto; margin-bottom: 0; cursor: pointer; text-shadow: none; }
        button.action-btn:hover { background: transparent; transform: scale(1.2); color: #fff; box-shadow: none; }

        table { width: 100%; border-collapse: collapse; margin-top: 20px; }
        th, td { border: 1px solid #0f0; padding: 8px; text-align: left; }
        th { background-color: #0a220a; white-space: nowrap; font-size: 13px; letter-spacing: 1px; }
        td { word-break: break-word; font-size: 14px; }
        
        .hidden { display: none !important; }
        .modal { display: none; position: fixed; top: 0; left: 0; width: 100%; height: 100%; background: rgba(0,0,0,0.95); justify-content: center; align-items: center; z-index: 100; }
        .modal-content { background: #000; padding: 20px; border: 1px solid #f00; box-shadow: 0 0 20px #f00; max-width: 400px; width: 100%; }
        
        #loading { color: #0f0; font-weight: bold; margin-top: 10px; display: none; text-align: center; animation: blink 1s infinite; }
        .footer { text-align: center; margin-top: 30px; font-size: 12px; border-top: 1px dashed #0f0; padding-top: 15px; opacity: 0.7; letter-spacing: 1px; }
        
        .box-title { border: 1px solid #0f0; padding: 15px; text-align: center; display: inline-block; margin-bottom: 20px; box-shadow: inset 0 0 10px rgba(0,255,0,0.2); }
        .red-alert .box-title { border-color: #f00; box-shadow: inset 0 0 10px rgba(255,0,0,0.2); }
        
        /* Notificaciones Toast */
        #toast { visibility: hidden; min-width: 250px; background-color: #000; color: #0f0; text-align: center; border: 1px solid #0f0; padding: 16px; position: fixed; z-index: 1000; right: 30px; bottom: 30px; box-shadow: 0 0 15px #0f0; font-weight: bold; text-transform: uppercase; }
        #toast.show { visibility: visible; animation: fadein 0.5s, fadeout 0.5s 2.5s; }
        @keyframes fadein { from {bottom: 0; opacity: 0;} to {bottom: 30px; opacity: 1;} }
        @keyframes fadeout { from {bottom: 30px; opacity: 1;} to {bottom: 0; opacity: 0;} }
        @keyframes blink { 0% { opacity: 1; } 50% { opacity: 0; } 100% { opacity: 1; } }
    </style>
</head>
<body>

<!-- PANTALLA 1: BIENVENIDA NOSTROMO -->
<div class="container" id="welcomeScreen" style="text-align: center; padding-top: 60px; padding-bottom: 60px;">
    <div class="box-title">
        <div style="font-size: 40px; margin-bottom: 10px;">[ 🔒 ]</div>
        <div style="font-size: 24px; font-weight: bold; letter-spacing: 3px;">SEC-VAULT SYSTEM</div>
        <div style="font-size: 12px; margin-top: 5px;">TERMINAL DE ACCESO RESTRINGIDO</div>
    </div>
    
    <div style="margin: 30px 0; line-height: 1.8;">
        <p>ATENCIÓN: ESTÁS A PUNTO DE ACCEDER A LA CAJA FUERTE DE CLAVES.</p>
        <p>Cualquier intento de intrusión será registrado localmente.<br>El sistema utiliza encriptación de grado militar AES/RC4.</p>
    </div>
    
    <button onclick="checkSystemState()" style="font-size: 18px; padding: 15px 30px; animation: blink 2s infinite;">[ INICIAR CONEXIÓN ]</button>
    <div id="welcomeLoading" style="display:none; margin-top: 20px;">> VERIFICANDO INTEGRIDAD DEL SISTEMA...</div>
</div>

<!-- PANTALLA 2: CONFIGURACIÓN PRIMERA VEZ (ALERTA ROJA) -->
<div class="container red-alert hidden" id="setupScreen">
    <div style="text-align: center;">
        <div class="box-title" style="color: #f00;">
            <div style="font-size: 24px; font-weight: bold;">[!] PROTOCOLO DE INICIALIZACIÓN</div>
        </div>
    </div>
    
    <p style="text-align: justify; font-weight: bold;">ADVERTENCIA CRÍTICA: El sistema está vacío. Debes establecer las claves maestras fundacionales. <span style="text-decoration: underline;">ESTAS CLAVES NO PODRÁN CAMBIARSE NUNCA MÁS.</span> Si las olvidas, tus datos se perderán para siempre en el vacío digital. Apúntalas en un papel y guárdalo bajo llave.</p>
    
    <h3 style="border-bottom: 1px solid #f00; padding-bottom: 5px;">> FASE 1: CLAVES DE DESENCRIPTADO (Para entrar)</h3>
    <label>> 1. ¿Cómo se llamaba tu primera mascota?</label>
    <input type="password" id="s_a1" placeholder="Ej: Toby">
    <label>> 2. ¿Año de tu película favorita?</label>
    <input type="password" id="s_a2" placeholder="Ej: 1999">
    <label>> 3. ¿Nombre de la calle de tu infancia?</label>
    <input type="password" id="s_a3" placeholder="Ej: Gran Via">

    <h3 style="border-bottom: 1px solid #f00; padding-bottom: 5px; margin-top: 30px;">> FASE 2: CLAVES DE PURGA (Para borrar registros)</h3>
    <p style="font-size: 12px;">Se te pedirán 3 de estas al azar antes de permitirte borrar una contraseña.</p>
    <label>> 1. ¿Color favorito?</label>
    <input type="password" id="s_d1">
    <label>> 2. ¿Marca de tu primer coche?</label>
    <input type="password" id="s_d2">
    <label>> 3. ¿Mejor banda de rock?</label>
    <input type="password" id="s_d3">
    <label>> 4. ¿Ciudad de tu luna de miel?</label>
    <input type="password" id="s_d4">
    <label>> 5. ¿Bebida favorita?</label>
    <input type="password" id="s_d5">
    
    <h3 style="border-bottom: 1px solid #f00; padding-bottom: 5px; margin-top: 30px;">> FASE 3: CAMUFLAJE DE RED (Punto de Acceso)</h3>
    <p style="font-size: 12px;">Ponle un nombre aburrido (ej: Impresora_HP) para que nadie sospeche. Mínimo 8 caracteres para la clave.</p>
    <label>> Nombre de la red (SSID):</label>
    <input type="text" id="s_ssid" value="FortKnox_D1" style="color:#0f0; border-color:#0f0;">
    <label>> Contraseña del WiFi:</label>
    <input type="text" id="s_wifipass" value="123456789" style="color:#0f0; border-color:#0f0;">
    
    <button class="btn-red" onclick="saveInitialSetup()" style="width: 100%; margin-top: 20px; font-size: 18px;">[ ENTENDIDO. INICIALIZAR BÓVEDA DEFINITIVAMENTE ]</button>
</div>

<!-- PANTALLA 3: LOGIN HABITUAL -->
<div class="container hidden" id="loginScreen">
    <div style="text-align: center;">
        <div class="box-title">
            <div style="font-size: 24px; font-weight: bold;">> IDENTIFICACIÓN REQUERIDA_</div>
        </div>
    </div>
    
    <p style="text-align: center;">Introduce credenciales fundacionales para desencriptar la bóveda.</p>
    
    <label>> 1. ¿Primera mascota?</label>
    <input type="password" id="q1">
    
    <label>> 2. ¿Año película favorita?</label>
    <input type="password" id="q2">
    
    <label>> 3. ¿Calle de la infancia?</label>
    <input type="password" id="q3">
    
    <button id="btnUnlock" onclick="attemptUnlock()" style="width: 100%;">[ INICIAR DESENCRIPTADO ]</button>
    <div id="loading">> PROCESANDO CLAVES CRIPTOGRÁFICAS...</div>
    <p id="loginError" style="color:#f00; display:none; text-align: center; border: 1px solid #f00; padding: 10px; margin-top: 10px;">[!] ACCESO DENEGADO. DATOS CORRUPTOS.</p>
    
    <div class="footer">⚙️ SEC-VAULT v2.1 | Diseñado por jmcaamnog ⚙️</div>
</div>

<!-- PANTALLA 4: LA BÓVEDA -->
<div class="container hidden" id="vaultScreen">
    <h1>> BASE DE DATOS ACTIVA_</h1>
    
    <h2>> NUEVO REGISTRO</h2>
    <div style="display: flex; gap: 10px; flex-wrap: wrap; margin-bottom: 15px;">
        <input type="text" id="prog" placeholder="Programa / Web" style="flex: 1; min-width: 150px; margin: 0;">
        <input type="text" id="user" placeholder="Usuario" style="flex: 1; min-width: 150px; margin: 0;">
        
        <div style="display: flex; align-items: center; flex: 1; min-width: 150px;">
            <input type="password" id="pass" placeholder="Contraseña" style="flex: 1; margin: 0;">
            <button onclick="toggleViewPass()" class="action-btn" title="Ver Clave" style="color:#0f0;">👁️</button>
            <button onclick="generatePass()" class="action-btn" title="Generar Contraseña Segura">🎲</button>
        </div>
        
        <input type="text" id="url" placeholder="Dirección Web" style="flex: 1; min-width: 150px; margin: 0;">
    </div>
    <button onclick="addEntry()">[ GUARDAR Y BLINDAR ]</button>

    <h2 style="margin-top: 30px;">> CREDENCIALES ALMACENADAS</h2>
    
    <div style="display: flex; justify-content: space-between; margin-bottom: 15px; flex-wrap: wrap; gap: 10px; background: #050505; padding: 10px; border: 1px solid #0f0;">
        <input type="text" id="searchInput" placeholder="> Buscar..." onkeyup="filterTable()" style="width: 100%; max-width: 250px; margin: 0;">
        <div style="display: flex; gap: 10px; align-items: center; flex-wrap: wrap;">
            <input type="file" id="csvFile" accept=".csv" style="margin: 0; padding: 5px; width: 200px;">
            <button onclick="importCSV()" style="margin: 0;">[ INYECTAR CSV ]</button>
            <button onclick="exportBackup()" style="margin: 0; border-color: #bb86fc; color: #bb86fc;">[ EXTRAER BACKUP ]</button>
        </div>
    </div>

    <div style="overflow-x: auto;">
        <table id="dataTable">
            <thead>
                <tr>
                    <th>PROGRAMA</th>
                    <th>USUARIO</th>
                    <th>CONTRASEÑA</th>
                    <th>URL</th>
                    <th>ACCIÓN</th>
                </tr>
            </thead>
            <tbody></tbody>
        </table>
    </div>
    <div class="footer">⚙️ SEC-VAULT v2.1 | Diseñado por jmcaamnog ⚙️</div>
</div>

<!-- Modal Borrado -->
<div class="modal" id="deleteModal">
    <div class="modal-content">
        <h2 style="color: #f00; border-bottom: 1px dashed #f00;">[!] ADVERTENCIA CRÍTICA</h2>
        <p style="color: #f00;">Verificación de identidad requerida para purgar registro.</p>
        <div id="randomQuestionsContainer"></div>
        <div style="margin-top: 20px; display: flex; justify-content: space-between;">
            <button onclick="closeModal()">[ ABORTAR ]</button>
            <button class="btn-red" onclick="confirmDelete()">[ PURGAR ]</button>
        </div>
        <p id="deleteError" style="color:#f00; display:none;">[!] VERIFICACIÓN FALLIDA.</p>
    </div>
</div>

<div id="toast">Notificación</div>

<script>
    // appData contiene la configuración de borrado Y las contraseñas
    let appData = { config: [], entries: [] };
    let aesKey = null; 
    let rowToDelete = null;
    let rawEncryptedFromServer = ""; // Lo guardamos al entrar para no hacer 2 peticiones

    const deleteQuestionsText = [
        "> ¿Color favorito?", 
        "> ¿Marca de tu primer coche?", 
        "> ¿Mejor banda de rock?", 
        "> ¿Ciudad de tu luna de miel?", 
        "> ¿Bebida favorita?"
    ];
    let currentDeleteIndices = [];

    // --- FLUJO DE ARRANQUE ---
    async function checkSystemState() {
        document.getElementById('welcomeLoading').style.display = 'block';
        try {
            const res = await fetch('/api/load');
            rawEncryptedFromServer = await res.text();
            
            document.getElementById('welcomeScreen').classList.add('hidden');
            
            if (rawEncryptedFromServer.trim() === "") {
                // PRIMERA VEZ
                document.getElementById('setupScreen').classList.remove('hidden');
            } else {
                // USUARIO EXISTENTE
                document.getElementById('loginScreen').classList.remove('hidden');
            }
        } catch(e) {
            alert("Error conectando con el hardware. Revisa el WiFi.");
            document.getElementById('welcomeLoading').style.display = 'none';
        }
    }

    // --- SETUP INICIAL ---
    async function saveInitialSetup() {
        const a1 = document.getElementById('s_a1').value;
        const a2 = document.getElementById('s_a2').value;
        const a3 = document.getElementById('s_a3').value;
        
        const d1 = document.getElementById('s_d1').value;
        const d2 = document.getElementById('s_d2').value;
        const d3 = document.getElementById('s_d3').value;
        const d4 = document.getElementById('s_d4').value;
        const d5 = document.getElementById('s_d5').value;

        const newSsid = document.getElementById('s_ssid').value.trim();
        const newWifiPass = document.getElementById('s_wifipass').value.trim();

        if(!a1 || !a2 || !a3 || !d1 || !d2 || !d3 || !d4 || !d5) {
            alert("¡NO PUEDES DEJAR CAMPOS VACÍOS! Es por tu seguridad.");
            return;
        }
        
        if(newWifiPass.length < 8) {
            alert("La clave del WiFi debe tener al menos 8 caracteres (regla de los routers).");
            return;
        }

        // Crear clave maestra
        aesKey = deriveKey(a1, a2, a3);
        
        // Guardar configuración de borrado (normalizada)
        appData.config = [
            d1.toLowerCase().trim(),
            d2.toLowerCase().trim(),
            d3.toLowerCase().trim(),
            d4.toLowerCase().trim(),
            d5.toLowerCase().trim()
        ];
        appData.entries = [];

        // 1. Guardar primero la bóveda
        await saveToESP();
        
        // 2. Enviar la nueva configuración WiFi
        await fetch('/api/wifi', {
            method: 'POST',
            headers: { 'Content-Type': 'application/x-www-form-urlencoded' },
            body: 'ssid=' + encodeURIComponent(newSsid) + '&pass=' + encodeURIComponent(newWifiPass)
        });
        
        // 3. Avisar al usuario del reinicio inminente
        alert(`¡BÓVEDA INICIALIZADA Y BLINDADA! ⚠️\n\nEl sistema se va a reiniciar ahora mismo para aplicar el nuevo nombre de red WiFi: '${newSsid}'.\n\nTu conexión actual se cortará. Ve a los ajustes de WiFi de tu móvil, busca la nueva red, conéctate y recarga esta página.`);
        
        document.getElementById('setupScreen').innerHTML = "<div class='box-title' style='color:#0f0; margin-top:50px;'><div style='font-size:24px; font-weight:bold;'>> REINICIANDO SISTEMA...</div><div style='font-size:14px; margin-top:10px;'>Busca la nueva red WiFi para continuar.</div></div>";
    }

    // --- SISTEMA AUTO-BLOQUEO POR INACTIVIDAD (3 Minutos) ---
    let idleTime = 0;
    setInterval(() => {
        if (!document.getElementById('vaultScreen').classList.contains('hidden')) {
            idleTime++;
            if (idleTime > 180) lockVault(); 
        }
    }, 1000);

    function resetTimer() { idleTime = 0; }
    document.onmousemove = resetTimer; document.onkeypress = resetTimer;
    document.onclick = resetTimer; document.ontouchstart = resetTimer;

    function lockVault() {
        appData = { config: [], entries: [] };
        aesKey = null;
        document.getElementById('vaultScreen').classList.add('hidden');
        document.getElementById('loginScreen').classList.remove('hidden');
        document.getElementById('q1').value = ''; document.getElementById('q2').value = ''; document.getElementById('q3').value = '';
        showToast('🔒 SESIÓN CERRADA POR INACTIVIDAD');
    }

    // --- UTILIDADES ---
    function showToast(msg) {
        const t = document.getElementById('toast');
        t.innerText = msg; t.className = 'show';
        setTimeout(() => { t.className = t.className.replace("show", ""); }, 3000);
    }

    function copyText(encodedText) {
        const text = decodeURIComponent(encodedText);
        const el = document.createElement('textarea');
        el.value = text; document.body.appendChild(el); el.select();
        document.execCommand('copy'); document.body.removeChild(el);
        showToast('📋 COPIADO AL PORTAPAPELES');
        resetTimer();
    }
    
    function toggleViewPass() {
        const p = document.getElementById('pass');
        p.type = p.type === 'password' ? 'text' : 'password';
    }

    function generatePass() {
        const chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789!@#$%^&*()_+";
        let pass = "";
        for(let i=0; i<16; i++) pass += chars.charAt(Math.floor(Math.random() * chars.length));
        const p = document.getElementById('pass');
        p.type = 'text'; // Mostrar para que la vea
        p.value = pass;
        showToast('🎲 CLAVE SEGURA GENERADA');
        resetTimer();
    }

    // --- MOTOR CRIPTOGRÁFICO NATIVO (RC4 Modificado) ---
    function cryptRC4(key, text) {
        let s = [], j = 0, x, res = '';
        for (let i = 0; i < 256; i++) s[i] = i;
        for (let i = 0; i < 256; i++) {
            j = (j + s[i] + key.charCodeAt(i % key.length)) % 256;
            x = s[i]; s[i] = s[j]; s[j] = x;
        }
        let i = 0; j = 0;
        for (let y = 0; y < text.length; y++) {
            i = (i + 1) % 256;
            j = (j + s[i]) % 256;
            x = s[i]; s[i] = s[j]; s[j] = x;
            res += String.fromCharCode(text.charCodeAt(y) ^ s[(s[i] + s[j]) % 256]);
        }
        return res;
    }

    function deriveKey(q1, q2, q3) {
        return q1.trim().toLowerCase() + "|" + q2.trim().toLowerCase() + "|" + q3.trim().toLowerCase();
    }

    async function encryptData(dataObj) {
        const salt = Math.random().toString(36).substring(2, 10);
        const saltedKey = aesKey + salt;
        const jsonStr = encodeURIComponent(JSON.stringify(dataObj));
        const encryptedStr = cryptRC4(saltedKey, jsonStr);
        return JSON.stringify({ s: salt, d: btoa(encryptedStr) });
    }

    async function decryptData(payloadStr) {
        try {
            const parsed = JSON.parse(payloadStr);
            const saltedKey = aesKey + parsed.s;
            const rawStr = atob(parsed.d);
            const decryptedStr = cryptRC4(saltedKey, rawStr);
            return JSON.parse(decodeURIComponent(decryptedStr));
        } catch(e) { return null; }
    }

    // --- LÓGICA PRINCIPAL ---
    async function attemptUnlock() {
        const q1 = document.getElementById('q1').value;
        const q2 = document.getElementById('q2').value;
        const q3 = document.getElementById('q3').value;
        
        if(!q1 || !q2 || !q3) return;

        document.getElementById('loading').style.display = 'block';
        document.getElementById('btnUnlock').disabled = true;
        document.getElementById('loginError').style.display = 'none';

        try {
            aesKey = deriveKey(q1, q2, q3);
            const decryptedObj = await decryptData(rawEncryptedFromServer);
            
            if (decryptedObj) {
                // Control de retrocompatibilidad (por si tiene el formato viejo de array)
                if (Array.isArray(decryptedObj)) {
                    appData.entries = decryptedObj;
                    // Claves por defecto para no romper bóvedas antiguas
                    appData.config = ['azul', 'seat', 'queen', 'paris', 'cafe'];
                    showToast('⚠️ MODO COMPATIBILIDAD. CLAVES DE BORRADO POR DEFECTO.');
                } else {
                    appData = decryptedObj;
                }
                
                showVault();
                showToast('🔓 DESENCRIPTADO EXITOSO');
            } else {
                throw new Error("Respuestas incorrectas.");
            }
        } catch (error) {
            document.getElementById('loginError').style.display = 'block';
        }
        
        document.getElementById('loading').style.display = 'none';
        document.getElementById('btnUnlock').disabled = false;
        resetTimer();
    }

    function showVault() {
        document.getElementById('loginScreen').classList.add('hidden');
        document.getElementById('vaultScreen').classList.remove('hidden');
        renderTable();
        resetTimer();
    }

    function renderTable() {
        const tbody = document.querySelector('#dataTable tbody');
        tbody.innerHTML = '';
        appData.entries.forEach(row => {
            const tr = document.createElement('tr');
            const encUser = encodeURIComponent(row.user);
            const encPass = encodeURIComponent(row.pass);
            
            tr.innerHTML = `
                <td>${row.prog}</td>
                <td>${row.user} <button class="action-btn" onclick="copyText('${encUser}')" title="Copiar Usuario">📋</button></td>
                <td><span style="opacity:0.3;">********</span> <button class="action-btn" onclick="copyText('${encPass}')" title="Copiar Contraseña">📋</button></td>
                <td><a href="http://${row.url}" target="_blank" style="color:#0f0;">${row.url}</a></td>
                <td><button class="btn-red" onclick="requestDelete(${row.id})">[ X ]</button></td>
            `;
            tbody.appendChild(tr);
        });
        if(document.getElementById('searchInput')) filterTable();
    }
    
    function filterTable() {
        const term = document.getElementById('searchInput').value.toLowerCase();
        const rows = document.querySelectorAll('#dataTable tbody tr');
        rows.forEach(row => {
            const text = row.innerText.toLowerCase();
            row.style.display = text.includes(term) ? '' : 'none';
        });
        resetTimer();
    }

    function importCSV() {
        const fileInput = document.getElementById('csvFile');
        const file = fileInput.files[0];
        if (!file) { showToast("[!] SELECCIONA UN CSV"); return; }

        const reader = new FileReader();
        reader.onload = async function(e) {
            const text = e.target.result;
            const lines = text.split('\n');
            let added = 0;
            
            lines.forEach(line => {
                const parts = line.split(',');
                if (parts.length >= 3 && parts[0].trim() !== '') {
                    appData.entries.push({
                        id: Date.now() + Math.random(),
                        prog: parts[0].trim(),
                        user: parts[1].trim(),
                        pass: parts[2].trim(),
                        url: parts[3] ? parts[3].trim() : ''
                    });
                    added++;
                }
            });
            
            if (added > 0) {
                renderTable();
                await saveToESP();
                fileInput.value = ''; 
                showToast(`🚀 ${added} REGISTROS INYECTADOS`);
            } else {
                showToast("[!] FORMATO CSV INVÁLIDO");
            }
        };
        reader.readAsText(file);
        resetTimer();
    }

    async function exportBackup() {
        try {
            const res = await fetch('/api/load');
            const text = await res.text();
            if(!text) { showToast('[!] LA BÓVEDA ESTÁ VACÍA'); return; }
            
            const blob = new Blob([text], { type: 'text/plain' });
            const a = document.createElement('a');
            a.href = URL.createObjectURL(blob);
            a.download = 'sec_vault_backup.bak';
            document.body.appendChild(a);
            a.click();
            document.body.removeChild(a);
            showToast('💾 BACKUP CIFRADO EXTRAÍDO');
        } catch(e) {
            showToast('[!] ERROR AL EXTRAER BACKUP');
        }
        resetTimer();
    }

    async function saveToESP() {
        const payload = await encryptData(appData);
        await fetch('/api/save', {
            method: 'POST',
            headers: { 'Content-Type': 'application/x-www-form-urlencoded' },
            body: 'payload=' + encodeURIComponent(payload)
        });
        // Actualizamos la variable local para que exportar funcione sin recargar
        rawEncryptedFromServer = payload; 
    }

    async function addEntry() {
        const prog = document.getElementById('prog').value;
        const user = document.getElementById('user').value;
        const pass = document.getElementById('pass').value;
        const url = document.getElementById('url').value;
        
        if(!prog || !user || !pass) { showToast("[!] RELLENA LOS DATOS CLAVE"); return; }
        
        appData.entries.push({ id: Date.now(), prog, user, pass, url });
        renderTable();
        
        document.getElementById('prog').value = ''; document.getElementById('user').value = '';
        document.getElementById('pass').value = ''; document.getElementById('url').value = '';
        document.getElementById('pass').type = 'password';
        
        await saveToESP();
        showToast("🛡️ DATOS BLINDADOS EN HARDWARE");
        resetTimer();
    }

    function requestDelete(id) {
        rowToDelete = id;
        document.getElementById('deleteError').style.display = 'none';
        
        // Seleccionar 3 índices aleatorios de los 5 posibles (0 a 4)
        let indices = [0, 1, 2, 3, 4].sort(() => 0.5 - Math.random()).slice(0, 3);
        currentDeleteIndices = indices;

        const container = document.getElementById('randomQuestionsContainer');
        container.innerHTML = '';
        indices.forEach((idx, i) => {
            container.innerHTML += `<label style="display:block;margin-top:10px;">${deleteQuestionsText[idx]}</label><input type="password" id="delQ${i}" style="border-color:#f00; color:#f00; outline:none; background:#050000; font-family:'Courier New'; padding:8px; width:100%; box-sizing:border-box;">`;
        });

        document.getElementById('deleteModal').style.display = 'flex';
        resetTimer();
    }

    async function confirmDelete() {
        let valid = true;
        
        // Comprobar las 3 respuestas contra appData.config usando los índices guardados
        for(let i=0; i<3; i++) {
            const answerUser = document.getElementById(`delQ${i}`).value.toLowerCase().trim();
            const realAnswer = appData.config[currentDeleteIndices[i]];
            if(answerUser !== realAnswer) {
                valid = false;
            }
        }

        if(valid) {
            appData.entries = appData.entries.filter(r => r.id !== rowToDelete);
            renderTable();
            closeModal();
            await saveToESP();
            showToast("🗑️ REGISTRO PURGADO");
        } else {
            document.getElementById('deleteError').style.display = 'block';
        }
        resetTimer();
    }

    function closeModal() {
        document.getElementById('deleteModal').style.display = 'none';
        rowToDelete = null;
        resetTimer();
    }
</script>
</body>
</html>
)rawliteral";

// =======================================================================
// LÓGICA DEL ESP8266
// =======================================================================

void setup() {
  Serial.begin(115200);
  Serial.println("\n--- Arrancando Bóveda FortKnox ---");

  if (!LittleFS.begin()) {
    Serial.println("Error montando memoria Flash (LittleFS)");
    return;
  }
  Serial.println("Memoria lista.");

  // Leemos si hay una configuración WiFi guardada
  if (LittleFS.exists("/wifi.txt")) {
    File f = LittleFS.open("/wifi.txt", "r");
    currentSSID = f.readStringUntil('\n');
    currentSSID.trim();
    currentWifiPass = f.readStringUntil('\n');
    currentWifiPass.trim();
    f.close();
    Serial.println("Configuración WiFi cargada desde memoria.");
  }

  WiFi.softAP(currentSSID.c_str(), currentWifiPass.c_str());
  Serial.print("Punto de acceso creado (");
  Serial.print(currentSSID);
  Serial.print("). IP: ");
  Serial.println(WiFi.softAPIP());

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    AsyncWebServerResponse *response =
        request->beginResponse_P(200, "text/html", index_html);
    response->addHeader("Cache-Control", "no-cache, no-store, must-revalidate");
    response->addHeader("Pragma", "no-cache");
    response->addHeader("Expires", "-1");
    request->send(response);
  });

  server.on("/api/load", HTTP_GET, [](AsyncWebServerRequest *request) {
    if (!LittleFS.exists("/boveda.enc")) {
      request->send(200, "text/plain", "");
      return;
    }
    request->send(LittleFS, "/boveda.enc", "text/plain");
  });

  server.on("/api/save", HTTP_POST, [](AsyncWebServerRequest *request) {
    if (request->hasParam("payload", true)) {
      String encryptedPayload = request->getParam("payload", true)->value();

      File f = LittleFS.open("/boveda.enc", "w");
      if (f) {
        f.print(encryptedPayload);
        f.close();
        request->send(200, "text/plain", "Guardado ok");
        Serial.println("Nuevos datos guardados.");
      } else {
        request->send(500, "text/plain", "Error de hardware al guardar");
      }
    } else {
      request->send(400, "text/plain", "Petición vacía");
    }
  });

  // Nuevo endpoint para guardar la configuración del WiFi y forzar reinicio
  server.on("/api/wifi", HTTP_POST, [](AsyncWebServerRequest *request) {
    if (request->hasParam("ssid", true) && request->hasParam("pass", true)) {
      String newSsid = request->getParam("ssid", true)->value();
      String newPass = request->getParam("pass", true)->value();

      File f = LittleFS.open("/wifi.txt", "w");
      if (f) {
        f.println(newSsid);
        f.println(newPass);
        f.close();
        request->send(200, "text/plain", "WiFi config guardada");
        Serial.println(
            "Nuevas credenciales WiFi recibidas. Preparando reinicio...");
        needsReboot = true;
        rebootTimer = millis();
      } else {
        request->send(500, "text/plain", "Error guardando wifi");
      }
    } else {
      request->send(400, "text/plain", "Faltan parametros");
    }
  });

  server.begin();
  Serial.println("Servidor Web corriendo y a la espera...");
}

void loop() {
  // Comprobamos si nos han pedido reiniciar (con un retraso de 2 segundos para
  // que le dé tiempo a enviar el OK al navegador)
  if (needsReboot && millis() - rebootTimer > 2000) {
    Serial.println("Reiniciando ESP8266 para aplicar nuevo WiFi...");
    ESP.restart();
  }
}