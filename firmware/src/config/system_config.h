#ifndef SYSTEM_CONFIG_H
#define SYSTEM_CONFIG_H

/* ── LoRa (SX1262) ────────────────────────────────────────────────────── */
#define LORA_FREQUENCY      915E6   /* Hz — Región Americas (cambiar a 868E6 para Europa) */
#define LORA_BANDWIDTH      125E3
#define LORA_SF             7       /* Spreading Factor 7-12 */
#define LORA_TX_POWER       14      /* dBm */

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

#endif /* SYSTEM_CONFIG_H */
