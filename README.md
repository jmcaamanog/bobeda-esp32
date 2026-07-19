# SEC-VAULT - Bóveda de Contraseñas Offline (D1 Mini) 🔒🛡️

| 🏗️ Perfil & ConTech | 📈 Repositorio & Enlaces |
| :--- | :--- |
| ![Profesión](https://img.shields.io/badge/Profesi%C3%B3n-Arquitectos%20T%C3%A9cnicos-2e7d32?logo=micro%3Abit&logoColor=white&style=plastic) <br> ![Role](https://img.shields.io/badge/Role-BIM%20%26%20ConTech-007ACC?logo=bim360&style=plastic) <br> ![Location](https://img.shields.io/badge/Location-A%20Coru%C3%B1a%20%F0%9F%8C%8A-005B94?logo=lighthouse&logoColor=white&style=plastic) <br> ![Maker](https://img.shields.io/badge/Maker-Software%20+%20Hardware-red?logo=makerbot&style=plastic) | ![Hardware](https://img.shields.io/badge/Hardware-ESP8266-E7352C&style=plastic) <br> ![Windows](https://img.shields.io/badge/Platform-Windows-0078D6?logo=windows&style=plastic) <br> ![Language](https://img.shields.io/badge/Language-C%2B%2B-00599C?logo=c%2B%2B&logoColor=white&style=plastic) <br> ![Stars](https://img.shields.io/github/stars/jmcaamanog/bobeda-esp32?style=plastic&color=yellow&logo=github) <br> ![License](https://img.shields.io/github/license/jmcaamanog/bobeda-esp32?style=plastic&color=green) |

(Arquitecto Técnico_JMC) **Sec-Vault** es un gestor de contraseñas de hardware *Air-Gapped* construido sobre un microcontrolador ESP8266 (D1 Mini). Diseñado para operar en un entorno de "Conocimiento Cero" (Zero-Knowledge), el dispositivo funciona como un Punto de Acceso (AP) WiFi totalmente aislado de internet. Una bóveda física de bolsillo con estética retro-cyberpunk para los verdaderos paranoicos de la privacidad.

## 🚀 La Filosofía de la Herramienta (Aislamiento Total)

¿Por qué confiar tus contraseñas a la nube o a servidores de terceros? Esta herramienta nace de una premisa radical: **si no hay internet, no hay hackeo remoto**. Eres tú y tu hardware. El D1 Mini emite su propia red WiFi camuflada. Te conectas, desencriptas tus datos localmente en la memoria RAM de tu móvil o PC, y el hardware solo se encarga de guardar un bloque de texto cifrado incomprensible. Si pierdes el aparato, quien lo encuentre solo verá basura digital.

## 🧠 Características del Software (v3.5)

* **Arquitectura Zero-Knowledge & Criptografía Local:** El ESP8266 jamás conoce tus contraseñas ni tus claves maestras. Toda la criptografía (RC4 modificado con Salting dinámico) ocurre en el cliente mediante Javascript puro en el navegador, superando de forma nativa las restricciones de la API de Web Crypto offline en conexiones HTTP sin HTTPS.
* **🚨 Modo Pánico de Autodestrucción:** Permite configurar una contraseña falsa de "autodestrucción" durante la inicialización. Al introducirla en la pantalla de desbloqueo, el sistema purga y formatea el almacenamiento (`LittleFS`) instantáneamente sin dejar rastro de los datos.
* **📊 Barra de Telemetría en Tiempo Real:** Monitoriza el estado de la bóveda mediante indicadores visuales de candado abierto/cerrado, visualización de la IP de conexión y contador dinámico de usuarios (`USERS_XX`) conectados simultáneamente a la red.
* **⏱️ Temporizador de Exposición Activo:** Cuenta atrás de autobloqueo por inactividad visible directamente en la cabecera. Si pasas 3 minutos sin interacción, la memoria se limpia y el sistema te expulsa a la pantalla de identificación de forma automática.
* **📝 Notas Secretas Desplegables:** Soporte para añadir bloques de notas multilínea (semillas de criptomonedas, códigos PIN/PUK o datos sensibles) integrados de forma compacta y ocultos tras un menú desplegable en la tabla.
* **🏷️ Filtro por Etiquetas (Tags):** Posibilidad de categorizar tus credenciales utilizando etiquetas separadas por comas. El buscador en tiempo real permite filtrar tanto por nombre de servicio como por etiqueta.
* **⚙️ Generador de Claves "A la Carta":** Crea contraseñas robustas de longitud regulable (hasta 64 caracteres) seleccionando explícitamente los conjuntos de caracteres requeridos (mayúsculas, minúsculas, números y símbolos).
* **Camuflaje WiFi & Gestión Ágil:** Cambia de forma dinámica el nombre de la red SSID y su clave en el arranque para simular un dispositivo inofensivo (ej: `Impresora_HP_Piso2`). Permite importación masiva mediante archivos `.csv`, backup cifrado exportable (`.bak`) y un control de fuerza de clave maestra visual.

## 🔌 Hardware y Esquema de Conexiones (Modo Búnker Autónomo)

Para que el dispositivo sea 100% portátil, se recomienda integrarle una batería LiPo mediante un módulo de carga TP4056 y añadirle un pequeño display OLED (0.96" I2C) para monitorizar el estado físico.

### 🖥️ Conexión de Pantalla OLED (0.96" I2C)

La pantalla se comunica por el bus I2C. Los pines D1 y D2 del Wemos D1 Mini son los estándar para I2C (SCL y SDA respectivamente).

| Pin Pantalla OLED | Pin Wemos D1 Mini | Función / Descripción |
| :--- | :--- | :--- |
| **VCC** | **3V3** | Alimentación lógica (3.3V) |
| **GND** | **G** | Tierra (Ground) |
| **SCL** | **D1** (GPIO5) | Reloj (Clock) del bus I2C |
| **SDA** | **D2** (GPIO4) | Datos (Data) del bus I2C |

### 🔋 Conexión de Batería LiPo (con módulo TP4056)

El TP4056 se encarga de cargar la batería de forma segura por USB y protegerla contra descargas profundas.

| Salida / Pin Origen | Conexión Destino | Función / Descripción |
| :--- | :--- | :--- |
| **LiPo (+)** (Rojo) | **B+** (TP4056) | Polo positivo de la batería al módulo |
| **LiPo (-)** (Negro) | **B-** (TP4056) | Polo negativo de la batería al módulo |
| **OUT+** (TP4056) | Interruptor -> **5V** (D1 Mini) | Salida regulada. Se conecta a través de un interruptor físico al pin de 5V del D1 Mini*. |
| **OUT-** (TP4056) | **G** (D1 Mini) | Cierre del circuito de alimentación (Tierra). |

> ***Nota Técnica:*** *Aunque la batería LiPo da entre 3.7V y 4.2V, meter este voltaje por el pin de 5V del D1 Mini es seguro, ya que pasa por el regulador de voltaje interno (LDO) que lo bajará a unos estables 3.3V para el chip ESP8266.*

## 📂 Estructura del Repositorio

* 📁 **`CODE/`**: Contiene el código fuente organizado para su despliegue:
  * `BOBEDA_ESP32_jmcaamnog_v3.ino`: Lógica del servidor web asíncrono en C++ para el ESP8266 con control de escritura segura (LittleFS).
  * `index_v3.h`: Interfaz gráfica de usuario y motor criptográfico en JS aislado en PROGMEM para un rendimiento óptimo sin depender de servicios externos.
* 📁 **`CAPTURAS/`**: Esquema de conexionado de elementos.

## ⚙️ Requisitos y Despliegue (Cómo flashear tu bóveda)

Necesitarás una placa basada en el chip **ESP8266** (Wemos D1 Mini o similar) y el IDE de Arduino.

1. Copia los archivos del directorio `CODE/` a tu espacio de trabajo local.
2. Instala las siguientes librerías en tu Arduino IDE:
   * `ESP8266WiFi`
   * `LittleFS`
   * `ESPAsyncTCP`
   * `ESPAsyncWebServer`
3. Conecta tu placa por USB, selecciona el puerto COM correspondiente y asegúrate de elegir un esquema de partición que deje espacio para la memoria de archivos **LittleFS** (por ejemplo, *FS: 1MB* o superior).
4. Sube el programa a la placa.

*Nota: La primera vez que accedas a la red WiFi por defecto, el sistema te redirigirá a la pantalla de inicialización en rojo para que establezcas tus contraseñas inmutables, claves de purga y parámetros de camuflaje de red.*

## 👨‍💻 Autor

**Jose Manuel Caamaño González** | Arquitecto Técnico & BIM Manager
Digital Product Lead | ConTech & Digital Twin SaaS | Data Analytics (SQL, Power BI)

Hecho con código y café desde A Coruña. ☕
[LinkedIn](https://www.linkedin.com/in/jmcaamanog/) · [Web](https://jmcaamanog.pages.dev)
