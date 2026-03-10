#include "gps.h"
#include "../../config/pins.h"
#include "../../config/system_config.h"

#include <Arduino.h>
#include <TinyGPSPlus.h>

static TinyGPSPlus      _gps;
static HardwareSerial   _serial(1);   /* UART1 */
static gps_data_t       _data = {};

/* ── Ciclo de vida ────────────────────────────────────────────────────── */

bool gps_init(void)
{
    _serial.begin(GPS_BAUD, SERIAL_8N1, GPS_RX_PIN, GPS_TX_PIN);
    return true;
}

/* ── Actualización (llamar en cada loop) ─────────────────────────────── */

bool gps_update(void)
{
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

    return _data.valid;
}

/* ── Acceso a datos ───────────────────────────────────────────────────── */

gps_data_t gps_get_data(void)
{
    return _data;
}
