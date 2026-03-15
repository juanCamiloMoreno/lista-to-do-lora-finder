#include "role_manager.h"
#include "fsm_searcher.h"
#include "fsm_target.h"
#include "test_menu.h"
#include "comm/lora_comm.h"
#include "config/system_config.h"
#include "drivers/display/display.h"
#include "drivers/compass/compass.h"
#include "drivers/gps/gps.h"
#include "drivers/battery/battery.h"
#include "drivers/btn/btn.h"
#include "drivers/alert/alert.h"
#include "drivers/power/power.h"

#include <Arduino.h>
#include <stdio.h>

static node_role_t _role = ROLE_NONE;

/* ── Pantalla de inicio (ROLE_NONE) ───────────────────────────────────── */

static void _draw_home(void)
{
    char buf[24];
    display_clear();

    battery_data_t bat;
    battery_read(&bat);
    battery_draw_icon(0, 0, &bat);

    display_print_medium(20, 14, "LoRa Finder");

    gps_data_t g = gps_get_data();
    if (g.valid) {
        snprintf(buf, sizeof(buf), "GPS OK  Sat:%d", g.satellites);
        display_print_small(0, 28, buf);
    } else {
        snprintf(buf, sizeof(buf), "GPS --  Sat:%d", g.satellites);
        display_print_small(0, 28, buf);
    }

    /* Brújula compacta: solo el heading numérico + mini-UI */
    compass_data_t cmp;
    compass_read(&cmp);
    snprintf(buf, sizeof(buf), "Rumbo: %3d", (int)cmp.heading);
    display_print_small(0, 40, buf);

    compass_draw_ui(cmp.heading, COMPASS_UI_CX, COMPASS_UI_CY, COMPASS_UI_RADIUS);

    display_print_small(0, 57, "[OK]=Buscar [v]=Tests");
    display_update();
}

/* ── API pública ──────────────────────────────────────────────────────── */

void role_manager_init(void)
{
    _role = ROLE_NONE;
    power_standby_init();
}

void role_manager_update(void)
{
    alert_tick();

    switch (_role) {

    /* ── SIN ROL: pantalla de inicio ──────────────────────────────────── */
    case ROLE_NONE: {
        gps_update();
        lora_comm_tick();

        /* ¿Recibimos SEARCH_START de otro dispositivo? → TARGET */
        lora_msg_t msg;
        int16_t    rssi;
        float      snr;
        if (lora_comm_receive(&msg, &rssi, &snr)) {
            if (msg.msg_type == (uint8_t)MSG_SEARCH_START) {
                power_standby_reset();   /* reactivar pantalla antes de cambiar rol */
                _role = ROLE_TARGET;
                fsm_target_init(&msg, rssi);
                alert_beep_short();
                return;
            }
            /* Cualquier otro paquete recibido también despierta la pantalla */
            power_standby_reset();
        }

        /* Cualquier botón presionado reactiva la pantalla */
        bool up  = btn_pressed(BTN_UP);
        bool dn  = btn_pressed(BTN_DOWN);
        bool sel = btn_pressed(BTN_SELECT);

        if (up || dn || sel) {
            power_standby_reset();
        }

        /* Si la pantalla estaba apagada, el primer toque solo la reactiva */
        if (!power_standby_active()) {
            if (dn) {
                _role = ROLE_TEST_MENU;
                test_menu_init();
                return;
            }
            if (sel) {
                _role = ROLE_SEARCHER;
                fsm_searcher_init();
                return;
            }
            _draw_home();
        }

        power_standby_tick();   /* apaga pantalla si se agotó el timeout */
        break;
    }

    /* ── ROL BUSCADOR ─────────────────────────────────────────────────── */
    case ROLE_SEARCHER:
        fsm_searcher_update();
        if (fsm_searcher_is_done()) {
            _role = ROLE_NONE;
            power_standby_init();
            /* Pantalla breve de retorno */
            display_clear();
            display_print_medium(15, 24, "LoRa Finder");
            display_print_small (0,  42, "Sesion terminada");
            display_update();
            delay(1500);
        }
        break;

    /* ── ROL OBJETIVO ─────────────────────────────────────────────────── */
    case ROLE_TARGET:
        fsm_target_update();
        if (fsm_target_is_done()) {
            _role = ROLE_NONE;
            power_standby_init();
            display_clear();
            display_print_medium(15, 24, "LoRa Finder");
            display_print_small (0,  42, "Sesion terminada");
            display_update();
            delay(1500);
        }
        break;

    /* ── MENÚ DE PRUEBAS ──────────────────────────────────────────────── */
    case ROLE_TEST_MENU:
        test_menu_update();
        if (test_menu_is_done()) {
            _role = ROLE_NONE;
            power_standby_init();
        }
        break;
    }
}

node_role_t role_manager_get_role(void) { return _role; }
