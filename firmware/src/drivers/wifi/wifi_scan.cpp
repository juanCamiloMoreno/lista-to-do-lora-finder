#include "wifi_scan.h"
#include <Arduino.h>
#include <WiFi.h>
#include <string.h>

/* Intervalo entre scans (ms) */
#define SCAN_INTERVAL_MS  3000u

static int16_t    _rssi        = 0;
static bool       _visible     = false;
static uint32_t   _last_scan   = 0;
static bool       _scan_active = false;

/* ── Target ──────────────────────────────────────────────────── */

void wifi_ap_start(void)
{
    WiFi.mode(WIFI_AP);
    WiFi.softAP(WIFI_AP_SSID, nullptr, WIFI_AP_CH, false, 4);
}

void wifi_ap_stop(void)
{
    WiFi.softAPdisconnect(true);
    WiFi.mode(WIFI_OFF);
}

/* ── Searcher ────────────────────────────────────────────────── */

void wifi_scan_start(void)
{
    _rssi        = 0;
    _visible     = false;
    _scan_active = false;
    _last_scan   = 0;
    WiFi.mode(WIFI_STA);
    WiFi.disconnect();
    /* Lanzar primer scan */
    WiFi.scanNetworks(/*async=*/true, /*show_hidden=*/false);
    _scan_active = true;
    _last_scan   = millis();
}

void wifi_scan_stop(void)
{
    WiFi.scanDelete();
    WiFi.disconnect(true);
    WiFi.mode(WIFI_OFF);
    _scan_active = false;
    _rssi        = 0;
    _visible     = false;
}

void wifi_scan_tick(void)
{
    if (!_scan_active) return;

    int n = WiFi.scanComplete();

    if (n == WIFI_SCAN_RUNNING) return;   /* todavía escaneando */

    if (n >= 0) {
        /* Scan terminado — buscar nuestro SSID */
        bool found = false;
        for (int i = 0; i < n; i++) {
            if (strcmp(WiFi.SSID(i).c_str(), WIFI_AP_SSID) == 0) {
                _rssi    = (int16_t)WiFi.RSSI(i);
                _visible = true;
                found    = true;
                break;
            }
        }
        if (!found) {
            _visible = false;
            /* Conservar último RSSI si ya se había visto */
        }
        WiFi.scanDelete();
    }

    /* Lanzar siguiente scan si pasó el intervalo */
    uint32_t now = millis();
    if (now - _last_scan >= SCAN_INTERVAL_MS) {
        WiFi.scanNetworks(/*async=*/true, /*show_hidden=*/false);
        _last_scan = now;
    }
}

int16_t wifi_scan_rssi(void)    { return _rssi;    }
bool    wifi_scan_visible(void) { return _visible; }

wifi_hot_t wifi_scan_hot_cold(void)
{
    if (!_visible || _rssi == 0) return WIFI_HOT_NONE;
    if (_rssi > -50)             return WIFI_HOT_MUY_CALIENTE;
    if (_rssi > -60)             return WIFI_HOT_CALIENTE;
    if (_rssi > -70)             return WIFI_HOT_TIBIO;
    if (_rssi > -80)             return WIFI_HOT_FRIO;
    return WIFI_HOT_MUY_FRIO;
}

const char* wifi_hot_label(wifi_hot_t h)
{
    switch (h) {
        case WIFI_HOT_MUY_CALIENTE: return "MUY CALIENTE";
        case WIFI_HOT_CALIENTE:     return "CALIENTE";
        case WIFI_HOT_TIBIO:        return "TIBIO";
        case WIFI_HOT_FRIO:         return "FRIO";
        case WIFI_HOT_MUY_FRIO:     return "MUY FRIO";
        default:                    return "Buscando...";
    }
}
