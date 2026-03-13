#include "app.h"
#include "config/system_config.h"
#include "drivers/display/display.h"
#include "drivers/compass/compass.h"
#include "drivers/gps/gps.h"

#include <Arduino.h>
#include <stdio.h>

static compass_data_t _compass;

/* ── Inicialización (llamado una sola vez desde setup()) ─────────────── */

void app_init(void)
{
    Serial.begin(115200);
    delay(300);

    if (!display_init()) {
        Serial.println("[app] display_init fallo — revisa pines/OLED");
    }
    compass_init();
    gps_init();

    /* Calibrar si no hay datos guardados */
    if (!compass_has_calibration()) {
        Serial.println("[app] Sin calibracion — iniciando calibracion...");
        compass_calibrate();
    }

    /* Pantalla de bienvenida */
    Serial.println("[app] Mostrando splash...");
    display_clear();
    display_print_medium(0, 20, "LoRa Finder");
    display_print_small (0, 40, "Iniciando...");
    display_update();
    delay(800);
}

/* ── Bucle principal (llamado repetidamente desde loop()) ────────────── */

void app_run(void)
{
    compass_read(&_compass);

    display_clear();

    /* ── Panel derecho: brújula gráfica ── */
    compass_draw_ui(_compass.heading,
                    COMPASS_UI_CX,
                    COMPASS_UI_CY,
                    COMPASS_UI_RADIUS);

    /* ── Panel izquierdo: heading + GPS ── */
    char buf[24];

    gps_update();
    gps_data_t gps = gps_get_data();

    snprintf(buf, sizeof(buf), "%3d deg", (int)_compass.heading);
    display_print_medium(2, 10, buf);

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

    display_update();

    delay(60);
}
