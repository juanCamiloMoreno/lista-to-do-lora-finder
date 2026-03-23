#pragma once
#include <stdint.h>
#include <stdbool.h>

/* ─────────────────────────────────────────────────────────────────────────
 * Módulo de configuración persistente via NVS (Preferences.h — ESP32).
 * Los valores se guardan con nvs_config_save() y se recuperan al arranque
 * con nvs_config_init(). Si no hay datos guardados, se usan los defaults
 * definidos en system_config.h. (C-11)
 * ───────────────────────────────────────────────────────────────────────── */

typedef struct {
    int16_t rssi_threshold;    /* dBm — umbral RSSI para "encontrado"      */
    uint8_t rssi_avg_window;   /* muestras para media móvil RSSI           */
    uint8_t rssi_confirm;      /* confirmaciones consecutivas requeridas    */
    int8_t  tx_power;          /* potencia TX en dBm (2–22)                */
    uint8_t spreading_factor;  /* Spreading Factor por defecto (7–12)      */
} nvs_config_t;

/* Carga configuración desde NVS (con defaults de system_config.h) */
void          nvs_config_init(void);

/* Persiste la configuración actual en NVS */
void          nvs_config_save(void);

/* Puntero a la configuración en memoria (modificar y luego llamar save) */
nvs_config_t *nvs_config_get(void);
