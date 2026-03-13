#include "app.h"
#include "diag.h"
#include "config/system_config.h"
#include "drivers/display/display.h"
#include "drivers/compass/compass.h"
#include "drivers/gps/gps.h"
#include "drivers/battery/battery.h"
#include "drivers/lora/lora.h"

#include <Arduino.h>
#include <stdio.h>
#include <string.h>

static compass_data_t _compass;
static battery_data_t _battery;

/* ── Paquete LoRa (mismo struct en sender y receiver) ────────────────── */
typedef struct __attribute__((packed)) {
    uint32_t seq;          /* Número de secuencia */
    int16_t  heading_x10;  /* heading * 10  →  3411 = 341.1° */
    uint16_t bat_mv;       /* voltaje batería en mV */
    uint8_t  bat_pct;      /* porcentaje batería */
} lora_pkt_t;

/* ── Inicialización (llamado una sola vez desde setup()) ─────────────── */

void app_init(void)
{
    Serial.begin(115200);
    /* USB-CDC en ESP32-S3 necesita tiempo para enumerar.
     * Espera hasta 3 s o hasta que el host abra el puerto. */
    for (uint32_t t = millis(); !Serial && (millis() - t) < 3000; ) delay(10);
    delay(100);

    if (!display_init()) {
        Serial.println("[app] display_init fallo — revisa pines/OLED");
    }
    compass_init();
    gps_init();
    battery_init();
    lora_init();

#if defined(NODE_ROLE_RECEIVER)
    lora_start_rx();
    Serial.println("[app] Modo RECEIVER — escuchando...");
#elif defined(NODE_ROLE_SENDER)
    Serial.println("[app] Modo SENDER — enviando cada 2 s");
#endif

    /* Calibrar si no hay datos guardados */
    if (!compass_has_calibration()) {
        Serial.println("[app] Sin calibracion — iniciando calibracion...");
        compass_calibrate();
    }

    /* Diagnóstico completo por Serial */
    diag_report();

    /* Pantalla de bienvenida */
    Serial.println("[app] Mostrando splash...");
    display_clear();
    display_print_medium(0, 20, "LoRa Finder");
    display_print_small (0, 40, "Iniciando...");
    display_update();
    delay(800);
}

/* ── Bucle principal (llamado repetidamente desde loop()) ────────────── */

#if defined(NODE_ROLE_SENDER)
static void _lora_send_tick(void)
{
    static uint32_t seq      = 0;
    static uint32_t last_ms  = 0;
    if (millis() - last_ms < 2000) return;
    last_ms = millis();

    lora_pkt_t pkt;
    pkt.seq         = ++seq;
    pkt.heading_x10 = (int16_t)(_compass.heading * 10.0f);
    pkt.bat_mv      = (uint16_t)(_battery.voltage * 1000.0f);
    pkt.bat_pct     = _battery.percent;

    bool ok = lora_send((const uint8_t *)&pkt, sizeof(pkt));
    Serial.printf("[TX] seq=%lu  heading=%.1f  bat=%umV %u%%  %s\n",
                  (unsigned long)seq,
                  _compass.heading,
                  (unsigned)pkt.bat_mv,
                  (unsigned)pkt.bat_pct,
                  ok ? "OK" : "ERROR");
}
#endif

#if defined(NODE_ROLE_RECEIVER)
static void _lora_recv_tick(void)
{
    uint8_t  buf[sizeof(lora_pkt_t)];
    int16_t  rssi;
    float    snr;

    int n = lora_receive(buf, sizeof(buf), &rssi, &snr);
    if (n <= 0) return;

    if (n != (int)sizeof(lora_pkt_t)) {
        Serial.printf("[RX] paquete inesperado: %d bytes\n", n);
        return;
    }
    lora_pkt_t pkt;
    memcpy(&pkt, buf, sizeof(pkt));

    Serial.printf("[RX] seq=%lu  heading=%.1f  bat=%umV %u%%"
                  "  RSSI=%d dBm  SNR=%.1f dB\n",
                  (unsigned long)pkt.seq,
                  pkt.heading_x10 / 10.0f,
                  (unsigned)pkt.bat_mv,
                  (unsigned)pkt.bat_pct,
                  (int)rssi,
                  snr);
}
#endif

void app_run(void)
{
    compass_read(&_compass);
    battery_read(&_battery);

    display_clear();

    /* ── Panel derecho: brújula gráfica ── */
    compass_draw_ui(_compass.heading,
                    COMPASS_UI_CX,
                    COMPASS_UI_CY,
                    COMPASS_UI_RADIUS);

    /* ── Panel izquierdo: heading + batería + GPS ── */
    char buf[24];

    /* Ícono gráfico de batería: cuerpo + relleno + % */
    battery_draw_icon(0, 0, &_battery);

    gps_update();
    gps_data_t gps = gps_get_data();

    snprintf(buf, sizeof(buf), "%3d deg", (int)_compass.heading);
    display_print_small(2, 20, buf);

    if (gps.valid) {
        snprintf(buf, sizeof(buf), "%.5f", gps.latitude);
        display_print_small(2, 36, buf);
        snprintf(buf, sizeof(buf), "%.5f", gps.longitude);
        display_print_small(2, 46, buf);
        snprintf(buf, sizeof(buf), "Sat:%d", gps.satellites);
        display_print_small(2, 57, buf);
    } else {
        display_print_small(2, 38, "GPS buscando");
        snprintf(buf, sizeof(buf), "Sat:%d", gps.satellites);
        display_print_small(2, 50, buf);
    }

#if defined(NODE_ROLE_SENDER)
    _lora_send_tick();
#elif defined(NODE_ROLE_RECEIVER)
    _lora_recv_tick();
#endif

    display_update();

    delay(60);
}
