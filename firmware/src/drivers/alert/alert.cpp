#include "alert.h"
#include "config/pins.h"
#include <Arduino.h>

/* ── Patrón de alerta: lista de segmentos ON/OFF ─────────────────────── */
typedef struct {
    uint16_t buzz_on_ms;   /* 0 = buzzer apagado en este segmento */
    uint16_t vib_on_ms;    /* 0 = motor apagado en este segmento  */
    uint16_t total_ms;     /* Duración total del segmento          */
} alert_seg_t;

#define MAX_SEGS 6

static alert_seg_t _segs[MAX_SEGS];
static int         _n_segs   = 0;
static int         _cur_seg  = 0;
static uint32_t    _seg_start = 0;
static bool        _active   = false;

/* ── Helpers internos ─────────────────────────────────────────────────── */

static void _start_seg(int idx)
{
    _cur_seg   = idx;
    _seg_start = millis();
    digitalWrite(BUZZER_PIN,    _segs[idx].buzz_on_ms > 0 ? HIGH : LOW);
    digitalWrite(MOTOR_VIB_PIN, _segs[idx].vib_on_ms  > 0 ? HIGH : LOW);
}

static void _stop_all(void)
{
    digitalWrite(BUZZER_PIN,    LOW);
    digitalWrite(MOTOR_VIB_PIN, LOW);
    _active = false;
}

static void _queue(const alert_seg_t *segs, int n)
{
    if (n > MAX_SEGS) n = MAX_SEGS;
    memcpy(_segs, segs, n * sizeof(alert_seg_t));
    _n_segs = n;
    _active = true;
    _start_seg(0);
}

/* ── API pública ──────────────────────────────────────────────────────── */

void alert_init(void)
{
    pinMode(BUZZER_PIN,    OUTPUT);
    pinMode(MOTOR_VIB_PIN, OUTPUT);
    digitalWrite(BUZZER_PIN,    LOW);
    digitalWrite(MOTOR_VIB_PIN, LOW);
}

void alert_tick(void)
{
    if (!_active) return;

    uint32_t elapsed = millis() - _seg_start;
    alert_seg_t &s = _segs[_cur_seg];

    /* Apagar buzzer cuando expira su tiempo ON dentro del segmento */
    if (s.buzz_on_ms > 0 && elapsed >= s.buzz_on_ms)
        digitalWrite(BUZZER_PIN, LOW);

    /* Apagar vibrador cuando expira su tiempo ON */
    if (s.vib_on_ms > 0 && elapsed >= s.vib_on_ms)
        digitalWrite(MOTOR_VIB_PIN, LOW);

    /* Avanzar al siguiente segmento */
    if (elapsed >= s.total_ms) {
        int next = _cur_seg + 1;
        if (next < _n_segs) {
            _start_seg(next);
        } else {
            _stop_all();
        }
    }
}

/* ── Definiciones de patrones ─────────────────────────────────────────── */

void alert_beep_short(void) {
    static const alert_seg_t p[] = { {80, 0, 80} };
    _queue(p, 1);
}

void alert_beep_long(void) {
    static const alert_seg_t p[] = { {400, 0, 400} };
    _queue(p, 1);
}

void alert_beep_double(void) {
    static const alert_seg_t p[] = {
        {80,  0, 80},
        {0,   0, 80},   /* pausa */
        {80,  0, 80},
    };
    _queue(p, 3);
}

void alert_vib_short(void) {
    static const alert_seg_t p[] = { {0, 100, 100} };
    _queue(p, 1);
}

void alert_vib_long(void) {
    static const alert_seg_t p[] = { {0, 400, 400} };
    _queue(p, 1);
}

void alert_rx(void) {
    /* Vibración corta + pitido suave */
    static const alert_seg_t p[] = { {60, 100, 120} };
    _queue(p, 1);
}

void alert_tx(void) {
    /* Pitido muy corto sin vibración */
    static const alert_seg_t p[] = { {40, 0, 40} };
    _queue(p, 1);
}

void alert_error(void) {
    /* 3 pitidos rápidos + vibración larga */
    static const alert_seg_t p[] = {
        {80, 200, 100},
        {80, 0,   100},
        {80, 0,   80 },
    };
    _queue(p, 3);
}
