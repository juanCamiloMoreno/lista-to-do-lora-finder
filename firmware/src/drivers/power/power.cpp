#include "power.h"
#include "drivers/display/display.h"

#include <Arduino.h>

/* Sin actividad durante este tiempo → apagar pantalla */
#define STANDBY_TIMEOUT_MS  20000u   /* 20 segundos */

static uint32_t _last_activity_ms = 0;
static bool     _screen_off       = false;

void power_standby_init(void)
{
    _last_activity_ms = millis();
    if (_screen_off) {
        display_on();
        _screen_off = false;
    }
}

void power_standby_reset(void)
{
    _last_activity_ms = millis();
    if (_screen_off) {
        display_on();
        _screen_off = false;
    }
}

void power_standby_tick(void)
{
    if (!_screen_off && (millis() - _last_activity_ms >= STANDBY_TIMEOUT_MS)) {
        display_off();
        _screen_off = true;
    }
}

bool power_standby_active(void) { return _screen_off; }
