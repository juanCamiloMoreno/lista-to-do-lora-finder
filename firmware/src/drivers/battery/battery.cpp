#include "battery.h"
#include "config/pins.h"
#include "drivers/display/display.h"
#include <Arduino.h>
#include <stdio.h>

/*
 * Heltec WiFi LoRa 32 V4 — medición de batería:
 *   BAT_CTRL_PIN (GPIO37) OUTPUT HIGH  → habilita el divisor resistivo
 *   BAT_ADC_PIN  (GPIO1)  ADC1_CH0     → lectura del divisor
 *   Atenuación 2.5 dB (divisor de alta impedancia)
 *   Vbat_real = Vadc_mV × 4.9 × 1.045 / 1000
 */

/* ── Umbrales de la celda LiPo 1S ──────────────────────────────────────── */
#define BAT_VMAX      4.20f   /* 100 % — carga completa */
#define BAT_VMIN      3.00f   /*   0 % — vacía (corte de seguridad) */
#define BAT_VFULL     4.15f   /* A partir de aquí se considera "lleno" */

#define ADC_SAMPLES      8       /* Promediado para reducir ruido */
#define ADC_MULTIPLIER   5.1205f /* 4.9 × 1.045 — ratio del divisor del V4 */

/* ── Implementación ─────────────────────────────────────────────────────── */

void battery_init(void)
{
    /* GPIO37 HIGH habilita el divisor resistivo de batería en el V4 */
    pinMode(BAT_CTRL_PIN, OUTPUT);
    digitalWrite(BAT_CTRL_PIN, HIGH);

    /* Atenuación 2.5 dB: divisor de alta impedancia, rango ~0-1 V en el ADC */
    analogSetPinAttenuation(BAT_ADC_PIN, ADC_2_5db);
    pinMode(BAT_ADC_PIN, INPUT);
}

static float _read_voltage(void)
{
    uint32_t sum = 0;
    for (int i = 0; i < ADC_SAMPLES; i++) {
        sum += analogReadMilliVolts(BAT_ADC_PIN);
    }
    float vadc_mv = (float)sum / ADC_SAMPLES;

    return (vadc_mv * ADC_MULTIPLIER) / 1000.0f;   /* → Voltios */
}

static uint8_t _voltage_to_percent(float v)
{
    if (v >= BAT_VMAX) return 100;
    if (v <= BAT_VMIN) return 0;

    /* Curva aproximada de descarga LiPo (lineal entre VMIN y VMAX) */
    float pct = (v - BAT_VMIN) / (BAT_VMAX - BAT_VMIN) * 100.0f;
    return (uint8_t)(pct + 0.5f);   /* redondeo */
}

void battery_read(battery_data_t *out)
{
    if (!out) return;

    out->voltage = _read_voltage();
    out->percent = _voltage_to_percent(out->voltage);

    /* Detección de estado de carga por voltaje
     * (sin pin CHRG confirmado en V4)
     * ─ Si el voltaje sube respecto a la lectura anterior → cargando
     * ─ Si ya alcanzó BAT_VFULL → lleno
     */
    static float _prev_voltage = 0.0f;
    static uint32_t _prev_ts    = 0;

    uint32_t now = millis();
    bool rising = false;
    if (_prev_ts > 0 && (now - _prev_ts) >= 4000) {
        rising = (out->voltage - _prev_voltage) > 0.005f;  /* +5 mV en 4 s */
    }
    if ((now - _prev_ts) >= 4000) {
        _prev_voltage = out->voltage;
        _prev_ts      = now;
    }
    if (_prev_ts == 0) _prev_ts = now;

    if (out->voltage >= BAT_VFULL) {
        out->status = BAT_STATUS_FULL;
    } else if (rising) {
        out->status = BAT_STATUS_CHARGING;
    } else {
        out->status = BAT_STATUS_DISCHARGING;
    }

    /* ── Debug: voltaje cada 5 s ── */
    static uint32_t _last_print = 0;
    if (millis() - _last_print >= 5000) {
        _last_print = millis();
        Serial.printf("[bat] %.3fV  %d%%  status=%d\n",
                      out->voltage, out->percent, (int)out->status);
    }
}

/* ── Ícono gráfico de batería ────────────────────────────────────────────
 *
 *  Cuerpo: rectángulo de 22 × 10 px  (x..x+21, y..y+9)
 *  Nub   : terminal derecho 2 × 4 px (x+22..x+23, y+3..y+6)
 *  Relleno: columnas internas según porcentaje (máx 18 columnas)
 *  Texto : "xx%" a la derecha del nub
 *
 * ─────────────────────────────────────────────────────────────────────── */
void battery_draw_icon(int x, int y, const battery_data_t *data)
{
    if (!data) return;

    /* ── Cuerpo (outline) con 4 líneas ── */
    display_draw_line(x,      y,     x + 21, y);      /* top    */
    display_draw_line(x,      y + 9, x + 21, y + 9);  /* bottom */
    display_draw_line(x,      y,     x,      y + 9);   /* left   */
    display_draw_line(x + 21, y,     x + 21, y + 9);   /* right  */

    /* ── Terminal positivo (nub) ── */
    display_draw_line(x + 22, y + 3, x + 23, y + 3);  /* top nub    */
    display_draw_line(x + 22, y + 6, x + 23, y + 6);  /* bottom nub */
    display_draw_line(x + 23, y + 3, x + 23, y + 6);  /* right nub  */

    /* ── Relleno interior proporcional al % ──
     * Área interna: x+1 .. x+20  (20 columnas disponibles → usamos 18)
     * Para no pegar el relleno al borde izquierdo dejamos 1 px de margen.
     */
    int fill_cols = (18 * data->percent) / 100;  /* 0-18 columnas */
    for (int col = 0; col < fill_cols; col++) {
        display_draw_line(x + 2 + col, y + 2,
                          x + 2 + col, y + 7);
    }

    /* ── Símbolo de carga ⚡ (rayo simple) cuando está cargando ── */
    if (data->status == BAT_STATUS_CHARGING) {
        /* Rayo: línea diagonal central sobre el relleno */
        display_draw_line(x + 12, y + 1, x + 10, y + 5);
        display_draw_line(x + 10, y + 5, x + 13, y + 5);
        display_draw_line(x + 13, y + 5, x + 11, y + 8);
    }

    /* ── Porcentaje a la derecha ── */
    char buf[6];
    snprintf(buf, sizeof(buf), "%3d%%", data->percent);
    display_print_small(x + 26, y + 7, buf);
}
