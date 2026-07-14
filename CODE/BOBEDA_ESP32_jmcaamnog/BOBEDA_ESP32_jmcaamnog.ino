/*
       ██╗███╗   ███╗ ██████╗
       ██║████╗ ████║██╔════╝
       ██║██╔████╔██║██║
  ██   ██║██║╚██╔╝██║██║    
  █████╔╝ ██║ ╚═╝ ██║╚██████╗
  ╚════╝  ╚═╝     ╚═╝ ╚═════╝

  SEC-VAULT v3 - ESP8266 / Wemos D1 mini
  Seguridad: cifrado autenticado AES-GCM y PBKDF2 se realizan en el navegador.
*/
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <LittleFS.h>

static const size_t MAX_PAYLOAD = 46000;
String ssid = "SEC-VAULT-SETUP";
String wifiPass = "cambia-esto-ya";
bool rebootPending = false;
uint32_t rebootAt = 0;
AsyncWebServer server(80);

bool validWifi(const String &name, const String &pass) {
  return name.length() >= 1 && name.length() <= 32 && pass.length() >= 8 && pass.length() <= 63;
}

bool writeVault(const String &payload) {
  if (payload.length() == 0 || payload.length() > MAX_PAYLOAD) return false;
  File temp = LittleFS.open("/vault.tmp", "w");
  if (!temp) return false;
  const size_t written = temp.print(payload);
  temp.close();
  if (written != payload.length()) { LittleFS.remove("/vault.tmp"); return false; }
  // Conserva una generaciÃƒÂ³n anterior para recuperarse de un corte durante el guardado.
  LittleFS.remove("/vault.prev");
  if (LittleFS.exists("/vault.enc") && !LittleFS.rename("/vault.enc", "/vault.prev")) {
    LittleFS.remove("/vault.tmp"); return false;
  }
  if (!LittleFS.rename("/vault.tmp", "/vault.enc")) {
    if (LittleFS.exists("/vault.prev")) LittleFS.rename("/vault.prev", "/vault.enc");
    return false;
  }
  return true;
}

#include "index_v3.h"

void setup() {
  Serial.begin(115200);
  if (!LittleFS.begin()) { Serial.println("LittleFS no disponible"); return; }
  if (LittleFS.exists("/wifi.txt")) {
    File f = LittleFS.open("/wifi.txt", "r");
    ssid = f.readStringUntil('\n'); ssid.trim(); wifiPass = f.readStringUntil('\n'); wifiPass.trim(); f.close();
    if (!validWifi(ssid, wifiPass)) { ssid = "SEC-VAULT-SETUP"; wifiPass = "cambia-esto-ya"; }
  }
  WiFi.mode(WIFI_AP); WiFi.softAP(ssid.c_str(), wifiPass.c_str());
  
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *r) { 
    auto *res=r->beginResponse_P(200,"text/html; charset=utf-8",INDEX_HTML);
    res->addHeader("Cache-Control","no-store");
    res->addHeader("X-Content-Type-Options","nosniff");
    res->addHeader("Content-Security-Policy","default-src 'self'; style-src 'unsafe-inline'; script-src 'unsafe-inline'");
    r->send(res); 
  });
  
  server.on("/api/load", HTTP_GET, [](AsyncWebServerRequest *r) { 
    if (!LittleFS.exists("/vault.enc")) { r->send(200,"application/json",""); return; } 
    r->send(LittleFS,"/vault.enc","application/json",false); 
  });
  
  server.on("/api/save", HTTP_POST, [](AsyncWebServerRequest *r) { 
    if(!r->hasParam("payload",true)){r->send(400,"text/plain","Falta payload");return;} 
    String p=r->getParam("payload",true)->value(); 
    if(p.length()>MAX_PAYLOAD){r->send(413,"text/plain","Payload demasiado grande");return;} 
    if(writeVault(p))r->send(200,"text/plain","OK");
    else r->send(500,"text/plain","No se pudo guardar"); 
  });
  
  server.on("/api/wifi", HTTP_POST, [](AsyncWebServerRequest *r) { 
    if(!r->hasParam("ssid",true)||!r->hasParam("pass",true)){r->send(400,"text/plain","Faltan parámetros");return;} 
    String n=r->getParam("ssid",true)->value(),p=r->getParam("pass",true)->value(); 
    if(!validWifi(n,p)){r->send(400,"text/plain","WiFi inválido");return;} 
    File f=LittleFS.open("/wifi.tmp","w");
    if(!f){r->send(500,"text/plain","No se pudo guardar");return;}
    f.println(n);f.println(p);f.close();
    LittleFS.remove("/wifi.txt");
    if(!LittleFS.rename("/wifi.tmp","/wifi.txt")){r->send(500,"text/plain","No se pudo aplicar");return;}
    r->send(200,"text/plain","OK");
    rebootPending=true;
    rebootAt=millis(); 
  });
  
  // NUEVA RUTA PARA TELEMETRÍA (IP Y USUARIOS)
  server.on("/api/status", HTTP_GET, [](AsyncWebServerRequest *r) {
    String json = "{";
    json += "\"ip\":\"" + WiFi.softAPIP().toString() + "\",";
    json += "\"clients\":" + String(wifi_softap_get_station_num());
    json += "}";
    r->send(200, "application/json", json);
  });
  
  server.begin();
}

void loop(){ 
  if(rebootPending && millis()-rebootAt>1500) ESP.restart(); 
}