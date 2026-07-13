# SEC-VAULT - Bóveda de Contraseñas Offline (D1 Mini) 🔒🛡️

![C++](https://img.shields.io/badge/C++-Arduino-00979D.svg)
![Hardware](https://img.shields.io/badge/Hardware-ESP8266-black.svg)
![Security](https://img.shields.io/badge/Security-AES%2FRC4-red.svg)
![License](https://img.shields.io/badge/License-MIT-green.svg)

(Arquitecto Técnico_JMC) **Sec-Vault** es un gestor de contraseñas de hardware *Air-Gapped* construido sobre un microcontrolador ESP8266 (D1 Mini). Diseñado para operar en un entorno de "Conocimiento Cero" (Zero-Knowledge), el dispositivo funciona como un Punto de Acceso (AP) WiFi totalmente aislado de internet. Una bóveda física de bolsillo con estética retro-cyberpunk para los verdaderos paranoicos de la privacidad.

## 🚀 La Filosofía de la Herramienta (Aislamiento Total)

¿Por qué confiar tus contraseñas a la nube o a servidores de terceros? Esta herramienta nace de una premisa radical: **si no hay internet, no hay hackeo remoto**. Eres tú y tu hardware. El D1 Mini emite su propia red WiFi camuflada. Te conectas, desencriptas tus datos localmente en la memoria RAM de tu móvil/PC, y el hardware solo se encarga de guardar un bloque de texto cifrado incomprensible. Si pierdes el aparato, quien lo encuentre solo verá basura digital.

## 🧠 Características del Software (v2.1)

* **Arquitectura Zero-Knowledge:** El ESP8266 jamás conoce tus contraseñas ni tus claves maestras. Toda la criptografía (RC4 modificado con Salting dinámico) ocurre del lado del cliente mediante Javascript puro en el navegador.
* **Estética Nostromo / Terminal Retro:** Interfaz de usuario inmersiva en fósforo verde. Respuestas rápidas, notificaciones emergentes (Toasts) y modo de auto-bloqueo por inactividad a los 3 minutos.
* **Protocolo de Purga Segura:** Borrar un registro exige superar un test de seguridad aleatorio (configurado la primera vez que enciendes el dispositivo) para evitar accidentes o accesos no autorizados al equipo desbloqueado.
* **Modo Camuflaje (Stealth WiFi):** Posibilidad de renombrar la red WiFi a nombres mundanos (ej: `Impresora_HP_Piso2`) para no levantar sospechas.
* **Gestión Ágil:** Importación masiva de credenciales mediante archivos `.csv`, buscador integrado en tiempo real, generador de contraseñas robustas y extracción de Backups cifrados (`.bak`).

## 📂 Estructura del Repositorio

* 📁 **`CODE/`**: Contiene el código fuente completo. El archivo `.ino` empaqueta tanto el backend en C++ para el ESP8266 como el frontend (HTML/CSS/JS) comprimido en la memoria PROGMEM para un despliegue ultra-rápido.

## ⚙️ Requisitos y Despliegue (Cómo flashear tu bóveda)

Necesitarás una placa basada en el chip **ESP8266** (recomendado Wemos D1 Mini por su tamaño) y el IDE de Arduino.

1. Clona el repositorio y abre el archivo `.ino` situado en la carpeta `CODE/` usando Arduino IDE.
2. Instala las siguientes librerías desde el Gestor de Librerías del IDE o mediante los `.zip` de GitHub:
   * `ESP8266WiFi` (Nativa del core de ESP8266)
   * `LittleFS` (Nativa del core de ESP8266)
   * `ESPAsyncTCP`
   * `ESPAsyncWebServer`
3. Conecta tu placa por USB, selecciona el puerto COM adecuado y asegúrate de elegir el esquema de partición que deje espacio para **LittleFS** (ej. *FS: 1MB* o superior).
4. Dale a **Subir**. 

*Nota: La primera vez que te conectes a la red por defecto, el sistema te guiará por el protocolo de inicialización en rojo para definir tus claves inmutables.*

## 👨‍💻 Autor

Jose Manuel Caamaño González | Arquitecto Técnico & BIM Manager.
Digital Product Lead | ConTech & Digital Twin SaaS | BIM, Energy Modeling & Sustainability | Data Analytics (SQL, Power BI)

Hecho con código y café desde A Coruña. ☕

Jose Manuel Caamaño González | [LinkedIn](https://www.linkedin.com/in/jmcaamanog/)