#include "gps.h"
#include "../../config/pins.h"
#include "../../config/system_config.h"

#include <Arduino.h>
#include <TinyGPSPlus.h>

static TinyGPSPlus      _gps;
static HardwareSerial   _serial(1);   /* UART1 */
static gps_data_t       _data = {};
static bool             _enabled = false;
static uint32_t         _last_log_ms = 0;

/* ── Debug GPS ────────────────────────────────────────────────────────── */

/* ── Control de enable (C-06) ─────────────────────────────────────────── */

void gps_enable(void)
{
    digitalWrite(GPS_EN_PIN, LOW);   /* active LOW — LOW = módulo encendido */
    _enabled = true;
}

void gps_disable(void)
{
    digitalWrite(GPS_EN_PIN, HIGH);  /* HIGH = módulo apagado */
    _enabled = false;
}

bool gps_is_enabled(void)
{
    return _enabled;
}

/* ── Ciclo de vida ────────────────────────────────────────────────────── */

bool gps_init(void)
{
    pinMode(GPS_EN_PIN, OUTPUT);
    gps_disable();   /* apagado por defecto — se activa en modo búsqueda o test GPS */

    _serial.begin(GPS_BAUD, SERIAL_8N1, GPS_RX_PIN, GPS_TX_PIN);

#if DEBUG_LOG
    Serial.printf("[GPS] init — RX=GPIO%d TX=GPIO%d EN=GPIO%d baud=%d\n",
                  GPS_RX_PIN, GPS_TX_PIN, GPS_EN_PIN, GPS_BAUD);
#endif

    return true;
}

/* ── Actualización (llamar en cada loop) ─────────────────────────────── */

bool gps_update(void)
{
    if (!_enabled) return false;

    while (_serial.available() > 0)
        _gps.encode(_serial.read());

    if (_gps.location.isValid()) {
        _data.latitude   = _gps.location.lat();
        _data.longitude  = _gps.location.lng();
        _data.altitude   = _gps.altitude.meters();
        _data.speed      = (float)_gps.speed.kmph();
        _data.satellites = (uint8_t)_gps.satellites.value();
        _data.valid      = true;
    } else {
        _data.satellites = (uint8_t)_gps.satellites.value();
        _data.valid      = false;
    }

    /* Reporte cada 5 s — siempre visible */
    uint32_t now = millis();
    if (now - _last_log_ms >= 5000UL) {
        _last_log_ms = now;
        if (_data.valid)
            Serial.printf("[GPS] Lat:%.6f  Lon:%.6f  Sat:%d\n",
                          _data.latitude, _data.longitude, _data.satellites);
        else
            Serial.printf("[GPS] Sin fix  Sat:%d  bytes:%lu\n",
                          _data.satellites, (unsigned long)_gps.charsProcessed());
    }


    return _data.valid;
}

/* ── Acceso a datos ───────────────────────────────────────────────────── */

gps_data_t gps_get_data(void)
{
    return _data;
}
