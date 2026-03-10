# Sistema de Búsqueda y Rescate LoRa
## Documento de Requerimientos

**Proyecto:** Sistema de localización peer-to-peer basado en LoRa para escenarios sin cobertura celular  
**Autor:** Juan  
**Director:** Ing. Gustavo Adolfo Ramírez Espinosa, PhD  
**Programa:** Maestría en Ingeniería de Internet de las Cosas - Pontificia Universidad Javeriana  
**Fecha:** Febrero 2026  
**Versión:** 1.0

---

## 1. REQUERIMIENTOS FUNCIONALES

### 1.1 Comunicación LoRa (RF-LORA)

| ID | Requerimiento | Prioridad | Estado |
|----|---------------|-----------|--------|
| RF-LORA-01 | El sistema debe establecer comunicación bidireccional LoRa entre Nodo A y Nodo B | Alta | ✓ |
| RF-LORA-02 | El sistema debe operar en la banda de 915 MHz (región américas) | Alta | ✓ |
| RF-LORA-03 | El sistema debe soportar alcance mínimo de 2 km en línea de vista | Alta | ✓ |
| RF-LORA-04 | El sistema debe implementar protocolo de mensajes con tipos: SEARCH_START, ACK, STATUS, BEACON, SOS_ALERT, SOS_CANCEL, REUNITE_CONFIRM | Alta | ✓ |
| RF-LORA-05 | El sistema debe incluir RSSI y SNR en cada mensaje recibido | Alta | ✓ |
| RF-LORA-06 | El sistema debe soportar cambio dinámico de Spreading Factor (SF7-SF12) según condiciones | Media | ✓ |
| RF-LORA-07 | El sistema debe implementar timeout de 3 segundos para espera de ACK | Alta | ✓ |
| RF-LORA-08 | El sistema debe reintentar transmisión hasta 5 veces antes de declarar fallo | Alta | ✓ |

### 1.2 Geolocalización GPS (RF-GPS)

| ID | Requerimiento | Prioridad | Estado |
|----|---------------|-----------|--------|
| RF-GPS-01 | El sistema debe obtener coordenadas GPS (latitud, longitud, altitud) | Alta | ✓ |
| RF-GPS-02 | El sistema debe reportar número de satélites y HDOP | Media | ✓ |
| RF-GPS-03 | El sistema debe detectar pérdida de fix GPS en menos de 10 segundos | Alta | ✓ |
| RF-GPS-04 | El sistema debe permitir timeout configurable para obtención de fix (default 30s) | Media | ✓ |
| RF-GPS-05 | El sistema debe calcular distancia entre dos coordenadas usando fórmula Haversine | Alta | ✓ |
| RF-GPS-06 | El sistema debe calcular rumbo (bearing) entre dos coordenadas | Alta | ✓ |
| RF-GPS-07 | El sistema debe operar sin GPS propio usando modo RSSI alternativo | Alta | ✓ |

### 1.3 Navegación y Orientación (RF-NAV)

| ID | Requerimiento | Prioridad | Estado |
|----|---------------|-----------|--------|
| RF-NAV-01 | El sistema debe mostrar distancia al objetivo en metros | Alta | ✓ |
| RF-NAV-02 | El sistema debe mostrar dirección al objetivo usando brújula digital | Alta | ✓ |
| RF-NAV-03 | El sistema debe soportar navegación sin brújula (solo distancia) | Alta | ✓ |
| RF-NAV-04 | El sistema debe implementar navegación tipo "hot/cold" basada en RSSI | Alta | ✓ |
| RF-NAV-05 | El sistema debe calcular tendencia de RSSI (acercándose/alejándose) | Alta | ✓ |
| RF-NAV-06 | El sistema debe detectar proximidad (<10m) para declarar reencuentro | Alta | ✓ |
| RF-NAV-07 | El sistema debe aplicar declinación magnética según ubicación geográfica | Media | ✓ |
| RF-NAV-08 | El sistema debe detectar interferencia magnética en la brújula | Media | ✓ |

### 1.4 Interfaz de Usuario (RF-UI)

| ID | Requerimiento | Prioridad | Estado |
|----|---------------|-----------|--------|
| RF-UI-01 | El sistema debe mostrar información en pantalla OLED 128x64 | Alta | ✓ |
| RF-UI-02 | El sistema debe mostrar flecha de dirección hacia el objetivo | Alta | ✓ |
| RF-UI-03 | El sistema debe mostrar barra de intensidad de señal RSSI | Media | ✓ |
| RF-UI-04 | El sistema debe indicar estado de batería | Media | ✓ |
| RF-UI-05 | El sistema debe operar sin pantalla usando LED RGB como indicador | Alta | ✓ |
| RF-UI-06 | El sistema debe operar sin pantalla usando vibrador como indicador | Alta | ✓ |
| RF-UI-07 | El sistema debe codificar distancia en frecuencia de parpadeo LED | Alta | ✓ |
| RF-UI-08 | El sistema debe codificar dirección en patrones de vibración | Alta | ✓ |
| RF-UI-09 | El sistema debe tener botón para iniciar búsqueda | Alta | ✓ |
| RF-UI-10 | El sistema debe tener botón SOS para emergencias | Alta | ✓ |

### 1.5 Modos de Búsqueda (RF-MODO)

| ID | Requerimiento | Prioridad | Estado |
|----|---------------|-----------|--------|
| RF-MODO-01 | El sistema debe soportar modo GPS con coordenadas completas | Alta | ✓ |
| RF-MODO-02 | El sistema debe soportar modo RSSI LoRa (beacons) | Alta | ✓ |
| RF-MODO-03 | El sistema debe soportar modo RSSI Wi-Fi (Access Point) | Alta | ✓ |
| RF-MODO-04 | El sistema debe soportar modo RSSI BLE (Advertising tipo AirTag) | Alta | ✓ |
| RF-MODO-05 | El sistema debe cambiar automáticamente entre modos según disponibilidad | Alta | ✓ |
| RF-MODO-06 | El sistema debe implementar fallback de Wi-Fi a LoRa si pierde señal | Alta | ✓ |

### 1.6 Gestión de Emergencias (RF-SOS)

| ID | Requerimiento | Prioridad | Estado |
|----|---------------|-----------|--------|
| RF-SOS-01 | El Nodo B debe poder activar SOS manteniendo botón 3 segundos | Alta | ✓ |
| RF-SOS-02 | El SOS debe transmitir alertas cada 1 segundo | Alta | ✓ |
| RF-SOS-03 | El SOS debe activar todos los canales (LoRa + Wi-Fi + BLE) | Alta | ✓ |
| RF-SOS-04 | El Nodo A debe mostrar alerta visual y háptica al recibir SOS | Alta | ✓ |
| RF-SOS-05 | El Nodo A debe priorizar búsqueda después de recibir SOS | Alta | ✓ |

### 1.7 Gestión de Energía (RF-PWR)

| ID | Requerimiento | Prioridad | Estado |
|----|---------------|-----------|--------|
| RF-PWR-01 | El Nodo B debe implementar duty cycle en modo escucha | Alta | ✓ |
| RF-PWR-02 | El sistema debe detectar batería baja (<20%) | Alta | ✓ |
| RF-PWR-03 | El sistema debe detectar batería crítica (<5%) | Alta | ✓ |
| RF-PWR-04 | El sistema debe reducir frecuencia de TX en modo batería baja | Alta | ✓ |
| RF-PWR-05 | El sistema debe enviar último STATUS antes de apagarse | Alta | ✓ |
| RF-PWR-06 | El sistema debe reportar nivel de batería en mensajes STATUS | Media | ✓ |

### 1.8 Conectividad Alternativa (RF-ALT)

| ID | Requerimiento | Prioridad | Estado |
|----|---------------|-----------|--------|
| RF-ALT-01 | El Nodo B debe poder crear Access Point Wi-Fi "RESCUE_XX" | Alta | ✓ |
| RF-ALT-02 | El Nodo A debe poder escanear y detectar AP "RESCUE_*" | Alta | ✓ |
| RF-ALT-03 | El Nodo B debe poder hacer BLE Advertising "RESCUE_XX" | Alta | ✓ |
| RF-ALT-04 | El Nodo A debe poder escanear BLE y detectar "RESCUE_*" | Alta | ✓ |
| RF-ALT-05 | El sistema debe medir RSSI de Wi-Fi para navegación | Alta | ✓ |
| RF-ALT-06 | El sistema debe medir RSSI de BLE para navegación | Alta | ✓ |

---

## 2. REQUERIMIENTOS NO FUNCIONALES

### 2.1 Rendimiento (RNF-PERF)

| ID | Requerimiento | Valor | Estado |
|----|---------------|-------|--------|
| RNF-PERF-01 | Latencia máxima de comunicación LoRa | < 500 ms | ✓ |
| RNF-PERF-02 | Tiempo de respuesta UI | < 100 ms | ✓ |
| RNF-PERF-03 | Frecuencia de actualización GPS | 1 Hz | ✓ |
| RNF-PERF-04 | Frecuencia de actualización RSSI | 0.5 Hz | ✓ |
| RNF-PERF-05 | Tiempo máximo para obtener fix GPS | 30 s | ✓ |
| RNF-PERF-06 | Alcance mínimo LoRa (línea de vista) | 2 km | ✓ |
| RNF-PERF-07 | Alcance mínimo Wi-Fi | 100 m | ✓ |
| RNF-PERF-08 | Alcance mínimo BLE | 50 m | ✓ |

### 2.2 Confiabilidad (RNF-CONF)

| ID | Requerimiento | Valor | Estado |
|----|---------------|-------|--------|
| RNF-CONF-01 | Disponibilidad del sistema | 99% | ✓ |
| RNF-CONF-02 | Tasa de entrega de mensajes LoRa | > 95% | ✓ |
| RNF-CONF-03 | Reintentos antes de declarar fallo | 5 | ✓ |
| RNF-CONF-04 | Timeout de señal perdida | 30 s | ✓ |
| RNF-CONF-05 | Timeout total antes de inalcanzable | 60 s | ✓ |

### 2.3 Usabilidad (RNF-USA)

| ID | Requerimiento | Descripción | Estado |
|----|---------------|-------------|--------|
| RNF-USA-01 | Operación con una mano | Botones accesibles con pulgar | ✓ |
| RNF-USA-02 | Feedback multimodal | Visual + Háptico + LED | ✓ |
| RNF-USA-03 | Modo ciego completo | Operable sin ver pantalla | ✓ |
| RNF-USA-04 | Indicadores intuitivos | Hot/Cold fácil de entender | ✓ |
| RNF-USA-05 | Sin configuración inicial | Plug & Play | ✓ |

### 2.4 Hardware (RNF-HW)

| ID | Requerimiento | Especificación | Estado |
|----|---------------|----------------|--------|
| RNF-HW-01 | Plataforma base | Heltec WiFi LoRa 32 V4 | ✓ |
| RNF-HW-02 | Microcontrolador | ESP32-S3 | ✓ |
| RNF-HW-03 | Chip LoRa | SX1262 | ✓ |
| RNF-HW-04 | Pantalla | OLED SSD1306 128x64 | ✓ |
| RNF-HW-05 | Módulo GPS | Compatible NMEA (ej. NEO-6M) | ✓ |
| RNF-HW-06 | Brújula | GY-271 (QMC5883L o HMC5883L) | ✓ |
| RNF-HW-07 | Alimentación | Batería LiPo 3.7V | ✓ |
| RNF-HW-08 | Indicadores | LED RGB + Vibrador | ✓ |

### 2.5 Software (RNF-SW)

| ID | Requerimiento | Especificación | Estado |
|----|---------------|----------------|--------|
| RNF-SW-01 | Framework | Arduino / PlatformIO | ✓ |
| RNF-SW-02 | Librería LoRa | RadioLib | ✓ |
| RNF-SW-03 | Librería GPS | TinyGPS++ | ✓ |
| RNF-SW-04 | Librería OLED | Adafruit GFX + SSD1306 | ✓ |
| RNF-SW-05 | Arquitectura | Máquina de Estados Finitos | ✓ |
| RNF-SW-06 | Patrón de diseño | Event-driven, modular | ✓ |

### 2.6 Ambientales (RNF-AMB)

| ID | Requerimiento | Especificación | Estado |
|----|---------------|----------------|--------|
| RNF-AMB-01 | Temperatura operación | -10°C a 50°C | ✓ |
| RNF-AMB-02 | Resistencia agua | IPX4 (salpicaduras) | Pendiente |
| RNF-AMB-03 | Resistencia golpes | Carcasa protectora | Pendiente |

---

## 3. CASOS DE USO vs REQUERIMIENTOS

| Caso de Uso | Requerimientos Relacionados |
|-------------|----------------------------|
| UC-01: Standby (B escuchando) | RF-PWR-01, RF-LORA-01 |
| UC-02: Búsqueda exitosa con GPS | RF-GPS-01 a 06, RF-NAV-01 a 02, RF-LORA-01 a 08 |
| UC-03: B sin GPS | RF-GPS-07, RF-MODO-02, RF-LORA-04 |
| UC-04: B no responde | RF-LORA-07, RF-LORA-08, RNF-CONF-03 |
| UC-05: A sin GPS | RF-GPS-07, RF-MODO-02 a 04 |
| UC-06: Brújula falla | RF-NAV-03, RF-NAV-08 |
| UC-07: RSSI LoRa (hot/cold) | RF-NAV-04 a 05, RF-MODO-02 |
| UC-08: RSSI Wi-Fi | RF-ALT-01 a 02, RF-ALT-05, RF-MODO-03 |
| UC-09: BLE AirTag | RF-ALT-03 a 04, RF-ALT-06, RF-MODO-04 |
| UC-10: Fallback Wi-Fi a LoRa | RF-MODO-05 a 06 |

---

## 4. MATRIZ DE TRAZABILIDAD

### 4.1 Estados Nodo A vs Requerimientos

| Estado | RF-LORA | RF-GPS | RF-NAV | RF-UI | RF-MODO | RF-SOS | RF-PWR | RF-ALT |
|--------|---------|--------|--------|-------|---------|--------|--------|--------|
| IDLE | 01 | - | - | 01,09 | - | - | - | - |
| SEARCHING | 01-08 | 01-06 | 01-02,06 | 01-04 | 01 | - | - | - |
| RETRY | 07-08 | - | - | 01 | - | - | - | - |
| NO_GPS | 04 | 07 | 04-05 | 01 | 02-04 | - | - | - |
| RSSI_LORA | 04-05 | - | 04-05 | 01,03 | 02 | - | - | - |
| RSSI_WIFI | - | - | 04-05 | 01,03 | 03 | - | - | 01-02,05 |
| RSSI_BLE | - | - | 04-05 | 01,03 | 04 | - | - | 03-04,06 |
| NO_COMPASS | 04 | 05-06 | 03 | 01 | - | - | - | - |
| BLIND | 04 | 05-06 | - | 05-08 | - | - | - | - |
| SIGNAL_LOST | - | - | - | 01 | - | - | - | - |
| SOS | 04 | - | - | 01 | - | 01-05 | - | - |
| LORA_FAIL | - | - | - | 01 | 05-06 | - | - | 01-06 |
| UNREACHABLE | - | - | - | 01,09 | - | - | - | - |
| REUNITED | 04 | - | 06 | 01,05-06 | - | - | - | - |

### 4.2 Estados Nodo B vs Requerimientos

| Estado | RF-LORA | RF-GPS | RF-NAV | RF-UI | RF-MODO | RF-SOS | RF-PWR | RF-ALT |
|--------|---------|--------|--------|-------|---------|--------|--------|--------|
| LISTENING | 01 | - | - | - | - | - | 01 | - |
| ALERTING | 01-05 | 01-04 | - | 01,04,10 | 01 | - | 06 | - |
| NO_GPS | 04 | 03 | - | 01 | 02 | - | - | - |
| BEACON | 04-05 | - | - | 01 | 02 | - | - | - |
| WIFI_AP | 04 | - | - | 01 | 03 | - | - | 01 |
| BLE_ADV | 04 | - | - | 01 | 04 | - | - | 03 |
| SOS | 04 | 01 | - | 01,10 | - | 01-04 | - | 01,03 |
| LOW_POWER | 04 | - | - | 01,04 | - | - | 02,04,06 | - |
| CRITICAL | 04 | 01 | - | 01,04 | - | - | 03,05 | 01,03 |
| REUNITED | 04 | - | - | 01 | - | - | - | - |

---

## 5. CRITERIOS DE ACEPTACIÓN

### 5.1 Pruebas Funcionales

| ID | Prueba | Criterio de Éxito |
|----|--------|-------------------|
| PF-01 | Comunicación LoRa básica | A envía mensaje, B lo recibe y responde |
| PF-02 | Búsqueda con GPS | A localiza a B usando coordenadas |
| PF-03 | Búsqueda sin GPS (RSSI) | A localiza a B usando hot/cold |
| PF-04 | Fallback Wi-Fi | A detecta AP de B cuando LoRa falla |
| PF-05 | Fallback BLE | A detecta advertising de B |
| PF-06 | SOS | B envía SOS, A recibe y responde |
| PF-07 | Batería baja | B reduce consumo cuando bat<20% |
| PF-08 | Modo ciego | A navega usando solo LED y vibrador |
| PF-09 | Sin brújula | A navega usando solo distancia |
| PF-10 | Reencuentro | Sistema detecta proximidad <10m |

### 5.2 Pruebas de Rendimiento

| ID | Prueba | Criterio de Éxito |
|----|--------|-------------------|
| PP-01 | Alcance LoRa | Comunicación exitosa a 2km (línea vista) |
| PP-02 | Latencia | Respuesta ACK en <500ms |
| PP-03 | Duración batería LISTENING | >24 horas en modo duty cycle |
| PP-04 | Duración batería ALERTING | >4 horas transmitiendo |
| PP-05 | Precisión GPS | Error <10m |
| PP-06 | Precisión brújula | Error <15° |

---

## 6. GLOSARIO

| Término | Definición |
|---------|------------|
| LoRa | Long Range - Tecnología de comunicación inalámbrica de largo alcance y bajo consumo |
| RSSI | Received Signal Strength Indicator - Indicador de intensidad de señal recibida |
| SNR | Signal-to-Noise Ratio - Relación señal a ruido |
| SF | Spreading Factor - Factor de dispersión en modulación LoRa (SF7-SF12) |
| Fix | Obtención válida de posición GPS |
| HDOP | Horizontal Dilution of Precision - Precisión horizontal del GPS |
| Duty Cycle | Ciclo de trabajo - Alternar entre activo y dormido para ahorrar energía |
| Hot/Cold | Método de navegación indicando si te acercas (caliente) o alejas (frío) |
| Beacon | Señal periódica transmitida para ser detectada y medida |
| AP | Access Point - Punto de acceso Wi-Fi |
| Advertising | Transmisión BLE para ser descubierto por otros dispositivos |

---

## 7. HISTORIAL DE CAMBIOS

| Versión | Fecha | Cambios |
|---------|-------|---------|
| 1.0 | Feb 2026 | Versión inicial con 10 casos de uso |

---

**Aprobado por:**

_________________________  
Ing. Gustavo Adolfo Ramírez Espinosa, PhD  
Director de Tesis

_________________________  
Juan  
Estudiante
