# Diagrama de Estados — LoRa Finder
**Proyecto:** Sistema de búsqueda basado en LoRa
**Autor:** Juan Camilo Moreno Zornosa
**Fecha:** Marzo 2026

---

## 1. Arquitectura de Módulos

Muestra cómo cada archivo del firmware se relaciona con los demás.

```mermaid
graph TD
    MAIN["main.cpp\nsetup() / loop()"]
    APP["app.cpp\napp_init() / app_run()"]
    RM["role_manager.cpp\nROLE_NONE · SEARCHER · TARGET · TEST_MENU"]

    SRCH["fsm_searcher.cpp\n7 estados del Buscador"]
    TGT["fsm_target.cpp\n5 estados del Objetivo"]
    TEST["test_menu.cpp\nPruebas de campo"]

    COMM["lora_comm.cpp\nCapa de protocolo"]
    MSG["lora_msg.h\nPaquete 17 bytes"]
    NAV["navigation.cpp\nHaversine + Bearing"]
    PWR["power.cpp\nStandby pantalla"]

    DRV_LORA["drivers/lora\nSX1262 · RadioLib"]
    DRV_GPS["drivers/gps\nNEO-M8N · TinyGPS++"]
    DRV_CMP["drivers/compass\nQMC5883L · I2C"]
    DRV_DSP["drivers/display\nOLED SSD1306 · I2C"]
    DRV_BAT["drivers/battery\nADC divisor"]
    DRV_BTN["drivers/btn\nDebounce · Hold"]
    DRV_ALT["drivers/alert\nBuzzer · Vibrador · LED"]

    HW_LORA[("SX1262\nHeltec V3")]
    HW_GPS[("NEO-M8N\nUART2")]
    HW_CMP[("QMC5883L\nWire1")]
    HW_OLED[("OLED 128×64\nWire0")]
    HW_BTN[("BTN UP/DN/SEL\nGPIO 2·3·4")]

    MAIN --> APP
    APP --> RM

    RM --> SRCH
    RM --> TGT
    RM --> TEST
    RM --> PWR

    SRCH --> COMM
    SRCH --> NAV
    SRCH --> DRV_GPS
    SRCH --> DRV_CMP
    SRCH --> DRV_DSP
    SRCH --> DRV_BAT
    SRCH --> DRV_BTN
    SRCH --> DRV_ALT

    TGT --> COMM
    TGT --> DRV_GPS
    TGT --> DRV_CMP
    TGT --> DRV_DSP
    TGT --> DRV_BAT
    TGT --> DRV_BTN
    TGT --> DRV_ALT

    COMM --> MSG
    COMM --> DRV_LORA
    COMM --> DRV_ALT

    DRV_LORA --> HW_LORA
    DRV_GPS  --> HW_GPS
    DRV_CMP  --> HW_CMP
    DRV_DSP  --> HW_OLED
    DRV_BTN  --> HW_BTN

    style HW_LORA fill:#d4e6f1,stroke:#2980b9
    style HW_GPS  fill:#d5f5e3,stroke:#27ae60
    style HW_CMP  fill:#fdebd0,stroke:#e67e22
    style HW_OLED fill:#e8daef,stroke:#8e44ad
    style HW_BTN  fill:#fdfefe,stroke:#717d7e
```

---

## 2. Máquina de Estados del Role Manager

El `role_manager` decide qué FSM está activa. Ambos dispositivos corren el **mismo binario** — el rol se asigna dinámicamente.

```mermaid
stateDiagram-v2
    direction LR

    [*] --> ROLE_NONE : Encendido

    ROLE_NONE --> ROLE_SEARCHER : BTN_SELECT presionado\n(cursor = BUSCAR)
    ROLE_NONE --> ROLE_TARGET   : Recibe MSG_SEARCH_START\npor LoRa
    ROLE_NONE --> ROLE_TEST_MENU: BTN_SELECT presionado\n(cursor = MENU)
    ROLE_NONE --> ROLE_NONE     : Timeout pantalla\n→ display off (CPU sigue activo)

    ROLE_SEARCHER  --> ROLE_NONE : fsm_searcher_is_done()\n(DONE o timeout)
    ROLE_TARGET    --> ROLE_NONE : fsm_target_is_done()\n(DONE o timeout)
    ROLE_TEST_MENU --> ROLE_NONE : test_menu_is_done()

    note right of ROLE_NONE
        Pantalla: Home con brújula
        Opciones: MENU · BUSCAR · OFF-DIS
        PING periódico de LoRa
        standby display si inactivo
    end note

    note right of ROLE_SEARCHER
        Inicia SEARCH_START
        Navega hacia el objetivo
        Muestra distancia + flecha
    end note

    note right of ROLE_TARGET
        Alerta al usuario
        Envía balizas GPS
        Soporta SOS
    end note
```

---

## 3. FSM del Buscador (`fsm_searcher`)

Activo cuando el usuario presiona **BUSCAR** en la pantalla de inicio.

```mermaid
stateDiagram-v2
    direction TB

    [*] --> SEARCHER_SEARCHING : fsm_searcher_init()\nEnvía MSG_SEARCH_START

    SEARCHER_SEARCHING --> SEARCHER_NAV_GPS  : Recibe MSG_ACK\ncon GPS válido del objetivo
    SEARCHER_SEARCHING --> SEARCHER_NAV_RSSI : Recibe MSG_ACK\nsin GPS (lat=0, lon=0)
    SEARCHER_SEARCHING --> SEARCHER_DONE     : 5 reintentos × 3s sin ACK\no BTN_SELECT

    SEARCHER_NAV_GPS  --> SEARCHER_NAV_RSSI  : Objetivo pierde GPS\n(lat=0 en BEACON)
    SEARCHER_NAV_RSSI --> SEARCHER_NAV_GPS   : Objetivo recupera GPS\n(lat≠0 en BEACON)

    SEARCHER_NAV_GPS  --> SEARCHER_REUNITED  : Distancia < 20 m\no RSSI > -60 dBm
    SEARCHER_NAV_RSSI --> SEARCHER_REUNITED  : RSSI > -60 dBm
    SEARCHER_NAV_GPS  --> SEARCHER_SIGNAL_LOST : Sin paquetes 15 s
    SEARCHER_NAV_RSSI --> SEARCHER_SIGNAL_LOST : Sin paquetes 15 s
    SEARCHER_NAV_GPS  --> SEARCHER_SOS       : Recibe MSG_SOS_ALERT
    SEARCHER_NAV_RSSI --> SEARCHER_SOS       : Recibe MSG_SOS_ALERT
    SEARCHER_NAV_GPS  --> SEARCHER_DONE      : BTN_SELECT
    SEARCHER_NAV_RSSI --> SEARCHER_DONE      : BTN_SELECT

    SEARCHER_SIGNAL_LOST --> SEARCHER_NAV_GPS  : Recibe BEACON con GPS
    SEARCHER_SIGNAL_LOST --> SEARCHER_NAV_RSSI : Recibe BEACON sin GPS
    SEARCHER_SIGNAL_LOST --> SEARCHER_DONE     : 60 s sin señal\no BTN_SELECT

    SEARCHER_REUNITED --> SEARCHER_DONE : BTN_SELECT\no 4 s de pantalla

    SEARCHER_SOS --> SEARCHER_NAV_GPS  : BTN_SELECT (entendido)\no recibe MSG_SOS_CANCEL con GPS
    SEARCHER_SOS --> SEARCHER_NAV_RSSI : recibe MSG_SOS_CANCEL sin GPS

    SEARCHER_DONE --> [*]

    note right of SEARCHER_SEARCHING
        Pantalla: "Buscando..."
        Barra de progreso
        Reintenta cada 3 s
        Máx 5 intentos
    end note

    note right of SEARCHER_NAV_GPS
        Pantalla: Flecha + distancia
        Envía MSG_STATUS cada 2 s
        Recibe MSG_BEACON cada 2 s
        Calcula Haversine + bearing
    end note

    note right of SEARCHER_NAV_RSSI
        Pantalla: RSSI + CALIENTE/FRÍO
        Mismo intervalo que GPS
        Sin brújula de dirección
    end note

    note right of SEARCHER_SIGNAL_LOST
        Última posición en pantalla
        Sigue enviando STATUS
        Espera recuperar señal
    end note
```

---

## 4. FSM del Objetivo (`fsm_target`)

Se activa **automáticamente** al recibir un `MSG_SEARCH_START` por LoRa.

```mermaid
stateDiagram-v2
    direction TB

    [*] --> TARGET_ALERTING : fsm_target_init()\nRecibe MSG_SEARCH_START

    TARGET_ALERTING --> TARGET_BEACON  : BTN_SELECT presionado\n→ envía MSG_ACK
    TARGET_ALERTING --> TARGET_DONE    : Timeout 60 s\n(nadie pulsó SELECT)

    TARGET_BEACON --> TARGET_SOS      : BTN_UP sostenido 2 s\n→ envía MSG_SOS_ALERT
    TARGET_BEACON --> TARGET_REUNITED : Recibe MSG_REUNITE_CONFIRM
    TARGET_BEACON --> TARGET_DONE     : BTN_SELECT\n→ envía MSG_REUNITE_CONFIRM

    TARGET_SOS --> TARGET_BEACON  : BTN_SELECT (cancelar SOS)\n→ envía MSG_SOS_CANCEL

    TARGET_REUNITED --> TARGET_DONE : 4 s automático

    TARGET_DONE --> [*]

    note right of TARGET_ALERTING
        Pantalla: "¡Te buscan!"
        Alerta visual + buzzer
        cada 3 s
        Espera confirmación
        del usuario
    end note

    note right of TARGET_BEACON
        Pantalla: Modo baliza
        GPS + coordenadas
        Envía MSG_BEACON cada 2 s
        con lat/lon/heading/bat
    end note

    note right of TARGET_SOS
        Pantalla: "SOS ACTIVO"
        MSG_SOS_ALERT cada 1 s
        Buzzer + vibrador
    end note
```

---

## 5. Flujo de Mensajes LoRa entre Dispositivos

Intercambio de paquetes durante una sesión de búsqueda completa.

```mermaid
sequenceDiagram
    participant A as Dispositivo A<br/>(Buscador)
    participant B as Dispositivo B<br/>(Objetivo)

    Note over A: BTN_SELECT → ROLE_SEARCHER

    A->>B: MSG_SEARCH_START (broadcast)<br/>sender_id, lat, lon, heading, bat
    A->>B: MSG_SEARCH_START (retry 2/5)
    Note over B: Recibe → ROLE_TARGET<br/>Alerta al usuario
    Note over B: BTN_SELECT → acepta

    B->>A: MSG_ACK<br/>sender_id, lat, lon, gps_status

    Note over A: ACK con GPS → SEARCHER_NAV_GPS<br/>ACK sin GPS → SEARCHER_NAV_RSSI

    loop Cada 2 segundos (modo navegación)
        A->>B: MSG_STATUS<br/>posición del buscador
        B->>A: MSG_BEACON<br/>posición del objetivo
        Note over A: Calcula distancia y bearing
    end

    opt Emergencia en Objetivo
        B->>A: MSG_SOS_ALERT
        Note over A: Estado → SEARCHER_SOS\nAlertas periódicas
        Note over A: BTN_SELECT → "entendido"
        B->>A: MSG_SOS_CANCEL
        Note over A: Vuelve a NAV_GPS/RSSI
    end

    Note over A: Distancia < 20 m\no RSSI > -60 dBm

    A->>B: MSG_REUNITE_CONFIRM
    Note over A: Estado → SEARCHER_REUNITED
    Note over B: Estado → TARGET_REUNITED

    Note over A,B: Ambos vuelven a ROLE_NONE
```

---

## 6. Estructura del Paquete LoRa (`lora_msg_t`)

Paquete de **17 bytes fijos**, sin cabecera variable.

```
┌──────────┬───────────────────┬───────────┬───────────┬─────────────┬─────────┬───────────┐
│ Byte 0   │ Bytes 1–4         │ Bytes 5–8 │ Bytes 9–12│ Bytes 13–14 │ Byte 15 │ Byte 16   │
├──────────┼───────────────────┼───────────┼───────────┼─────────────┼─────────┼───────────┤
│ msg_type │ sender_id         │ lat_i     │ lon_i     │ heading_x10 │ bat_pct │ rssi_last │
│ uint8_t  │ uint32_t (MAC)    │ int32_t   │ int32_t   │ int16_t     │ uint8_t │ int8_t    │
│          │                   │ ×10⁶      │ ×10⁶      │ ×10 (0–3599)│ 0–100 % │ dBm       │
└──────────┴───────────────────┴───────────┴───────────┴─────────────┴─────────┴───────────┘
```

| msg_type | Código | Dirección | Descripción |
|----------|--------|-----------|-------------|
| MSG_SEARCH_START | 0x01 | A → broadcast | Inicia búsqueda |
| MSG_ACK | 0x02 | B → A | Objetivo acepta |
| MSG_STATUS | 0x03 | A → B | Posición del buscador |
| MSG_BEACON | 0x04 | B → A | Baliza periódica del objetivo |
| MSG_SOS_ALERT | 0x05 | B → A | Emergencia activa |
| MSG_SOS_CANCEL | 0x06 | B → A | Cancela emergencia |
| MSG_REUNITE_CONFIRM | 0x07 | cualquiera → otro | Confirma reunión |
| MSG_TEST_PING | 0x10 | broadcast | Prueba de campo TX |
| MSG_TEST_PONG | 0x11 | respuesta | Prueba de campo RX |

---

## 7. Configuración LoRa (`system_config.h`)

| Parámetro | Valor | Justificación |
|-----------|-------|---------------|
| Frecuencia | 915 MHz | Banda ISM Región 2 — Colombia |
| Bandwidth | 125 kHz | Balance sensibilidad / velocidad |
| Spreading Factor | SF7 | ToA ~41 ms, alcance urbano 200–300 m |
| TX Power | **22 dBm** | Máximo del SX1262 |
| Coding Rate | 4/5 | Redundancia mínima |
| Reintento SEARCH_START | 3 s × 5 | 15 s timeout total |
| Intervalo STATUS/BEACON | 2 s | Actualización de posición |
| Señal perdida | 15 s sin paquete | → estado SIGNAL_LOST |
| Abandono | 60 s en SIGNAL_LOST | → DONE |
| Umbral reunión GPS | < 20 m | Haversine entre ambos nodos |
| Umbral reunión RSSI | > −60 dBm | Sin GPS disponible |
