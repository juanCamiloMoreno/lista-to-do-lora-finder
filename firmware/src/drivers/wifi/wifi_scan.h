#pragma once
#include <stdint.h>
#include <stdbool.h>

/* SSID que el nodo objetivo (target) anuncia como soft-AP */
#define WIFI_AP_SSID  "LoRaFinder"
#define WIFI_AP_CH    6

typedef enum {
    WIFI_HOT_NONE = 0,        /* sin datos aún      */
    WIFI_HOT_MUY_FRIO,        /* < -80 dBm          */
    WIFI_HOT_FRIO,            /* -80 a -70 dBm      */
    WIFI_HOT_TIBIO,           /* -70 a -60 dBm      */
    WIFI_HOT_CALIENTE,        /* -60 a -50 dBm      */
    WIFI_HOT_MUY_CALIENTE,    /* > -50 dBm          */
} wifi_hot_t;

/* ── Target (nodo objetivo) ─────────────────────────────────────── */
void wifi_ap_start(void);   /* Crear soft-AP con WIFI_AP_SSID      */
void wifi_ap_stop(void);    /* Apagar soft-AP                       */

/* ── Searcher (nodo buscador) ───────────────────────────────────── */
void    wifi_scan_start(void);     /* Iniciar escaneo asíncrono continuo */
void    wifi_scan_stop(void);      /* Detener escaneo y apagar WiFi      */
void    wifi_scan_tick(void);      /* Llamar en cada loop — no bloqueante */
int16_t wifi_scan_rssi(void);      /* Último RSSI del target (0 = no visto) */
bool    wifi_scan_visible(void);   /* Target visible en último scan      */
wifi_hot_t  wifi_scan_hot_cold(void);       /* Nivel caliente/frío        */
const char* wifi_hot_label(wifi_hot_t h);   /* Texto del nivel            */
