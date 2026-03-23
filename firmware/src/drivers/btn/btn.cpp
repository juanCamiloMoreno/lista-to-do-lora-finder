#include "btn.h"
#include "config/pins.h"
#include <Arduino.h>

/* ── Constantes ──────────────────────────────────────────────────────── */
#define DEBOUNCE_MS   100u
#define HOLD_MS      500u

static const uint8_t _pins[BTN_COUNT] = { BTN_UP_PIN, BTN_DOWN_PIN, BTN_SELECT_PIN };

/* ── Estado por botón ────────────────────────────────────────────────── */
/* El ISR captura el flanco de bajada y registra el timestamp.
 * btn_pressed() verifica que pasó el tiempo de debounce y consume el evento. */
static volatile uint32_t _fall_ms[BTN_COUNT]   = {0, 0, 0};
static volatile bool     _fall_flag[BTN_COUNT] = {false, false, false};

/* Estado anterior del pin para detección de flanco por polling */
static bool _prev_low[BTN_COUNT] = {false, false, false};

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
        _prev_low[i]  = false;
    }
    /* GPIO 2 (BTN_UP) tiene problemas de ISR en el Heltec V4 (ESP32-S3):
     * el interrupt nunca dispara confiablemente — se usa solo polling via btn_update(). */
    attachInterrupt(digitalPinToInterrupt(_pins[BTN_DOWN]),   _isr_down, FALLING);
    attachInterrupt(digitalPinToInterrupt(_pins[BTN_SELECT]), _isr_sel,  FALLING);
}

/* ── Fallback polling ────────────────────────────────────────────────── */
/* Detecta el flanco de bajada por muestreo. Si el ISR ya disparó y
 * actualizó _fall_ms, el chequeo de debounce evita el doble disparo. */
void btn_update(void)
{
    uint32_t t = millis();
    for (int i = 0; i < BTN_COUNT; i++) {
        bool cur_low = (digitalRead(_pins[i]) == LOW);
        if (cur_low && !_prev_low[i]) {          /* flanco detectado */
            if (t - _fall_ms[i] > DEBOUNCE_MS) { /* ISR no lo capturó */
                _fall_ms[i]   = t;
                _fall_flag[i] = true;
            }
        }
        _prev_low[i] = cur_low;
    }
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

bool btn_raw_low(btn_id_t btn)
{
    return digitalRead(_pins[btn]) == LOW;
}

void btn_flush(void)
{
    for (int i = 0; i < BTN_COUNT; i++)
        _fall_flag[i] = false;
}
