#include "battery.h"
#include "config/pins.h"
#include "drivers/display/display.h"
#include <Arduino.h>
#include <stdio.h>

/*
 * BAT_ADC_PIN  (GPIO1)  → ADC1_CH0: voltaje batería vía divisor 1:2 del PCB
 *                          Vbat_real = Vadc × 2
 * BAT_CHRG_PIN (GPIO7)  → CHRG del TP4054 (open-drain, LOW = cargando)
 */

/* ── Umbrales de la celda LiPo 1S ──────────────────────────────────────── */
#define BAT_VMAX      4.20f   /* 100 % — carga completa */
#define BAT_VMIN      3.00f   /*   0 % — vacía (corte de seguridad) */
#define BAT_VFULL     4.15f   /* A partir de aquí se considera "lleno" */

/* ── Calibración ADC ─────────────────────────────────────────────────────
 * ESP32-S3: referencia interna ~3.3 V, resolución 12 bits (0-4095).
 * Usamos analogReadMilliVolts() que incluye la corrección de linealidad
 * del IDF, así que solo aplicamos el factor del divisor.
 * ─────────────────────────────────────────────────────────────────────── */
#define ADC_SAMPLES   8   /* Promediado para reducir ruido */

/* ── Estado interno ─────────────────────────────────────────────────────── */
static bool _chrg_pin_ok = false;  /* false si el pin no está disponible */

/* ── Implementación ─────────────────────────────────────────────────────── */

void battery_init(void)
{
    /* ADC: atenuación 11 dB → rango ~0-3.3 V */
    analogSetAttenuation(ADC_11db);
    pinMode(BAT_ADC_PIN, INPUT);

    /* Pin de detección de carga con pull-up interno */
    pinMode(BAT_CHRG_PIN, INPUT_PULLUP);
    _chrg_pin_ok = true;
}

static float _read_voltage(void)
{
    uint32_t sum = 0;
    for (int i = 0; i < ADC_SAMPLES; i++) {
        sum += analogReadMilliVolts(BAT_ADC_PIN);
    }
    float vadc_mv = (float)sum / ADC_SAMPLES;

    /* El divisor 1:2 del PCB divide el voltaje a la mitad → multiplicamos ×2 */
    return (vadc_mv * 2.0f) / 1000.0f;   /* → Voltios */
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

    /* Detección de estado de carga */
    if (_chrg_pin_ok && digitalRead(BAT_CHRG_PIN) == LOW) {
        out->status = BAT_STATUS_CHARGING;
    } else if (out->voltage >= BAT_VFULL) {
        out->status = BAT_STATUS_FULL;
    } else {
        out->status = BAT_STATUS_DISCHARGING;
    }

    /* ── Debug: imprime cada 2 s para diagnóstico de pines ── */
    static uint32_t _last_print = 0;
    if (millis() - _last_print >= 2000) {
        _last_print = millis();
        uint32_t raw_mv = analogReadMilliVolts(BAT_ADC_PIN);
        Serial.printf("[bat] GPIO%d raw=%lumV  Vbat=%.3fV  pct=%d%%  "
                      "CHRG(GPIO%d)=%d\n",
                      BAT_ADC_PIN, raw_mv,
                      out->voltage, out->percent,
                      BAT_CHRG_PIN, digitalRead(BAT_CHRG_PIN));
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
