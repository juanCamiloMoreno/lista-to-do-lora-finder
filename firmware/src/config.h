/**
 * config.h — Configuración global del sistema LoRa Finder
 * 
 * Este archivo contiene todos los parámetros configurables del sistema:
 * pines de hardware, identificadores de nodo, clave de cifrado y
 * parámetros de comunicación LoRa.
 * 
 * IMPORTANTE: Para programar Nodo A o Nodo B, cambiar NODE_ID y PEER_NODE_ID
 * o usar build_flags en platformio.ini (-DNODE_ROLE_A / -DNODE_ROLE_B)
 */

#ifndef CONFIG_H
#define CONFIG_H

// ── Identificación de nodos ─────────────────────────────
// Estos IDs se queman en el firmware de cada dispositivo.
// Nodo A solo acepta paquetes de Nodo B y viceversa.
#ifdef NODE_ROLE_A
    #define NODE_ID         0x01
    #define PEER_NODE_ID    0x02
#elif defined(NODE_ROLE_B)
    #define NODE_ID         0x02
    #define PEER_NODE_ID    0x01
#else
    // Valores por defecto (configurar manualmente si no se usa build_flags)
    #define NODE_ID         0x01
    #define PEER_NODE_ID    0x02
#endif

// ── Clave de cifrado compartida (16 bytes) ──────────────
// Ambos nodos deben tener la misma clave.
// Cambiar antes de desplegar en campo.
#define CIPHER_KEY  { 0xA1, 0xB2, 0xC3, 0xD4, 0xE5, 0xF6, 0x17, 0x28, \
                      0x39, 0x4A, 0x5B, 0x6C, 0x7D, 0x8E, 0x9F, 0x00 }

// ── Parámetros LoRa ─────────────────────────────────────
#define LORA_FREQUENCY      915E6       // 915 MHz (banda ISM Américas)
#define LORA_SF_DEFAULT     7           // Spreading Factor inicial (7-12)
#define LORA_BW             125E3       // Bandwidth: 125 kHz
#define LORA_CR             5           // Coding Rate: 4/5
#define LORA_TX_POWER       22          // Potencia Tx: 22 dBm (máx SX1262)
#define LORA_SYNC_WORD      0x12        // Sync word (evitar colisión con LoRaWAN)

// ── Pines GPS (UART) ────────────────────────────────────
#define GPS_RX_PIN          46          // GPIO del ESP32 ← TX del GPS
#define GPS_TX_PIN          45          // GPIO del ESP32 → RX del GPS
#define GPS_BAUD            9600        // Velocidad del módulo NEO-M8N

// ── Pines brújula QMC5883L (I2C) ────────────────────────
// Usa el bus I2C por defecto del ESP32-S3
#define COMPASS_SDA         41          // GPIO SDA
#define COMPASS_SCL         42          // GPIO SCL
#define COMPASS_ADDR        0x0D        // Dirección I2C del QMC5883L

// ── Pines OLED SSD1306 (I2C) ────────────────────────────
// La Heltec V3 tiene la OLED en un bus I2C separado
#define OLED_SDA            17
#define OLED_SCL            18
#define OLED_RST            21
#define OLED_WIDTH          128
#define OLED_HEIGHT         64
#define OLED_ADDR           0x3C

// ── Pines de actuadores ─────────────────────────────────
#define BUZZER_PIN          47          // Buzzer activo
#define VIBRATOR_PIN        48          // Motor de vibración (vía MOSFET)

// ── Pin botón de usuario ────────────────────────────────
#define BUTTON_PIN          0           // Botón PRG de la Heltec V3

// ── Pin lectura de batería ──────────────────────────────
#define BATTERY_ADC_PIN     1           // ADC para divisor de voltaje de batería
#define BATTERY_LOW_PCT     20          // Umbral de batería baja (%)
#define BATTERY_CRITICAL_PCT 10         // Umbral crítico (%)

// ── Tiempos y ciclos ────────────────────────────────────
#define TX_INTERVAL_MS      5000        // Intervalo de transmisión en búsqueda activa
#define ACK_TIMEOUT_MS      3000        // Timeout para esperar ACK
#define LINK_LOST_MS        60000       // Tiempo sin paquetes para declarar enlace perdido
#define GPS_READ_INTERVAL   1000        // Lectura GPS cada 1 segundo
#define COMPASS_READ_INTERVAL 200       // Lectura brújula cada 200 ms
#define DISPLAY_REFRESH_MS  500         // Refresco de pantalla
#define SLEEP_LISTEN_MS     5000        // Ventana de escucha en modo sleep (Nodo B)
#define SLEEP_INTERVAL_MS   30000       // Intervalo entre ventanas de escucha

#endif // CONFIG_H
