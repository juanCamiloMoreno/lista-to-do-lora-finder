#include "app.h"
#include "config/system_config.h"
#include "diag.h"
#include "fsm/role_manager.h"
#include "comm/lora_comm.h"
#include "config/nvs_config.h"   /* C-11: configuración persistente */
#include "drivers/display/display.h"
#include "drivers/compass/compass.h"
#include "drivers/gps/gps.h"
#include "drivers/battery/battery.h"
#include "drivers/lora/lora.h"
#include "drivers/led/led.h"
#include "drivers/alert/alert.h"
#include "drivers/btn/btn.h"
#include "drivers/power/power.h"

#include <Arduino.h>

/* ── Inicialización — llamado una vez desde setup() ──────────────────── */

void app_init(void)
{
    Serial.begin(115200);
    /* ESP32-S3 USB-CDC: esperar hasta 3 s a que el host abra el puerto */
    for (uint32_t t = millis(); !Serial && (millis() - t) < 3000; ) delay(10);
    delay(100);

    if (!display_init()) {
        Serial.println("[app] display_init fallo");
        /* UC-07: modo ciego — continuar sin pantalla */
    }

    compass_init();
    gps_init();
    battery_init();
    led_init();
    alert_init();
    btn_init();
    lora_init();
    lora_comm_init();

    /* Cargar configuración desde NVS (C-11) y aplicar SF guardado */
    nvs_config_init();
    {
        nvs_config_t *cfg = nvs_config_get();
        if (cfg->spreading_factor != LORA_SF) {
            lora_comm_set_sf(cfg->spreading_factor);
        }
    }

    /* Calibrar compás si no hay datos guardados */
    if (!compass_has_calibration()) {
        Serial.println("[app] Sin calibracion — iniciando calibracion...");
        compass_calibrate();
    }

    diag_report();

    /* Pantalla de bienvenida */
    display_clear();
    display_print_medium(20, 20, "LoRa Finder");
    display_print_small (20, 40, "Iniciando...");
    display_update();
    delay(800);

    role_manager_init();
    Serial.println("[app] Sistema listo — en modo sin rol");
}

/* ── Bucle principal — llamado repetidamente desde loop() ────────────── */

void app_run(void)
{
    role_manager_update();
    delay(40);
}
