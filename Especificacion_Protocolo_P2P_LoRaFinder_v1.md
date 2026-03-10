# Especificación del Protocolo de Comunicación P2P

## Sistema de Búsqueda LoRa — LoRa Finder

---

**Versión:** 1.0  
**Fecha:** Enero 2026  
**Autor:** Juan Camilo Moreno Zornosa  
**Proyecto:** Trabajo de Grado — Maestría en IoT  

---

## Tabla de Contenidos

1. [Introducción](#1-introducción)
2. [Arquitectura del Sistema](#2-arquitectura-del-sistema)
3. [Identificación de Nodos](#3-identificación-de-nodos)
4. [Tipos de Mensajes](#4-tipos-de-mensajes)
5. [Estructura del Paquete](#5-estructura-del-paquete)
6. [Codificación del Payload](#6-codificación-del-payload)
7. [Máquina de Estados](#7-máquina-de-estados)
8. [Secuencias de Comunicación](#8-secuencias-de-comunicación)
9. [Manejo de Errores](#9-manejo-de-errores)
10. [Configuración LoRa](#10-configuración-lora)
11. [Implementación en C++](#11-implementación-en-c)

---

## 1. Introducción

### 1.1 Propósito

Este documento define el protocolo de comunicación punto a punto (P2P) para el sistema LoRa Finder, diseñado para permitir la localización y comunicación básica entre dos dispositivos portátiles en escenarios donde la red celular no está disponible.

### 1.2 Alcance

El protocolo está optimizado para:
- Comunicación bidireccional entre exactamente **2 dispositivos**
- Transmisión de alertas y coordenadas GPS
- Mínimo Time on Air (ToA) para conservar energía
- Operación en banda ISM 915 MHz (región 2 - Colombia)

### 1.3 Requisitos del Protocolo

| Requisito | Especificación |
|-----------|----------------|
| Topología | Punto a punto (P2P) |
| Número de nodos | 2 (fijo) |
| Latencia máxima | < 5 segundos (sin GPS) |
| Tamaño máximo payload | 16 bytes |
| Confirmación de entrega | Obligatoria (ACK) |
| Reintentos | 3 intentos con backoff |

---

## 2. Arquitectura del Sistema

### 2.1 Diagrama de Alto Nivel

```
┌─────────────────────┐                    ┌─────────────────────┐
│   DISPOSITIVO A     │                    │   DISPOSITIVO B     │
│   (Node ID: 0xA1)   │                    │   (Node ID: 0xB2)   │
│                     │                    │                     │
│  ┌───────────────┐  │    LoRa P2P 915MHz │  ┌───────────────┐  │
│  │   ESP32-S3    │  │◄──────────────────►│  │   ESP32-S3    │  │
│  │  Heltec V4    │  │                    │  │  Heltec V4    │  │
│  └───────┬───────┘  │                    │  └───────┬───────┘  │
│          │          │                    │          │          │
│  ┌───────┴───────┐  │                    │  ┌───────┴───────┐  │
│  │ GPS │ OLED    │  │                    │  │ GPS │ OLED    │  │
│  │ GY271│ Button │  │                    │  │ GY271│ Button │  │
│  └───────────────┘  │                    │  └───────────────┘  │
└─────────────────────┘                    └─────────────────────┘
        Usuario A                                 Usuario B
       (Buscador)                                (Buscado)
```

### 2.2 Roles de los Dispositivos

Ambos dispositivos son **idénticos en hardware y firmware**. El rol se determina dinámicamente según quién inicie la búsqueda:

| Rol | Descripción | Acciones |
|-----|-------------|----------|
| **SEEKER** (Buscador) | Dispositivo que inicia la búsqueda | Envía ALERT, recibe LOCATION, muestra distancia |
| **TARGET** (Buscado) | Dispositivo que responde | Recibe ALERT, activa GPS, envía LOCATION |

---

## 3. Identificación de Nodos

### 3.1 Esquema de Node ID

Cada dispositivo tiene un identificador único de **1 byte** programado en el firmware durante la configuración inicial.

```
Node ID: 1 byte (0x00 - 0xFF)
```

### 3.2 Asignación de IDs

Para este sistema de 2 dispositivos, se utilizan IDs fijos:

| Dispositivo | Node ID | Constante en código |
|-------------|---------|---------------------|
| Dispositivo A | `0xA1` | `NODE_ID_A` |
| Dispositivo B | `0xB2` | `NODE_ID_B` |
| Broadcast | `0xFF` | `NODE_ID_BROADCAST` |

### 3.3 Configuración en Firmware

```cpp
// config.h
#define NODE_ID_SELF    0xA1    // Cambiar a 0xB2 para el otro dispositivo
#define NODE_ID_PEER    0xB2    // Cambiar a 0xA1 para el otro dispositivo
#define NODE_ID_BROADCAST 0xFF
```

### 3.4 Pareamiento (Pairing)

El pareamiento es **estático** (definido en tiempo de compilación). Cada dispositivo conoce:
- Su propio ID (`NODE_ID_SELF`)
- El ID de su par (`NODE_ID_PEER`)

> **Nota:** Para una versión comercial futura, se podría implementar pareamiento dinámico con intercambio de claves.

---

## 4. Tipos de Mensajes

### 4.1 Tabla de Tipos de Mensaje

| Tipo | Código | Dirección | Requiere ACK | Payload | Descripción |
|------|--------|-----------|--------------|---------|-------------|
| `PING` | `0x01` | Bidireccional | Sí (PONG) | 0 bytes | Verificación de enlace |
| `PONG` | `0x02` | Respuesta | No | 2 bytes | Respuesta a PING con RSSI |
| `ALERT` | `0x10` | Seeker → Target | Sí (ALERT_ACK) | 0 bytes | Solicitud de búsqueda |
| `ALERT_ACK` | `0x11` | Target → Seeker | No | 1 byte | Confirmación de alerta recibida |
| `LOCATION` | `0x20` | Target → Seeker | Sí (LOC_ACK) | 12 bytes | Coordenadas GPS |
| `LOC_ACK` | `0x21` | Seeker → Target | No | 0 bytes | Confirmación de ubicación recibida |
| `CANCEL` | `0x30` | Bidireccional | No | 0 bytes | Cancelar modo búsqueda |
| `BATTERY` | `0x40` | Bidireccional | No | 1 byte | Estado de batería |

### 4.2 Descripción Detallada de Mensajes

#### 4.2.1 PING (0x01)
- **Propósito:** Verificar que el otro dispositivo está en rango y operativo
- **Cuándo se usa:** Periódicamente en modo standby (cada 30 segundos)
- **Respuesta esperada:** PONG dentro de 2 segundos

#### 4.2.2 PONG (0x02)
- **Propósito:** Responder al PING con información de calidad de señal
- **Payload:** RSSI del mensaje PING recibido (2 bytes, signed int16)

#### 4.2.3 ALERT (0x10)
- **Propósito:** Iniciar el modo de búsqueda
- **Comportamiento del receptor:**
  1. Activar alerta visual/sonora
  2. Encender GPS
  3. Responder con ALERT_ACK
  4. Cambiar a estado SEARCH

#### 4.2.4 ALERT_ACK (0x11)
- **Propósito:** Confirmar que la alerta fue recibida
- **Payload:** Estado del GPS (1 byte)
  - `0x00`: GPS apagado, iniciando...
  - `0x01`: GPS encendido, buscando fix
  - `0x02`: GPS con fix válido

#### 4.2.5 LOCATION (0x20)
- **Propósito:** Enviar coordenadas GPS al buscador
- **Payload:** 12 bytes (ver sección 6)
- **Frecuencia:** Cada 5 segundos mientras esté activo el modo búsqueda

#### 4.2.6 LOC_ACK (0x21)
- **Propósito:** Confirmar recepción de ubicación
- **Importancia:** Permite al TARGET saber que su ubicación llegó correctamente

#### 4.2.7 CANCEL (0x30)
- **Propósito:** Terminar el modo de búsqueda
- **Comportamiento:** Ambos dispositivos vuelven a estado STANDBY

#### 4.2.8 BATTERY (0x40)
- **Propósito:** Informar nivel de batería al otro dispositivo
- **Payload:** Porcentaje de batería (1 byte, 0-100)

---

## 5. Estructura del Paquete

### 5.1 Formato General del Paquete

```
┌─────────────────────────────────────────────────────────────────┐
│                        PAQUETE LoRa                             │
├──────────┬──────────┬──────────┬──────────┬─────────────────────┤
│  BYTE 0  │  BYTE 1  │  BYTE 2  │  BYTE 3  │  BYTES 4-15         │
├──────────┼──────────┼──────────┼──────────┼─────────────────────┤
│  SRC_ID  │  DST_ID  │ MSG_TYPE │  SEQ_NUM │  PAYLOAD (0-12)     │
│ (1 byte) │ (1 byte) │ (1 byte) │ (1 byte) │  (variable)         │
└──────────┴──────────┴──────────┴──────────┴─────────────────────┘
     │          │          │          │              │
     │          │          │          │              └─► Datos específicos
     │          │          │          │                  del tipo de mensaje
     │          │          │          │
     │          │          │          └─► Número de secuencia (0-255)
     │          │          │              Para detección de duplicados
     │          │          │              y ordenamiento
     │          │          │
     │          │          └─► Tipo de mensaje (ver tabla 4.1)
     │          │
     │          └─► ID del destinatario (0xFF = broadcast)
     │
     └─► ID del remitente
```

### 5.2 Header (4 bytes fijos)

| Campo | Offset | Tamaño | Descripción |
|-------|--------|--------|-------------|
| `SRC_ID` | 0 | 1 byte | ID del nodo que envía |
| `DST_ID` | 1 | 1 byte | ID del nodo destino (0xFF = broadcast) |
| `MSG_TYPE` | 2 | 1 byte | Tipo de mensaje |
| `SEQ_NUM` | 3 | 1 byte | Número de secuencia (0-255, wrap around) |

### 5.3 Definición en C++

```cpp
// protocol.h

#pragma once
#include <stdint.h>

// Tamaños
#define HEADER_SIZE     4
#define MAX_PAYLOAD     12
#define MAX_PACKET_SIZE (HEADER_SIZE + MAX_PAYLOAD)  // 16 bytes

// Tipos de mensaje
typedef enum {
    MSG_PING      = 0x01,
    MSG_PONG      = 0x02,
    MSG_ALERT     = 0x10,
    MSG_ALERT_ACK = 0x11,
    MSG_LOCATION  = 0x20,
    MSG_LOC_ACK   = 0x21,
    MSG_CANCEL    = 0x30,
    MSG_BATTERY   = 0x40
} MessageType;

// Estructura del header
typedef struct __attribute__((packed)) {
    uint8_t src_id;
    uint8_t dst_id;
    uint8_t msg_type;
    uint8_t seq_num;
} PacketHeader;

// Estructura completa del paquete
typedef struct __attribute__((packed)) {
    PacketHeader header;
    uint8_t payload[MAX_PAYLOAD];
} LoRaPacket;
```

### 5.4 Número de Secuencia (SEQ_NUM)

El número de secuencia cumple varias funciones:

1. **Detección de duplicados:** Si se recibe un paquete con el mismo SEQ_NUM del mismo SRC_ID, se descarta
2. **Matching de ACKs:** El ACK debe tener el mismo SEQ_NUM que el mensaje original
3. **Ordenamiento:** Permite detectar paquetes fuera de orden

```cpp
// Gestión del número de secuencia
uint8_t current_seq_num = 0;

uint8_t getNextSeqNum() {
    return current_seq_num++;  // Wrap around automático (0-255)
}
```

---

## 6. Codificación del Payload

### 6.1 Payload LOCATION (12 bytes)

Este es el payload más importante y complejo del protocolo. Contiene la ubicación GPS del dispositivo buscado.

#### 6.1.1 Estructura del Payload LOCATION

```
┌─────────────────────────────────────────────────────────────────────────┐
│                      PAYLOAD LOCATION (12 bytes)                        │
├───────────────┬───────────────┬───────────────┬───────────────┬─────────┤
│   BYTES 0-3   │   BYTES 4-7   │   BYTES 8-9   │   BYTE 10     │ BYTE 11 │
├───────────────┼───────────────┼───────────────┼───────────────┼─────────┤
│   LATITUDE    │   LONGITUDE   │   HEADING     │   SATELLITES  │  FLAGS  │
│   (int32_t)   │   (int32_t)   │   (uint16_t)  │   (uint8_t)   │(uint8_t)│
│   scaled×10⁶  │   scaled×10⁶  │   0-359°      │   0-255       │ bitfield│
└───────────────┴───────────────┴───────────────┴───────────────┴─────────┘
```

#### 6.1.2 Codificación de Coordenadas GPS

Las coordenadas GPS se codifican usando **escalado por 10⁶** para convertir valores de punto flotante a enteros, eliminando la necesidad de transmitir floats.

**Rango de valores:**
- Latitud: -90.000000 a +90.000000 → -90,000,000 a +90,000,000
- Longitud: -180.000000 a +180.000000 → -180,000,000 a +180,000,000

**Precisión:** 
- 1 unidad = 0.000001° ≈ 0.11 metros en el ecuador
- Suficiente para la aplicación (precisión GPS típica: 3-5 metros)

```cpp
// Codificación (TX)
int32_t lat_encoded = (int32_t)(latitude * 1000000.0);
int32_t lon_encoded = (int32_t)(longitude * 1000000.0);

// Decodificación (RX)
double latitude = (double)lat_encoded / 1000000.0;
double longitude = (double)lon_encoded / 1000000.0;
```

#### 6.1.3 Campo HEADING (Orientación)

El heading de la brújula se transmite como un entero de 16 bits (0-359 grados).

```cpp
// Codificación
uint16_t heading_encoded = (uint16_t)compass_heading;  // 0-359

// Decodificación
float heading = (float)heading_encoded;
```

#### 6.1.4 Campo SATELLITES

Número de satélites GPS visibles. Útil para evaluar la calidad del fix.

| Valor | Significado |
|-------|-------------|
| 0 | Sin fix GPS |
| 1-3 | Fix 2D (poco confiable) |
| 4+ | Fix 3D (confiable) |

#### 6.1.5 Campo FLAGS (Bitfield)

```
┌─────┬─────┬─────┬─────┬─────┬─────┬─────┬─────┐
│ Bit7│ Bit6│ Bit5│ Bit4│ Bit3│ Bit2│ Bit1│ Bit0│
├─────┼─────┼─────┼─────┼─────┼─────┼─────┼─────┤
│ RSV │ RSV │ RSV │ RSV │ RSV │MOVE │VALID│ FIX │
└─────┴─────┴─────┴─────┴─────┴─────┴─────┴─────┘
```

| Bit | Nombre | Descripción |
|-----|--------|-------------|
| 0 | `FIX` | 1 = GPS tiene fix, 0 = sin fix |
| 1 | `VALID` | 1 = Coordenadas válidas, 0 = usando última conocida |
| 2 | `MOVING` | 1 = Dispositivo en movimiento, 0 = estático |
| 3-7 | Reserved | Para uso futuro |

```cpp
// Definición de flags
#define FLAG_FIX    (1 << 0)
#define FLAG_VALID  (1 << 1)
#define FLAG_MOVING (1 << 2)

// Uso
uint8_t flags = 0;
if (gps.hasFix()) flags |= FLAG_FIX;
if (gps.isValid()) flags |= FLAG_VALID;
if (speed > 0.5) flags |= FLAG_MOVING;
```

### 6.2 Estructura de Datos en C++

```cpp
// protocol.h

// Payload para mensaje LOCATION
typedef struct __attribute__((packed)) {
    int32_t  latitude;    // Latitud × 10⁶
    int32_t  longitude;   // Longitud × 10⁶
    uint16_t heading;     // Orientación 0-359°
    uint8_t  satellites;  // Número de satélites
    uint8_t  flags;       // Flags de estado
} LocationPayload;

// Payload para mensaje PONG
typedef struct __attribute__((packed)) {
    int16_t rssi;         // RSSI del PING recibido
} PongPayload;

// Payload para mensaje ALERT_ACK
typedef struct __attribute__((packed)) {
    uint8_t gps_status;   // 0=off, 1=searching, 2=fix
} AlertAckPayload;

// Payload para mensaje BATTERY
typedef struct __attribute__((packed)) {
    uint8_t percentage;   // 0-100%
} BatteryPayload;
```

### 6.3 Funciones de Codificación/Decodificación

```cpp
// location_codec.cpp

#include "protocol.h"

// Codificar ubicación GPS en payload
void encodeLocation(LocationPayload* payload, 
                    double lat, double lon, 
                    float heading, 
                    uint8_t sats, 
                    bool hasFix, bool isValid, bool isMoving) {
    
    payload->latitude = (int32_t)(lat * 1000000.0);
    payload->longitude = (int32_t)(lon * 1000000.0);
    payload->heading = (uint16_t)heading;
    payload->satellites = sats;
    
    payload->flags = 0;
    if (hasFix)    payload->flags |= FLAG_FIX;
    if (isValid)   payload->flags |= FLAG_VALID;
    if (isMoving)  payload->flags |= FLAG_MOVING;
}

// Decodificar payload a valores legibles
void decodeLocation(const LocationPayload* payload,
                    double* lat, double* lon,
                    float* heading,
                    uint8_t* sats,
                    bool* hasFix, bool* isValid, bool* isMoving) {
    
    *lat = (double)payload->latitude / 1000000.0;
    *lon = (double)payload->longitude / 1000000.0;
    *heading = (float)payload->heading;
    *sats = payload->satellites;
    
    *hasFix = (payload->flags & FLAG_FIX) != 0;
    *isValid = (payload->flags & FLAG_VALID) != 0;
    *isMoving = (payload->flags & FLAG_MOVING) != 0;
}
```

### 6.4 Ejemplo de Paquete Completo

**Escenario:** Dispositivo B (0xB2) envía su ubicación al Dispositivo A (0xA1)

```
Ubicación: 4.710989, -74.072090 (Bogotá)
Heading: 45°
Satélites: 8
Flags: FIX=1, VALID=1, MOVING=0

Paquete (16 bytes):
┌──────┬──────┬──────┬──────┬──────────────────────────────────────────┐
│ 0xB2 │ 0xA1 │ 0x20 │ 0x05 │ Payload (12 bytes)                       │
├──────┼──────┼──────┼──────┼──────────────────────────────────────────┤
│ SRC  │ DST  │ TYPE │ SEQ  │ LAT(4) │ LON(4) │ HDG(2) │ SAT(1)│FLG(1)│
└──────┴──────┴──────┴──────┴────────┴────────┴────────┴───────┴──────┘

Valores hexadecimales:
B2 A1 20 05 | 3D D6 47 00 | 6E 51 95 FB | 00 2D | 08 | 03

Desglose del payload:
- Latitude:  0x0047D63D = 4,710989 → 4.710989°
- Longitude: 0xFB95516E = -74,072090 → -74.072090°
- Heading:   0x002D = 45°
- Satellites: 0x08 = 8
- Flags:     0x03 = 0b00000011 (FIX=1, VALID=1)
```

---

## 7. Máquina de Estados

### 7.1 Diagrama de Estados

```
                              ┌─────────────┐
                              │             │
                    ┌────────►│   STANDBY   │◄────────┐
                    │         │             │         │
                    │         └──────┬──────┘         │
                    │                │                │
                    │    Doble clic  │  Recibe        │
                    │    botón       │  ALERT         │
                    │                │                │
                    │         ┌──────▼──────┐         │
                    │         │             │         │
        Recibe      │    ┌────┤  ALERTING   ├────┐    │  Timeout
        CANCEL      │    │    │             │    │    │  o CANCEL
                    │    │    └─────────────┘    │    │
                    │    │                       │    │
                    │    │ Envía      Envía      │    │
                    │    │ ALERT      ALERT_ACK  │    │
                    │    │                       │    │
                    │    ▼                       ▼    │
                    │ ┌─────────┐         ┌─────────┐ │
                    │ │         │         │         │ │
                    └─┤ SEEKING │         │ SENDING ├─┘
                      │         │         │         │
                      │(Buscador)│         │(Buscado)│
                      └────┬────┘         └────┬────┘
                           │                   │
                           │    Intercambio    │
                           │◄─────────────────►│
                           │    LOCATION /     │
                           │    LOC_ACK        │
                           │                   │
```

### 7.2 Descripción de Estados

| Estado | Descripción | Acciones | Transiciones |
|--------|-------------|----------|--------------|
| **STANDBY** | Modo de espera, bajo consumo | PING periódico, escucha activa | → ALERTING (botón o ALERT recibido) |
| **ALERTING** | Transición, enviando/recibiendo alerta | Enviar ALERT o ALERT_ACK | → SEEKING / SENDING |
| **SEEKING** | Buscador activo, esperando ubicación | Mostrar distancia, recibir LOCATION | → STANDBY (CANCEL o timeout) |
| **SENDING** | Buscado activo, enviando ubicación | GPS activo, enviar LOCATION cada 5s | → STANDBY (CANCEL o timeout) |

### 7.3 Implementación de la Máquina de Estados

```cpp
// state_machine.h

typedef enum {
    STATE_STANDBY,
    STATE_ALERTING,
    STATE_SEEKING,
    STATE_SENDING
} SystemState;

class StateMachine {
private:
    SystemState currentState;
    unsigned long stateEnteredAt;
    
public:
    StateMachine() : currentState(STATE_STANDBY), stateEnteredAt(0) {}
    
    void setState(SystemState newState) {
        currentState = newState;
        stateEnteredAt = millis();
        onStateEnter(newState);
    }
    
    SystemState getState() { return currentState; }
    
    unsigned long getTimeInState() {
        return millis() - stateEnteredAt;
    }
    
    void onStateEnter(SystemState state) {
        switch(state) {
            case STATE_STANDBY:
                // Apagar GPS, reducir consumo
                gps_disable();
                display_showStandby();
                break;
                
            case STATE_ALERTING:
                // Mostrar alerta, preparar transición
                display_showAlert();
                buzzer_beep();
                break;
                
            case STATE_SEEKING:
                // Activar GPS propio para calcular distancia
                gps_enable();
                display_showSeeking();
                break;
                
            case STATE_SENDING:
                // Activar GPS, comenzar a enviar ubicación
                gps_enable();
                display_showSending();
                break;
        }
    }
};
```

---

## 8. Secuencias de Comunicación

### 8.1 Secuencia de Verificación de Enlace (PING/PONG)

```
    Dispositivo A                    Dispositivo B
         │                                │
         │  PING (seq=0x10)               │
         │───────────────────────────────►│
         │                                │
         │  PONG (seq=0x10, rssi=-65)     │
         │◄───────────────────────────────│
         │                                │
    [Enlace OK]                      [Enlace OK]
```

### 8.2 Secuencia de Búsqueda Completa

```
    Dispositivo A (SEEKER)           Dispositivo B (TARGET)
         │                                │
    [Usuario presiona                     │
     doble clic]                          │
         │                                │
         │  ALERT (seq=0x01)              │
         │───────────────────────────────►│
         │                                │ [Activa alerta]
         │                                │ [Enciende GPS]
         │                                │
         │  ALERT_ACK (seq=0x01, gps=0x01)│
         │◄───────────────────────────────│
         │                                │
    [Cambia a SEEKING]              [Cambia a SENDING]
         │                                │
         │                                │ [GPS obtiene fix]
         │                                │
         │  LOCATION (seq=0x02, coords)   │
         │◄───────────────────────────────│
         │                                │
    [Muestra distancia]                   │
         │                                │
         │  LOC_ACK (seq=0x02)            │
         │───────────────────────────────►│
         │                                │
         │        ... (cada 5 segundos)   │
         │                                │
    [Usuario presiona                     │
     doble clic para cancelar]            │
         │                                │
         │  CANCEL (seq=0x10)             │
         │───────────────────────────────►│
         │                                │
    [Vuelve a STANDBY]              [Vuelve a STANDBY]
```

### 8.3 Secuencia con Retransmisión

```
    Dispositivo A                    Dispositivo B
         │                                │
         │  ALERT (seq=0x01)              │
         │───────────────────────────────►│ [Perdido]
         │                                │
    [Timeout 2s]                          │
         │                                │
         │  ALERT (seq=0x01) [Retry 1]    │
         │───────────────────────────────►│
         │                                │
         │  ALERT_ACK (seq=0x01)          │
         │◄───────────────────────────────│
         │                                │
    [Éxito]                               │
```

---

## 9. Manejo de Errores

### 9.1 Timeouts

| Operación | Timeout | Reintentos | Acción tras fallo |
|-----------|---------|------------|-------------------|
| PING → PONG | 2 segundos | 3 | Marcar enlace como "débil" |
| ALERT → ALERT_ACK | 3 segundos | 3 | Mostrar "Sin respuesta" |
| LOCATION → LOC_ACK | 2 segundos | 2 | Reenviar LOCATION |

### 9.2 Backoff Exponencial

Para evitar colisiones en reintentos:

```cpp
// Cálculo de delay para reintento
uint32_t calculateBackoff(uint8_t retryCount) {
    // Base: 100ms, máximo: 1600ms
    uint32_t baseDelay = 100;
    uint32_t maxDelay = 1600;
    
    uint32_t delay = baseDelay * (1 << retryCount);  // 100, 200, 400, 800, 1600
    
    // Añadir jitter aleatorio (±25%)
    int32_t jitter = (random(50) - 25) * delay / 100;
    delay += jitter;
    
    return min(delay, maxDelay);
}
```

### 9.3 Códigos de Error

```cpp
typedef enum {
    ERR_NONE           = 0x00,
    ERR_TIMEOUT        = 0x01,
    ERR_NO_ACK         = 0x02,
    ERR_INVALID_PACKET = 0x03,
    ERR_WRONG_DEST     = 0x04,
    ERR_GPS_NO_FIX     = 0x10,
    ERR_LOW_BATTERY    = 0x20
} ErrorCode;
```

---

## 10. Configuración LoRa

### 10.1 Parámetros Recomendados

| Parámetro | Valor | Justificación |
|-----------|-------|---------------|
| Frecuencia | 915 MHz | Banda ISM Región 2 (Colombia) |
| Spreading Factor | SF9 | Balance alcance/ToA |
| Bandwidth | 125 kHz | Estándar, buena sensibilidad |
| Coding Rate | 4/5 | Mínima redundancia |
| Preamble Length | 8 símbolos | Estándar |
| Sync Word | 0x12 | Privado (diferente de LoRaWAN) |
| TX Power | 17 dBm | Máximo permitido en Colombia |

### 10.2 Cálculo de Time on Air

Para un paquete de 16 bytes con SF9/BW125/CR4/5:

```
ToA ≈ 164.9 ms
```

**Desglose:**
- Preamble: 8 + 4.25 = 12.25 símbolos
- Header: 8 símbolos (modo explícito)
- Payload: ~24 símbolos (16 bytes)
- Total: ~44 símbolos × 3.9 ms/símbolo ≈ 165 ms

### 10.3 Configuración en Código

```cpp
// lora_config.h

#define LORA_FREQUENCY     915E6    // 915 MHz
#define LORA_BANDWIDTH     125E3    // 125 kHz
#define LORA_SPREADING     9        // SF9
#define LORA_CODING_RATE   5        // 4/5
#define LORA_PREAMBLE_LEN  8
#define LORA_SYNC_WORD     0x12
#define LORA_TX_POWER      17       // dBm

void setupLoRa() {
    LoRa.setFrequency(LORA_FREQUENCY);
    LoRa.setSpreadingFactor(LORA_SPREADING);
    LoRa.setSignalBandwidth(LORA_BANDWIDTH);
    LoRa.setCodingRate4(LORA_CODING_RATE);
    LoRa.setPreambleLength(LORA_PREAMBLE_LEN);
    LoRa.setSyncWord(LORA_SYNC_WORD);
    LoRa.setTxPower(LORA_TX_POWER);
}
```

### 10.4 Tabla de SF vs Rendimiento (Referencia)

| SF | ToA (16 bytes) | Sensibilidad | Alcance Urbano Est. |
|----|----------------|--------------|---------------------|
| SF7 | 41 ms | -123 dBm | 200-300 m |
| SF8 | 72 ms | -126 dBm | 300-400 m |
| **SF9** | **165 ms** | **-129 dBm** | **400-600 m** |
| SF10 | 289 ms | -132 dBm | 600-800 m |
| SF11 | 659 ms | -134.5 dBm | 800-1200 m |
| SF12 | 1155 ms | -137 dBm | 1-2 km |

---

## 11. Implementación en C++

### 11.1 Archivo de Cabecera Principal

```cpp
// protocol.h

#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <stdint.h>

//=============================================================================
// CONFIGURACIÓN DE NODOS
//=============================================================================
#define NODE_ID_A         0xA1
#define NODE_ID_B         0xB2
#define NODE_ID_BROADCAST 0xFF

// Cambiar según el dispositivo
#define NODE_ID_SELF      NODE_ID_A
#define NODE_ID_PEER      NODE_ID_B

//=============================================================================
// TAMAÑOS
//=============================================================================
#define HEADER_SIZE       4
#define MAX_PAYLOAD       12
#define MAX_PACKET_SIZE   16

//=============================================================================
// TIPOS DE MENSAJE
//=============================================================================
typedef enum : uint8_t {
    MSG_PING      = 0x01,
    MSG_PONG      = 0x02,
    MSG_ALERT     = 0x10,
    MSG_ALERT_ACK = 0x11,
    MSG_LOCATION  = 0x20,
    MSG_LOC_ACK   = 0x21,
    MSG_CANCEL    = 0x30,
    MSG_BATTERY   = 0x40
} MessageType;

//=============================================================================
// FLAGS
//=============================================================================
#define FLAG_FIX    (1 << 0)
#define FLAG_VALID  (1 << 1)
#define FLAG_MOVING (1 << 2)

//=============================================================================
// ESTRUCTURAS
//=============================================================================

// Header del paquete (4 bytes)
typedef struct __attribute__((packed)) {
    uint8_t src_id;
    uint8_t dst_id;
    uint8_t msg_type;
    uint8_t seq_num;
} PacketHeader;

// Payload LOCATION (12 bytes)
typedef struct __attribute__((packed)) {
    int32_t  latitude;    // × 10^6
    int32_t  longitude;   // × 10^6
    uint16_t heading;     // 0-359°
    uint8_t  satellites;
    uint8_t  flags;
} LocationPayload;

// Payload PONG (2 bytes)
typedef struct __attribute__((packed)) {
    int16_t rssi;
} PongPayload;

// Payload ALERT_ACK (1 byte)
typedef struct __attribute__((packed)) {
    uint8_t gps_status;  // 0=off, 1=searching, 2=fix
} AlertAckPayload;

// Payload BATTERY (1 byte)
typedef struct __attribute__((packed)) {
    uint8_t percentage;
} BatteryPayload;

// Paquete completo
typedef struct __attribute__((packed)) {
    PacketHeader header;
    uint8_t payload[MAX_PAYLOAD];
} LoRaPacket;

//=============================================================================
// FUNCIONES DE UTILIDAD
//=============================================================================

// Codificar coordenadas GPS
inline int32_t encodeCoordinate(double coord) {
    return (int32_t)(coord * 1000000.0);
}

// Decodificar coordenadas GPS
inline double decodeCoordinate(int32_t encoded) {
    return (double)encoded / 1000000.0;
}

// Obtener tamaño de payload según tipo de mensaje
inline uint8_t getPayloadSize(MessageType type) {
    switch(type) {
        case MSG_LOCATION:  return sizeof(LocationPayload);
        case MSG_PONG:      return sizeof(PongPayload);
        case MSG_ALERT_ACK: return sizeof(AlertAckPayload);
        case MSG_BATTERY:   return sizeof(BatteryPayload);
        default:            return 0;
    }
}

#endif // PROTOCOL_H
```

### 11.2 Clase de Comunicación LoRa

```cpp
// lora_handler.h

#ifndef LORA_HANDLER_H
#define LORA_HANDLER_H

#include <LoRa.h>
#include "protocol.h"

class LoRaHandler {
private:
    uint8_t seqNum;
    uint8_t lastReceivedSeq;
    
public:
    LoRaHandler() : seqNum(0), lastReceivedSeq(255) {}
    
    // Inicializar LoRa
    bool begin();
    
    // Enviar paquete
    bool sendPacket(uint8_t destId, MessageType type, 
                    const void* payload = nullptr, uint8_t payloadLen = 0);
    
    // Recibir paquete (no bloqueante)
    bool receivePacket(LoRaPacket* packet, int16_t* rssi = nullptr);
    
    // Enviar y esperar ACK
    bool sendWithAck(uint8_t destId, MessageType type,
                     const void* payload, uint8_t payloadLen,
                     MessageType expectedAck, uint32_t timeout = 2000);
    
    // Métodos de conveniencia
    bool sendPing();
    bool sendAlert();
    bool sendLocation(double lat, double lon, uint16_t heading, 
                      uint8_t sats, uint8_t flags);
    bool sendCancel();
    
    // Obtener último RSSI
    int16_t getLastRssi();
};

#endif // LORA_HANDLER_H
```

---

## Anexo A: Diagrama de Estructura del Payload

```
╔═══════════════════════════════════════════════════════════════════════════════╗
║                    ESTRUCTURA COMPLETA DEL PAQUETE LoRa                       ║
╠═══════════════════════════════════════════════════════════════════════════════╣
║                                                                               ║
║  ┌─────────────────────────────────────────────────────────────────────────┐  ║
║  │                         PAQUETE (16 bytes máx)                          │  ║
║  ├─────────────────────────────┬───────────────────────────────────────────┤  ║
║  │        HEADER (4 bytes)     │           PAYLOAD (0-12 bytes)            │  ║
║  └─────────────────────────────┴───────────────────────────────────────────┘  ║
║                                                                               ║
║  ╔═════════════════════════════════════════════════════════════════════════╗  ║
║  ║                           HEADER (4 bytes)                              ║  ║
║  ╠═════════════════════════════════════════════════════════════════════════╣  ║
║  ║  Byte 0      Byte 1      Byte 2       Byte 3                            ║  ║
║  ║ ┌────────┐  ┌────────┐  ┌────────┐  ┌────────┐                          ║  ║
║  ║ │ SRC_ID │  │ DST_ID │  │MSG_TYPE│  │SEQ_NUM │                          ║  ║
║  ║ │ 0xA1   │  │ 0xB2   │  │ 0x20   │  │ 0x05   │                          ║  ║
║  ║ └────────┘  └────────┘  └────────┘  └────────┘                          ║  ║
║  ║     │           │           │           │                               ║  ║
║  ║     │           │           │           └─► Secuencia: 5                ║  ║
║  ║     │           │           └─► Tipo: LOCATION                          ║  ║
║  ║     │           └─► Destino: Device B                                   ║  ║
║  ║     └─► Origen: Device A                                                ║  ║
║  ╚═════════════════════════════════════════════════════════════════════════╝  ║
║                                                                               ║
║  ╔═════════════════════════════════════════════════════════════════════════╗  ║
║  ║                    PAYLOAD LOCATION (12 bytes)                          ║  ║
║  ╠═════════════════════════════════════════════════════════════════════════╣  ║
║  ║                                                                         ║  ║
║  ║  Bytes 0-3          Bytes 4-7         Bytes 8-9    Byte 10   Byte 11    ║  ║
║  ║ ┌──────────────┐  ┌──────────────┐  ┌──────────┐  ┌───────┐  ┌───────┐  ║  ║
║  ║ │   LATITUDE   │  │  LONGITUDE   │  │ HEADING  │  │ SATS  │  │ FLAGS │  ║  ║
║  ║ │   int32_t    │  │   int32_t    │  │ uint16_t │  │uint8_t│  │uint8_t│  ║  ║
║  ║ │  0x0047D63D  │  │  0xFB95516E  │  │  0x002D  │  │ 0x08  │  │ 0x03  │  ║  ║
║  ║ └──────────────┘  └──────────────┘  └──────────┘  └───────┘  └───────┘  ║  ║
║  ║        │                 │               │           │          │       ║  ║
║  ║        │                 │               │           │          │       ║  ║
║  ║        ▼                 ▼               ▼           ▼          ▼       ║  ║
║  ║   4.710989°        -74.072090°         45°          8        0b011     ║  ║
║  ║   (Bogotá)         (Bogotá)          (NE)       satélites   FIX+VALID  ║  ║
║  ║                                                                         ║  ║
║  ╚═════════════════════════════════════════════════════════════════════════╝  ║
║                                                                               ║
║  ╔═════════════════════════════════════════════════════════════════════════╗  ║
║  ║                         CAMPO FLAGS (detalle)                           ║  ║
║  ╠═════════════════════════════════════════════════════════════════════════╣  ║
║  ║                                                                         ║  ║
║  ║   Bit 7   Bit 6   Bit 5   Bit 4   Bit 3   Bit 2   Bit 1   Bit 0        ║  ║
║  ║  ┌─────┐ ┌─────┐ ┌─────┐ ┌─────┐ ┌─────┐ ┌─────┐ ┌─────┐ ┌─────┐       ║  ║
║  ║  │  0  │ │  0  │ │  0  │ │  0  │ │  0  │ │  0  │ │  1  │ │  1  │       ║  ║
║  ║  └─────┘ └─────┘ └─────┘ └─────┘ └─────┘ └─────┘ └─────┘ └─────┘       ║  ║
║  ║    RSV     RSV     RSV     RSV     RSV    MOVING  VALID    FIX         ║  ║
║  ║                                             │       │       │          ║  ║
║  ║                                             │       │       └─► GPS OK ║  ║
║  ║                                             │       └─► Coords válidas ║  ║
║  ║                                             └─► No en movimiento       ║  ║
║  ║                                                                         ║  ║
║  ╚═════════════════════════════════════════════════════════════════════════╝  ║
║                                                                               ║
╚═══════════════════════════════════════════════════════════════════════════════╝
```

---

## Anexo B: Cálculo de Distancia entre Coordenadas

Para mostrar la distancia al usuario, se utiliza la fórmula de Haversine:

```cpp
// distance.cpp

#include <math.h>

#define EARTH_RADIUS_M 6371000.0  // Radio de la Tierra en metros

// Calcular distancia entre dos puntos GPS (en metros)
double calculateDistance(double lat1, double lon1, double lat2, double lon2) {
    // Convertir a radianes
    double lat1_rad = lat1 * M_PI / 180.0;
    double lat2_rad = lat2 * M_PI / 180.0;
    double delta_lat = (lat2 - lat1) * M_PI / 180.0;
    double delta_lon = (lon2 - lon1) * M_PI / 180.0;
    
    // Fórmula de Haversine
    double a = sin(delta_lat / 2) * sin(delta_lat / 2) +
               cos(lat1_rad) * cos(lat2_rad) *
               sin(delta_lon / 2) * sin(delta_lon / 2);
    
    double c = 2 * atan2(sqrt(a), sqrt(1 - a));
    
    return EARTH_RADIUS_M * c;
}

// Calcular bearing (dirección) hacia el objetivo
double calculateBearing(double lat1, double lon1, double lat2, double lon2) {
    double lat1_rad = lat1 * M_PI / 180.0;
    double lat2_rad = lat2 * M_PI / 180.0;
    double delta_lon = (lon2 - lon1) * M_PI / 180.0;
    
    double x = sin(delta_lon) * cos(lat2_rad);
    double y = cos(lat1_rad) * sin(lat2_rad) -
               sin(lat1_rad) * cos(lat2_rad) * cos(delta_lon);
    
    double bearing = atan2(x, y) * 180.0 / M_PI;
    
    // Normalizar a 0-360
    return fmod((bearing + 360.0), 360.0);
}
```

---

## Historial de Revisiones

| Versión | Fecha | Autor | Cambios |
|---------|-------|-------|---------|
| 1.0 | Enero 2026 | J. Moreno | Versión inicial |

---

*Documento de especificación técnica para el proyecto de Trabajo de Grado*  
*Pontificia Universidad Javeriana — Maestría en Ingeniería del Internet de las Cosas*
