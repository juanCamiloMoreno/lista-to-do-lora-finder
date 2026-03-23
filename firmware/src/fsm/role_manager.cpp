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

static node_role_t _role        = ROLE_NONE;
static int         _home_cursor = 1;   /* 0=MENU  1=BUSCAR  2=OFF-DIS */

/* ── Pantalla de inicio (ROLE_NONE) ───────────────────────────────────── */

/* Dibuja un botón de la home: si selected=true → caja llena + texto XOR */
static void _draw_home_btn(int bx, int by, int bw, int bh,
                           const char *label, int lx, int ly, bool selected)
{
    if (selected) {
        display_draw_rbox(bx, by, bw, bh, 3); /* fondo blanco redondeado */
        display_set_font_mode(1);
        display_set_draw_color(0);
        display_print_small(lx, ly, label);
        display_set_draw_color(1);
        display_set_font_mode(0);
    } else {
        display_draw_rframe(bx, by, bw, bh, 3);
        display_print_small(lx, ly, label);
    }
}

static void _draw_home(void)
{
    display_clear();

    /* ── Batería ──────────────────────────────────────────────────────── */
    battery_data_t bat;
    battery_read(&bat);
    battery_draw_icon(0, 0, &bat);

    /* ── Botón 0: MENU (arriba) ───────────────────────────────────────── */
    display_draw_line(7, 28, 7, 18);
    display_draw_line(7, 18, 4, 21);
    display_draw_line(7, 18, 10, 21);
    _draw_home_btn(15, 18, 27, 11, "MENU", 17, 27, _home_cursor == 0);

    /* ── Botón 1: BUSCAR (medio) ─────────────────────────────────────── */
    display_print_small(2, 42, "OK");
    _draw_home_btn(15, 33, 39, 11, "BUSCAR", 17, 42, _home_cursor == 1);

    /* ── Botón 2: OFF-DIS (abajo) ────────────────────────────────────── */
    display_draw_line(10, 55, 7, 58);
    display_draw_line(4,  55, 7, 58);
    display_draw_line(7,  58, 7, 48);
    _draw_home_btn(15, 48, 45, 11, "OFF-DIS", 17, 57, _home_cursor == 2);

    /* ── Brújula ──────────────────────────────────────────────────────── */
    compass_data_t cmp;
    compass_read(&cmp);
    display_draw_circle(96, 31, 31);
    display_draw_line(69, 32, 124, 32);
    display_draw_line(124, 32, 121, 29);
    display_draw_line(124, 32, 121, 35);
    display_print_small(119, 44, "N");
    display_print_small(112, 26, "D");
    compass_draw_arrow(96, 31, cmp.heading, 28);
    compass_draw_north_arrow(96, 31, 270.0f - cmp.heading, 18);

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
    btn_update();   /* fallback polling — complementa ISR para GPIO problemáticos */
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

        /* Leer botones */
        bool was_off = power_standby_active();
        bool up  = btn_pressed(BTN_UP);
        bool dn  = btn_pressed(BTN_DOWN);
        bool sel = btn_pressed(BTN_SELECT);

        if (up || dn || sel) power_standby_reset();

        if (!power_standby_active()) {
            /* Primer toque cuando estaba apagada: solo reactiva */
            if (!was_off) {
                if (up) _home_cursor = (_home_cursor > 0) ? _home_cursor - 1 : 2;
                if (dn) _home_cursor = (_home_cursor < 2) ? _home_cursor + 1 : 0;

                if (sel) {
                    switch (_home_cursor) {
                    case 0:                  /* MENU */
                        _role = ROLE_TEST_MENU;
                        lora_comm_set_state("TEST_MENU");
                        test_menu_init();
                        return;
                    case 1:                  /* BUSCAR */
                        _role = ROLE_SEARCHER;
                        fsm_searcher_init();   /* lora_comm_set_state dentro */
                        return;
                    case 2:                  /* OFF-DIS */
                        power_standby_force();
                        break;
                    }
                }
            }
            _draw_home();
        }

        power_standby_tick();
        break;
    }

    /* ── ROL BUSCADOR ─────────────────────────────────────────────────── */
    case ROLE_SEARCHER:
        fsm_searcher_update();
        if (fsm_searcher_is_done()) {
            _role = ROLE_NONE;
            lora_comm_set_state("ROLE_NONE");
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
            lora_comm_set_state("ROLE_NONE");
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
