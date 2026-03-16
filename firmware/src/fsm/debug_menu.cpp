#include "debug_menu.h"
#include "comm/lora_comm.h"
#include "comm/lora_msg.h"
#include "drivers/display/display.h"
#include "drivers/btn/btn.h"
#include "drivers/compass/compass.h"
#include "drivers/gps/gps.h"
#include "drivers/led/led.h"

#include <Arduino.h>
#include <stdio.h>
#include <string.h>

/* ── Estados ─────────────────────────────────────────────────────────── */
typedef enum {
    DM_MENU = 0,
    DM_BUTTONS,
    DM_COMPASS,
    DM_LED,
    DM_GPS,
    DM_LORA,
} dm_state_t;

/* ── Menú principal ───────────────────────────────────────────────────── */
#define DM_ITEM_COUNT 6
static const char * const _items[DM_ITEM_COUNT] = {
    "Botones",
    "Brujula",
    "LED RGB",
    "GPS",
    "LoRa TX/RX",
    "<< Volver",
};

static dm_state_t _state  = DM_MENU;
static int        _cursor = 0;
static bool       _done   = false;

/* ── Test Botones — latch por 500 ms ─────────────────────────────────── */
static bool     _b_up  = false, _b_dn = false, _b_sel = false;
static uint32_t _b_up_t = 0,   _b_dn_t = 0,   _b_sel_t = 0;
#define BTN_SHOW_MS 500u

/* ── Test LED ─────────────────────────────────────────────────────────── */
typedef struct { const char *name; uint8_t r, g, b; } color_t;
static const color_t _colors[] = {
    { "APAGADO",  0,   0,   0   },
    { "ROJO",     200, 0,   0   },
    { "VERDE",    0,   200, 0   },
    { "AZUL",     0,   0,   200 },
    { "BLANCO",   150, 150, 150 },
    { "CIAN",     0,   150, 150 },
    { "MAGENTA",  150, 0,   150 },
    { "AMARILLO", 150, 150, 0   },
};
#define COLOR_COUNT 8
static int _led_idx = 0;

/* ── Test LoRa ────────────────────────────────────────────────────────── */
static uint16_t _lora_tx   = 0;
static uint16_t _lora_rx   = 0;
static int16_t  _lora_rssi = 0;
static float    _lora_snr  = 0.0f;

/* ─────────────────────────────────────────────────────────────────────
 * Helpers de dibujo
 * ───────────────────────────────────────────────────────────────────── */

#define DM_VISIBLE 3

static void _draw_menu(void)
{
    display_clear();
    display_print_small(22, 9, "= Debug HW =");

    /* scroll simple: mostrar DM_VISIBLE ítems centrados en _cursor */
    int scroll = _cursor - 1;
    if (scroll < 0) scroll = 0;
    if (scroll + DM_VISIBLE > DM_ITEM_COUNT) scroll = DM_ITEM_COUNT - DM_VISIBLE;

    for (int i = 0; i < DM_VISIBLE; i++) {
        int idx = scroll + i;
        if (idx >= DM_ITEM_COUNT) break;
        int y  = 24 + i * 15;
        int fy = y - 12;
        if (idx == _cursor) {
            display_draw_rbox(0, fy, 128, 14, 3);
            display_set_font_mode(1);
            display_set_draw_color(0);
            display_print_medium(2, y, _items[idx]);
            display_set_draw_color(1);
            display_set_font_mode(0);
        } else {
            display_draw_rframe(0, fy, 128, 14, 3);
            display_print_medium(2, y, _items[idx]);
        }
    }

    if (scroll > 0)
        display_print_small(120, 9, "^");
    if (scroll + DM_VISIBLE < DM_ITEM_COUNT)
        display_print_small(120, 62, "v");

    display_update();
}

static void _draw_buttons(void)
{
    /* flag = capturado por ISR/polling  |  raw = digitalRead actual del pin */
    display_clear();
    display_print_small(10, 9, "= Test Botones =");

    /* columna flag (col 0) y columna raw (col 78) */
    display_print_small(0,  20, "UP  :");
    display_print_small(36, 20, _b_up  ? "[OK]" : "[  ]");
    display_print_small(66, 20, btn_raw_low(BTN_UP)     ? "L" : "H");

    display_print_small(0,  33, "DOWN:");
    display_print_small(36, 33, _b_dn  ? "[OK]" : "[  ]");
    display_print_small(66, 33, btn_raw_low(BTN_DOWN)   ? "L" : "H");

    display_print_small(0,  46, "OK  :");
    display_print_small(36, 46, _b_sel ? "[OK]" : "[  ]");
    display_print_small(66, 46, btn_raw_low(BTN_SELECT) ? "L" : "H");

    display_print_small(0, 57, "flag    pin");
    display_print_small(0, 62, "OK(largo)=salir");
    display_update();
}

static void _draw_compass(void)
{
    char buf[24];
    display_clear();
    display_print_small(10, 9, "= Test Brujula =");

    compass_data_t c;
    compass_read(&c);

    snprintf(buf, sizeof(buf), "Rumbo: %3d deg", (int)c.heading);
    display_print_small(0, 22, buf);

    snprintf(buf, sizeof(buf), "X: %-6d", c.x);
    display_print_small(0, 34, buf);

    snprintf(buf, sizeof(buf), "Y: %-6d", c.y);
    display_print_small(0, 44, buf);

    snprintf(buf, sizeof(buf), "Z: %-6d", c.z);
    display_print_small(0, 54, buf);

    display_print_small(0, 62, "[OK]=salir");
    display_update();
}

static void _draw_led(void)
{
    char buf[24];
    display_clear();
    display_print_small(16, 9, "= Test LED RGB =");

    snprintf(buf, sizeof(buf), "Color: %s", _colors[_led_idx].name);
    display_print_small(0, 26, buf);

    snprintf(buf, sizeof(buf), "R:%-3d G:%-3d B:%-3d",
             _colors[_led_idx].r, _colors[_led_idx].g, _colors[_led_idx].b);
    display_print_small(0, 39, buf);

    /* Indicador visual de posición en la lista */
    snprintf(buf, sizeof(buf), "(%d/%d)", _led_idx + 1, COLOR_COUNT);
    display_print_small(0, 52, buf);

    display_print_small(0, 62, "[^][v]=color [OK]=sal");
    display_update();
}

static void _draw_gps(void)
{
    char buf[24];
    display_clear();
    display_print_small(22, 9, "= Test GPS =");

    gps_data_t g = gps_get_data();

    if (g.valid) {
        snprintf(buf, sizeof(buf), "Lat:%+9.5f", g.latitude);
        display_print_small(0, 21, buf);
        snprintf(buf, sizeof(buf), "Lon:%+9.5f", g.longitude);
        display_print_small(0, 32, buf);
        snprintf(buf, sizeof(buf), "Alt:%.1fm", g.altitude);
        display_print_small(0, 43, buf);
        snprintf(buf, sizeof(buf), "Sat:%-2d Vel:%.1fkm/h", g.satellites, g.speed);
        display_print_small(0, 54, buf);
        display_print_small(0, 62, "Fix:OK  [OK]=salir");
    } else {
        display_print_small(0, 26, "Sin fix GPS");
        snprintf(buf, sizeof(buf), "Sat visibles: %d", g.satellites);
        display_print_small(0, 38, buf);
        display_print_small(0, 50, "Buscando...");
        display_print_small(0, 62, "[OK]=salir");
    }
    display_update();
}

static void _draw_lora(void)
{
    char buf[24];
    display_clear();
    display_print_small(14, 9, "= Test LoRa =");

    snprintf(buf, sizeof(buf), "TX:%-4u  RX:%-4u", (unsigned)_lora_tx, (unsigned)_lora_rx);
    display_print_small(0, 22, buf);

    if (_lora_rx > 0) {
        snprintf(buf, sizeof(buf), "RSSI: %4d dBm", (int)_lora_rssi);
        display_print_small(0, 34, buf);
        snprintf(buf, sizeof(buf), "SNR:  %+5.1f dB", _lora_snr);
        display_print_small(0, 45, buf);
    } else {
        display_print_small(0, 34, "RSSI: --- dBm");
        display_print_small(0, 45, "SNR:  --- dB");
    }

    display_print_small(0, 57, "SF:");
    char sf_buf[8];
    snprintf(sf_buf, sizeof(sf_buf), "%d", (int)lora_comm_get_sf());
    display_print_small(18, 57, sf_buf);

    display_print_small(0, 62, "[^]=TX  [OK]=salir");
    display_update();
}

/* ─────────────────────────────────────────────────────────────────────
 * Helpers de lógica
 * ───────────────────────────────────────────────────────────────────── */

static void _send_debug_ping(void)
{
    lora_msg_t m;
    memset(&m, 0, sizeof(m));
    m.msg_type  = (uint8_t)MSG_TEST_PING;
    m.sender_id = lora_comm_device_id();
    m.bat_pct   = (uint8_t)(_lora_tx & 0xFF);
    lora_comm_send(&m);
    _lora_tx++;
}

static void _apply_led(void)
{
    const color_t *c = &_colors[_led_idx];
    if (c->r == 0 && c->g == 0 && c->b == 0)
        led_off();
    else
        led_set_color(c->r, c->g, c->b);
}

/* ─────────────────────────────────────────────────────────────────────
 * API pública
 * ───────────────────────────────────────────────────────────────────── */

void debug_menu_init(void)
{
    _state  = DM_MENU;
    _cursor = 0;
    _done   = false;
    _b_up = _b_dn = _b_sel = false;
    _led_idx = 0;
    _lora_tx = _lora_rx = 0;
    _lora_rssi = 0; _lora_snr = 0.0f;
}

void debug_menu_update(void)
{
    btn_update();   /* asegurar polling antes de leer flags */
    gps_update();
    lora_comm_tick();

    /* Recibir paquetes LoRa (responder PING, registrar PONG) */
    lora_msg_t msg;
    int16_t    rssi;
    float      snr;
    if (lora_comm_receive(&msg, &rssi, &snr)) {
        if (msg.msg_type == (uint8_t)MSG_TEST_PING) {
            /* Responder como receptor */
            lora_msg_t pong;
            memset(&pong, 0, sizeof(pong));
            pong.msg_type  = (uint8_t)MSG_TEST_PONG;
            pong.sender_id = lora_comm_device_id();
            pong.bat_pct   = msg.bat_pct;
            lora_comm_send(&pong);
        }
        if (msg.msg_type == (uint8_t)MSG_TEST_PONG ||
            msg.msg_type == (uint8_t)MSG_TEST_PING) {
            _lora_rx++;
            _lora_rssi = rssi;
            _lora_snr  = snr;
        }
    }

    uint32_t now = millis();

    /* ── Menú ─────────────────────────────────────────────────────────── */
    if (_state == DM_MENU) {
        if (btn_pressed(BTN_UP))   _cursor = (_cursor > 0) ? _cursor - 1 : DM_ITEM_COUNT - 1;
        if (btn_pressed(BTN_DOWN)) _cursor = (_cursor < DM_ITEM_COUNT - 1) ? _cursor + 1 : 0;
        if (btn_pressed(BTN_SELECT)) {
            switch (_cursor) {
            case 0: _state = DM_BUTTONS; _b_up = _b_dn = _b_sel = false; break;
            case 1: _state = DM_COMPASS; break;
            case 2: _state = DM_LED; _led_idx = 0; led_off(); break;
            case 3: _state = DM_GPS; break;
            case 4: _state = DM_LORA; _lora_tx = _lora_rx = 0; break;
            case 5: _done = true; return;
            }
        }
        _draw_menu();
        return;
    }

    /* ── Test Botones ─────────────────────────────────────────────────── */
    if (_state == DM_BUTTONS) {
        /* Latch desde btn_pressed (flag) — confirma que la lógica funciona */
        if (btn_pressed(BTN_UP))     { _b_up  = true; _b_up_t  = now; }
        if (btn_pressed(BTN_DOWN))   { _b_dn  = true; _b_dn_t  = now; }
        if (btn_pressed(BTN_SELECT)) { _b_sel = true; _b_sel_t = now; }

        /* Salir si OK se mantiene presionado */
        if (btn_held(BTN_SELECT)) {
            _b_up = _b_dn = _b_sel = false;
            _state = DM_MENU;
            return;
        }

        /* Apagar latch tras 500 ms */
        if (_b_up  && now - _b_up_t  > BTN_SHOW_MS) _b_up  = false;
        if (_b_dn  && now - _b_dn_t  > BTN_SHOW_MS) _b_dn  = false;
        if (_b_sel && now - _b_sel_t > BTN_SHOW_MS) _b_sel = false;

        /* Para GPIO2 (UP): si el pin está bajo, forzar el latch directo aquí */
        if (btn_raw_low(BTN_UP) && !_b_up) { _b_up = true; _b_up_t = now; }

        _draw_buttons();
        return;
    }

    /* ── Test Brújula ─────────────────────────────────────────────────── */
    if (_state == DM_COMPASS) {
        if (btn_pressed(BTN_SELECT)) { _state = DM_MENU; return; }
        _draw_compass();
        return;
    }

    /* ── Test LED ─────────────────────────────────────────────────────── */
    if (_state == DM_LED) {
        bool changed = false;
        if (btn_pressed(BTN_UP)) {
            _led_idx = (_led_idx + 1) % COLOR_COUNT;
            changed = true;
        }
        if (btn_pressed(BTN_DOWN)) {
            _led_idx = (_led_idx > 0) ? _led_idx - 1 : COLOR_COUNT - 1;
            changed = true;
        }
        if (changed) _apply_led();

        if (btn_pressed(BTN_SELECT)) {
            led_off();
            _state = DM_MENU;
            return;
        }
        _draw_led();
        return;
    }

    /* ── Test GPS ─────────────────────────────────────────────────────── */
    if (_state == DM_GPS) {
        if (btn_pressed(BTN_SELECT)) { _state = DM_MENU; return; }
        _draw_gps();
        return;
    }

    /* ── Test LoRa ────────────────────────────────────────────────────── */
    if (_state == DM_LORA) {
        if (btn_pressed(BTN_UP))     _send_debug_ping();
        if (btn_pressed(BTN_SELECT)) { _state = DM_MENU; return; }
        _draw_lora();
        return;
    }
}

bool debug_menu_is_done(void) { return _done; }
