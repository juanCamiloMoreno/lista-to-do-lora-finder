#include "btn.h"
#include "config/pins.h"
#include <Arduino.h>

/* ── Constantes ──────────────────────────────────────────────────────── */
#define DEBOUNCE_MS   30u
#define HOLD_MS      500u

static const uint8_t _pins[BTN_COUNT] = { BTN_UP_PIN, BTN_DOWN_PIN, BTN_SELECT_PIN };

/* ── Estado por botón ────────────────────────────────────────────────── */
/* El ISR captura el flanco de bajada y registra el timestamp.
 * btn_pressed() verifica que pasó el tiempo de debounce y consume el evento. */
static volatile uint32_t _fall_ms[BTN_COUNT]   = {0, 0, 0};
static volatile bool     _fall_flag[BTN_COUNT] = {false, false, false};

/* ── ISRs (uno por botón) ────────────────────────────────────────────── */
static void IRAM_ATTR _isr_up(void)
{
    uint32_t t = millis();
    if (t - _fall_ms[BTN_UP] > DEBOUNCE_MS) {
        _fall_ms[BTN_UP]   = t;
        _fall_flag[BTN_UP] = true;
    }
}
static void IRAM_ATTR _isr_down(void)
{
    uint32_t t = millis();
    if (t - _fall_ms[BTN_DOWN] > DEBOUNCE_MS) {
        _fall_ms[BTN_DOWN]   = t;
        _fall_flag[BTN_DOWN] = true;
    }
}
static void IRAM_ATTR _isr_sel(void)
{
    uint32_t t = millis();
    if (t - _fall_ms[BTN_SELECT] > DEBOUNCE_MS) {
        _fall_ms[BTN_SELECT]   = t;
        _fall_flag[BTN_SELECT] = true;
    }
}

/* ── API pública ─────────────────────────────────────────────────────── */

void btn_init(void)
{
    for (int i = 0; i < BTN_COUNT; i++) {
        pinMode(_pins[i], INPUT_PULLUP);
        _fall_ms[i]   = 0;
        _fall_flag[i] = false;
    }
    attachInterrupt(digitalPinToInterrupt(_pins[BTN_UP]),     _isr_up,   FALLING);
    attachInterrupt(digitalPinToInterrupt(_pins[BTN_DOWN]),   _isr_down, FALLING);
    attachInterrupt(digitalPinToInterrupt(_pins[BTN_SELECT]), _isr_sel,  FALLING);
}

bool btn_pressed(btn_id_t btn)
{
    if (_fall_flag[btn]) {
        _fall_flag[btn] = false;   /* consumir — una sola vez por pulsación */
        return true;
    }
    return false;
}

bool btn_held(btn_id_t btn)
{
    return (digitalRead(_pins[btn]) == LOW) &&
           (millis() - _fall_ms[btn]) >= HOLD_MS;
}
