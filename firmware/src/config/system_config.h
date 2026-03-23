#ifndef SYSTEM_CONFIG_H
#define SYSTEM_CONFIG_H

/* ── Debug Log ─────────────────────────────────────────────────────────
 * DEBUG_LOG 1 → imprime [TX] / [RX] en Serial a 115200.
 * Cambiar a 0 para deshabilitar en producción y reducir overhead.
 * ─────────────────────────────────────────────────────────────────────── */
#define DEBUG_LOG           0

/* ── LoRa (SX1262) ────────────────────────────────────────────────────── */
#define LORA_FREQUENCY      915E6   /* Hz — Región Americas (cambiar a 868E6 para Europa) */
#define LORA_BANDWIDTH      125E3
#define LORA_SF             7       /* Spreading Factor 7-12 */
#define LORA_TX_POWER       22      /* dBm — máximo SX1262 */

/* ── GPS ──────────────────────────────────────────────────────────────── */
#define GPS_BAUD            9600

/* ── Display SSD1306 128x64 ───────────────────────────────────────────── */
#define DISPLAY_WIDTH       128
#define DISPLAY_HEIGHT      64
/* Posición del círculo de la brújula en pantalla */
#define COMPASS_UI_CX       96      /* Centro X (derecha) */
#define COMPASS_UI_CY       32      /* Centro Y */
#define COMPASS_UI_RADIUS   30      /* Radio en píxeles */

/* ── Compás QMC5883L ──────────────────────────────────────────────────── */
#define COMPASS_I2C_ADDR    0x0D    /* Dirección I2C del QMC5883L */
#define I2C_CLOCK_HZ        400000  /* 400 kHz Fast Mode */
/* Filtro EMA sobre heading: 0.0 = máximo suavizado, 1.0 = sin filtro */
#define COMPASS_FILTER_ALPHA    0.20f
/* Rechazo de outliers: muestras que salten más de este ángulo se descartan */
#define COMPASS_OUTLIER_DEG    90.0f
/* Duración del modo calibración en milisegundos */
#define COMPASS_CALIB_MS      15000

/* ── FSM — Temporización ──────────────────────────────────────────────── */
#define SEARCH_RETRY_MS       3000    /* ms entre reintentos SEARCH_START   */
#define SEARCH_MAX_RETRIES    5       /* intentos antes de timeout          */
#define STATUS_INTERVAL_MS    2000    /* ms entre STATUS/BEACON periódicos  */
#define SIGNAL_LOST_MS        15000   /* ms sin paquete → señal perdida     */
#define SIGNAL_GIVEUP_MS      60000   /* ms en señal perdida → rendirse     */
#define ALERT_ACK_TIMEOUT_MS  60000   /* ms máx para que objetivo confirme  */
#define SOS_INTERVAL_MS       1000    /* ms entre mensajes SOS_ALERT        */
#define REUNITED_SHOW_MS      4000    /* ms mostrando pantalla REUNIDOS     */

/* ── FSM — Umbrales de reunión ────────────────────────────────────────── */
#define REUNITE_DIST_M        20.0f   /* metros → considerar reunión        */
#define REUNITE_RSSI_DBM      (-60)   /* dBm (sin GPS) → considerar reunión */

/* ── FSM — Suavizado RSSI para detección "encontrado" (C-09) ──────────
 * Media móvil + contador de confirmaciones + histéresis para evitar
 * falsas detecciones con un solo paquete fuerte (H-02).
 * Estos valores son defaults; se pueden sobreescribir desde NVS (C-11).
 * ─────────────────────────────────────────────────────────────────────── */
#define RSSI_AVG_WINDOW       10    /* muestras para la media móvil         */
#define RSSI_CONFIRM_COUNT    5     /* confirmaciones consecutivas requeridas */
#define RSSI_THRESHOLD_FOUND  (-40) /* dBm — umbral para entrar "encontrado" */
#define RSSI_HYSTERESIS       15    /* dBm — umbral salida = THR - HIST      */

/* ── Compás en pantalla de navegación ────────────────────────────────── */
#define NAV_ARROW_CX          100
#define NAV_ARROW_CY          35
#define NAV_ARROW_LEN         24

#endif /* SYSTEM_CONFIG_H */
