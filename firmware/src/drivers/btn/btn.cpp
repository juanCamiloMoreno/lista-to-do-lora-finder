#include "btn.h"
#include "config/pins.h"
#include <Arduino.h>

#define DEBOUNCE_MS   30
#define HOLD_MS      500

static const uint8_t _pins[BTN_COUNT] = { BTN_UP_PIN, BTN_DOWN_PIN, BTN_SELECT_PIN };

typedef struct {
    bool     last_raw;      /* Último nivel leído */
    bool     stable;        /* Estado estable post-debounce */
    bool     event;         /* Flanco detectado pendiente de consumir */
    uint32_t change_ms;     /* Momento del último cambio */
    uint32_t press_ms;      /* Momento en que se presionó (para hold) */
} btn_state_t;

static btn_state_t _state[BTN_COUNT];

void btn_init(void)
{
    for (int i = 0; i < BTN_COUNT; i++) {
        pinMode(_pins[i], INPUT_PULLUP);
        _state[i].last_raw  = true;   /* Pull-up → reposo = HIGH */
        _state[i].stable    = true;
        _state[i].event     = false;
        _state[i].change_ms = 0;
        _state[i].press_ms  = 0;
    }
}

/* Actualizar estado interno — llamar desde app_run() cada ciclo */
static void _btn_update(void)
{
    uint32_t now = millis();
    for (int i = 0; i < BTN_COUNT; i++) {
        bool raw = (digitalRead(_pins[i]) == HIGH);   /* true = suelto */

        if (raw != _state[i].last_raw) {
            _state[i].last_raw  = raw;
            _state[i].change_ms = now;
        }

        if ((now - _state[i].change_ms) >= DEBOUNCE_MS) {
            if (!raw && _state[i].stable) {
                /* Flanco de bajada estable → pulsación */
                _state[i].stable   = false;
                _state[i].event    = true;
                _state[i].press_ms = now;
            } else if (raw && !_state[i].stable) {
                _state[i].stable = true;
            }
        }
    }
}

bool btn_pressed(btn_id_t btn)
{
    _btn_update();
    if (_state[btn].event) {
        _state[btn].event = false;
        return true;
    }
    return false;
}

bool btn_held(btn_id_t btn)
{
    _btn_update();
    return !_state[btn].stable &&
           (millis() - _state[btn].press_ms) >= HOLD_MS;
}
