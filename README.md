# LoRa Finder — Sistema de Localización P2P para Eventos Masivos

Sistema de localización punto a punto basado en **LoRa**, diseñado para entornos urbanos de alta afluencia (conciertos, festivales, eventos masivos) donde la conectividad celular puede estar saturada o no disponible.

---

## Descripción del Sistema

Dos nodos portátiles se comunican directamente entre sí mediante LoRa P2P (sin gateway ni infraestructura):

| Nodo | Rol | Descripción |
|------|-----|-------------|
| **Nodo A** | Buscador | Porta el rescatista. Muestra dirección, distancia e intensidad de señal hacia el Nodo B |
| **Nodo B** | Buscado | Porta la persona a localizar. Transmite su posición GPS al ser activado |

---

## Hardware

| Componente | Modelo | Interfaz | Función |
|------------|--------|----------|---------|
| MCU + LoRa + OLED | Heltec WiFi LoRa 32 V3 (ESP32-S3 + SX1262) | — | Procesamiento, comunicación, display |
| GPS | u-blox NEO-M8N | UART | Posición geográfica |
| Brújula | QMC5883L (módulo GY-271) | I2C | Orientación magnética |
| Buzzer | Activo 3.3V | GPIO | Alertas sonoras |
| Motor vibración | Coin 3V | GPIO + MOSFET | Alertas hápticas |
| Batería | Li-Po 3.7V 1000mAh | JST 1.25mm | Alimentación |

---

## Estructura del Repositorio

```
lora-finder/
│
├── README.md                       ← Este archivo
├── LICENSE
├── .gitignore
│
├── firmware/                       ← Proyecto PlatformIO (ESP32-S3)
│   ├── platformio.ini              ← Configuración de compilación y dependencias
│   ├── src/
│   │   ├── main.cpp                ← Entry point, máquina de estados principal
│   │   ├── config.h                ← Pines, constantes, IDs de nodo, clave de cifrado
│   │   ├── communication/          ← LoRa P2P: envío, recepción, ACK, SF dinámico
│   │   ├── gps/                    ← Lectura NEO-M8N, parsing NMEA, Haversine
│   │   ├── compass/                ← QMC5883L: calibración, rumbo magnético
│   │   ├── ui/                     ← OLED SSD1306: vistas por estado, buzzer, vibración
│   │   ├── energy/                 ← Batería, deep sleep, degradación gradual
│   │   └── protocol/               ← Payload binario, serialización, cifrado
│   ├── include/
│   ├── lib/
│   └── test/
│
├── docs/                           ← Documentación técnica
│   ├── requirements/               ← Requerimientos funcionales y no funcionales
│   ├── use_cases/                  ← Casos de uso con ramificaciones de falla
│   ├── architecture/               ← Diagramas de bloques, FSM, flujo de mensajes
│   ├── protocol/                   ← Especificación del protocolo P2P
│   ├── planning/                   ← Plan de acción y cronograma
│   └── references/                 ← Referencia del driver SX126x, docs auxiliares
│
├── hardware/                       ← Documentación de hardware
│   ├── schematics/                 ← Esquemáticos de conexión
│   ├── datasheets/                 ← Hojas de datos
│   └── bom/                        ← Lista de materiales
│
├── tools/                          ← Scripts auxiliares (calculadora ToA, etc.)
└── assets/                         ← Imágenes, diagramas exportados
```

---

## Protocolo de Comunicación P2P

Documentación completa en [`docs/protocol/`](docs/protocol/).

### Tipos de mensajes

| Tipo | Código | Descripción |
|------|--------|-------------|
| PING | 0x01 | Verificación de enlace |
| PONG | 0x02 | Respuesta a PING con RSSI |
| ALERT | 0x10 | Solicitud de activación / búsqueda |
| ACK | 0x11 | Confirmación |
| LOCATION | 0x20 | Coordenadas GPS + estado |

### Estructura del paquete

```
┌──────────┬──────────┬──────────┬──────────┬─────────────────┐
│ Src ID   │ Dst ID   │ Msg Type │ Seq Num  │ Payload         │
│ (1 byte) │ (1 byte) │ (1 byte) │ (1 byte) │ (0–12 bytes)    │
└──────────┴──────────┴──────────┴──────────┴─────────────────┘
```

### Codificación GPS (8 bytes)

```cpp
int32_t lat_scaled = (int32_t)(latitude  * 1e6);  // 4 bytes
int32_t lon_scaled = (int32_t)(longitude * 1e6);  // 4 bytes
// Total paquete LOCATION: 4 (header) + 8 (GPS) = 12 bytes
```

---

## Máquina de Estados

| Estado | Nodo A (Buscador) | Nodo B (Buscado) |
|--------|-------------------|------------------|
| `IDLE` | Esperando acción del usuario | Deep sleep con despertar periódico |
| `ACTIVATING` | Enviando comando de activación | Recibiendo y confirmando |
| `SEARCHING` | Búsqueda activa: GPS + brújula + OLED | Transmitiendo posición |
| `PROXIMITY` | Alerta de proximidad | Mayor frecuencia de Tx |
| `LINK_LOST` | Última posición + RSSI como guía | Balizas autónomas |
| `LOW_BATTERY` | Reducción de servicios | Baliza mínima |

---

## Configuración del Entorno

### Requisitos

- [PlatformIO](https://platformio.org/) (CLI o extensión VS Code)
- Driver USB CP2102 / CH9102 (según versión de la Heltec)

### Compilación y carga

```bash
cd firmware

# Compilar
pio run

# Cargar al dispositivo
pio run --target upload

# Monitor serial
pio device monitor --baud 115200
```

### Configuración de nodos

Editar `firmware/src/config.h`:

```cpp
#define NODE_ID         0x01        // 0x01 = Nodo A, 0x02 = Nodo B
#define PEER_NODE_ID    0x02        // ID del nodo emparejado
#define CIPHER_KEY      { 0xAB, 0xCD, 0x12, 0x34, ... }  // Clave compartida
#define LORA_FREQUENCY  915E6       // 915 MHz (región Américas)
#define LORA_SF_DEFAULT 7           // Spreading Factor inicial
```

---

## Métricas de Validación

| Métrica | Objetivo mínimo | Objetivo deseado | Método |
|---------|-----------------|------------------|--------|
| Alcance (urbano abierto) | 300 m | 500 m | GPS + conteo paquetes |
| Alcance (alta densidad) | 200 m | 300 m | GPS + conteo paquetes |
| Tasa de entrega (PDR) | > 80% | > 95% | Paquetes RX / TX |
| Precisión GPS | < 10 m | < 5 m | Comparación referencia |
| Autonomía standby (Nodo B) | > 3 h | > 8 h | Medición directa |
| Autonomía búsqueda (Nodo A) | > 3 h | > 4 h | Medición directa |

---

## Documentación

| Documento | Ubicación |
|-----------|-----------|
| Requerimientos del sistema | [`docs/requirements/`](docs/requirements/) |
| Casos de uso | [`docs/use_cases/`](docs/use_cases/) |
| Especificación protocolo P2P | [`docs/protocol/`](docs/protocol/) |
| Plan de acción y cronograma | [`docs/planning/`](docs/planning/) |
| Referencia driver SX126x | [`docs/references/`](docs/references/) |
| Lista de materiales (BOM) | [`hardware/bom/`](hardware/bom/) |

---

## Contexto Académico

| | |
|---|---|
| **Universidad** | Pontificia Universidad Javeriana — Bogotá |
| **Programa** | Maestría en Ingeniería del Internet de las Cosas |
| **Asignatura** | Trabajo de Grado I-II (IoT) |
| **Estudiante** | Juan Camilo Moreno Zornosa |
| **Director** | Ing. Gustavo Adolfo Ramírez Espinosa, PhD |
| **Periodo** | 2601 (2026-I) |

---

## Licencia

Proyecto académico — Todos los derechos reservados.
