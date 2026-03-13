#ifndef BATTERY_H
#define BATTERY_H

#include <stdbool.h>
#include <stdint.h>

/* ─────────────────────────────────────────────────────────────────────────
 * Driver de batería — Heltec WiFi LoRa 32 V4
 *
 * Mide voltaje via ADC (GPIO1, divisor 1:2) y detecta carga via
 * el pin CHRG del TP4054 (GPIO7, LOW = cargando).
 * ───────────────────────────────────────────────────────────────────────── */

typedef enum {
    BAT_STATUS_DISCHARGING = 0, /* Descargando / en uso */
    BAT_STATUS_CHARGING    = 1, /* Conectado a cargador */
    BAT_STATUS_FULL        = 2, /* Completamente cargado */
} battery_status_t;

typedef struct {
    float             voltage;    /* Voltaje real de la celda (V), ej. 3.75 */
    uint8_t           percent;    /* Nivel de carga 0–100 % */
    battery_status_t  status;     /* Descargando / cargando / lleno */
} battery_data_t;

/* Inicializa el ADC y el pin de detección de carga */
void battery_init(void);

/* Lee voltaje, calcula % y estado — llama esto desde app_run() */
void battery_read(battery_data_t *out);

/*
 * Dibuja un ícono de batería en la pantalla.
 *
 *  x, y   → esquina superior-izquierda del ícono
 *  data   → resultado de battery_read()
 *
 * Dimensiones: 26 × 10 px (cuerpo 22×10 + nub 2×4 + texto %)
 *
 *  ┌──────────────────────┐▐
 *  │ ████████░░░░░░░░░░░░ │▐   74%
 *  └──────────────────────┘▐
 */
void battery_draw_icon(int x, int y, const battery_data_t *data);

#endif /* BATTERY_H */
