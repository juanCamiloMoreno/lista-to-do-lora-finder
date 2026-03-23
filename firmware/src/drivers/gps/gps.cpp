#include "gps.h"
#include "../../config/pins.h"
#include "../../config/system_config.h"
#include "../led/led.h"

#include <Arduino.h>
#include <TinyGPSPlus.h>

static TinyGPSPlus      _gps;
static HardwareSerial   _serial(1);   /* UART1 */
static gps_data_t       _data = {};
static bool             _enabled = false;
static uint32_t         _last_log_ms = 0;

/* ── LED de estado GPS ────────────────────────────────────────────────── */
#define GPS_LED_DIM_R    40
#define GPS_LED_DIM_G     0
#define GPS_LED_DIM_B    40   /* morado tenue */
#define GPS_LED_AMB_R   200
#define GPS_LED_AMB_G   100
#define GPS_LED_AMB_B     0   /* ámbar */
#define GPS_LED_FIX_R   128
#define GPS_LED_FIX_G     0
#define GPS_LED_FIX_B   128  /* morado brillante */

static uint32_t _led_ms    = 0;
static bool     _led_amber = false;

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
    _led_amber = false;
    _led_ms    = 0;
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

    uint32_t now = millis();

    /* ── LED de estado ───────────────────────────────────────────────── */
    if (_data.valid) {
        /* Fix: morado brillante, destello en cada update de posición */
        if (_gps.location.isUpdated())
            led_set_color(GPS_LED_FIX_R, GPS_LED_FIX_G, GPS_LED_FIX_B);
    } else if (_gps.charsProcessed() == 0) {
        /* Sin bytes del módulo: morado tenue fijo */
        led_set_color(GPS_LED_DIM_R, GPS_LED_DIM_G, GPS_LED_DIM_B);
    } else {
        /* Datos llegando pero sin fix: ámbar ↔ morado tenue cada 1 s */
        if (now - _led_ms >= 1000UL) {
            _led_ms   = now;
            _led_amber = !_led_amber;
            if (_led_amber)
                led_set_color(GPS_LED_AMB_R, GPS_LED_AMB_G, GPS_LED_AMB_B);
            else
                led_set_color(GPS_LED_DIM_R, GPS_LED_DIM_G, GPS_LED_DIM_B);
        }
    }

    /* ── Reporte serial cada 5 s ─────────────────────────────────────── */
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
