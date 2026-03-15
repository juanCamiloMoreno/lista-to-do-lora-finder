#include "test_menu.h"
#include "debug_menu.h"
#include "comm/lora_comm.h"
#include "comm/lora_msg.h"
#include "config/system_config.h"
#include "drivers/display/display.h"
#include "drivers/gps/gps.h"
#include "drivers/btn/btn.h"
#include "drivers/alert/alert.h"

#include <Arduino.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <limits.h>

/* ── Estados ─────────────────────────────────────────────────────────── */
typedef enum {
    TM_MENU = 0,
    TM_ALCANCE,
    TM_PDR,
    TM_RTT,
    TM_GPS,
    TM_CONFIG_SF,
    TM_DEBUG,
} tm_state_t;

/* ── Menú ─────────────────────────────────────────────────────────────── */
#define MENU_COUNT   7
#define MENU_VISIBLE 4
static const char * const _items[MENU_COUNT] = {
    "Alcance/RSSI",
    "PDR paquetes",
    "T. Respuesta",
    "Precision GPS",
    "Config SF",
    "Debug HW",
    "<< Volver",
};

/* ── Estado global ───────────────────────────────────────────────────── */
static tm_state_t _state  = TM_MENU;
static int        _cursor = 0;
static int        _scroll = 0;
static bool       _done   = false;

/* ── Alcance/RSSI ────────────────────────────────────────────────────── */
static uint32_t _alc_tx      = 0;
static uint32_t _alc_rx      = 0;
static int16_t  _alc_rssi    = 0;
static float    _alc_snr     = 0.0f;
static uint32_t _alc_last_tx = 0;

/* ── PDR ─────────────────────────────────────────────────────────────── */
#define PDR_TOTAL 20
static uint8_t  _pdr_sent    = 0;
static uint8_t  _pdr_recv    = 0;
static uint32_t _pdr_last_tx = 0;
static uint32_t _pdr_tx_ms   = 0;
static uint32_t _pdr_rtt_ms  = 0;

/* ── RTT ─────────────────────────────────────────────────────────────── */
static uint32_t _rtt_last_ms = 0;
static uint32_t _rtt_min_ms  = UINT32_MAX;
static uint32_t _rtt_sum_ms  = 0;
static uint32_t _rtt_count   = 0;
static uint32_t _rtt_tx_ms   = 0;
static bool     _rtt_waiting = false;
static uint8_t  _rtt_seq     = 0;
static uint32_t _rtt_last_tx = 0;
static uint32_t _rtt_tx_cnt  = 0;
static uint32_t _rtt_rx_cnt  = 0;

/* ── Config SF ───────────────────────────────────────────────────────── */
static uint8_t  _cfg_sf      = LORA_SF;
static bool     _cfg_applied = false;

/* ─────────────────────────────────────────────────────────────────────
 * Helpers
 * ───────────────────────────────────────────────────────────────────── */

/* Estimación de ToA para 17 bytes (BW=125kHz, CR=4/5, explicit header) */
static uint32_t _toa_ms(uint8_t sf)
{
    /* Tsym = 2^SF / 125000 Hz → en µs = 2^SF * 8 */
    uint32_t tsym_us = ((uint32_t)1u << sf) * 8u;
    /* Preámbulo: 12.25 símbolos → 12*tsym + tsym/4 */
    uint32_t t_pre   = tsym_us * 12u + tsym_us / 4u;
    /* LDR activo si SF >= 11 */
    int de  = (sf >= 11) ? 1 : 0;
    int num = 8 * 17 - 4 * (int)sf + 44;      /* numerador */
    int den = 4 * ((int)sf - 2 * de);          /* denominador */
    if (den <= 0) den = 4;
    int ns  = ((num + den - 1) / den) * 5 + 8; /* símbolos payload */
    if (ns < 8) ns = 8;
    uint32_t t_pay = (uint32_t)ns * tsym_us;
    return (t_pre + t_pay) / 1000u;            /* ms */
}

static void _build_ping(lora_msg_t *m, uint8_t seq)
{
    memset(m, 0, sizeof(*m));
    m->msg_type  = (uint8_t)MSG_TEST_PING;
    m->sender_id = lora_comm_device_id();
    m->bat_pct   = seq;   /* bat_pct reutilizado como número de secuencia */
}

static void _send_pong(uint8_t seq)
{
    lora_msg_t p;
    memset(&p, 0, sizeof(p));
    p.msg_type  = (uint8_t)MSG_TEST_PONG;
    p.sender_id = lora_comm_device_id();
    p.bat_pct   = seq;
    lora_comm_send(&p);
}

/* ─────────────────────────────────────────────────────────────────────
 * Manejo de paquetes recibidos
 * ───────────────────────────────────────────────────────────────────── */
static void _handle_rx(const lora_msg_t *msg, int16_t rssi, float snr)
{
    if (msg->msg_type == (uint8_t)MSG_TEST_PING) {
        _send_pong(msg->bat_pct);           /* responder siempre */
        if (_state == TM_ALCANCE) {
            _alc_rx++;
            _alc_rssi = rssi;
            _alc_snr  = snr;
        }
        return;
    }

    if (msg->msg_type == (uint8_t)MSG_TEST_PONG) {
        uint8_t seq = msg->bat_pct;
        if (_state == TM_ALCANCE) {
            _alc_rx++;
            _alc_rssi = rssi;
            _alc_snr  = snr;
        }
        else if (_state == TM_PDR && _pdr_sent > 0) {
            _pdr_recv++;
            _pdr_rtt_ms = millis() - _pdr_tx_ms;
        }
        else if (_state == TM_RTT && _rtt_waiting && seq == _rtt_seq) {
            uint32_t rtt = millis() - _rtt_tx_ms;
            _rtt_last_ms = rtt;
            _rtt_waiting = false;
            _rtt_rx_cnt++;
            if (rtt < _rtt_min_ms) _rtt_min_ms = rtt;
            _rtt_sum_ms += rtt;
            _rtt_count++;
        }
    }
}

/* ─────────────────────────────────────────────────────────────────────
 * Dibujo por estado
 * ───────────────────────────────────────────────────────────────────── */

static void _draw_menu(void)
{
    display_clear();
    display_print_small(14, 9, "= Menu Pruebas =");

    for (int i = 0; i < MENU_VISIBLE && (_scroll + i) < MENU_COUNT; i++) {
        int idx = _scroll + i;
        int y   = 20 + i * 11;
        display_print_small(0,  y, (idx == _cursor) ? ">" : " ");
        display_print_small(9,  y, _items[idx]);
    }

    if (_scroll > 0)
        display_print_small(114, 9, "^");
    if (_scroll + MENU_VISIBLE < MENU_COUNT)
        display_print_small(114, 62, "v");

    display_print_small(0, 62, "[OK]=entra");
    display_update();
}

static void _draw_alcance(void)
{
    char buf[24];
    display_clear();
    display_print_small(14, 9, "= Alcance/RSSI =");

    snprintf(buf, sizeof(buf), "TX:%-4u  RX:%-4u", (unsigned)_alc_tx, (unsigned)_alc_rx);
    display_print_small(0, 21, buf);

    snprintf(buf, sizeof(buf), "RSSI:%4d dBm", (int)_alc_rssi);
    display_print_small(0, 32, buf);

    snprintf(buf, sizeof(buf), "SNR: %+5.1f dB", _alc_snr);
    display_print_small(0, 43, buf);

    unsigned pdr = (_alc_tx > 0) ? (unsigned)(_alc_rx * 100u / _alc_tx) : 0u;
    snprintf(buf, sizeof(buf), "PDR:%3u%%   SF:%d", pdr, (int)lora_comm_get_sf());
    display_print_small(0, 54, buf);

    display_print_small(0, 62, "[OK]=salir");
    display_update();
}

static void _draw_pdr(void)
{
    char buf[24];
    display_clear();
    display_print_small(20, 9, "= PDR 20 pkts =");

    snprintf(buf, sizeof(buf), "Enviados:  %2d / %d", (int)_pdr_sent, PDR_TOTAL);
    display_print_small(0, 21, buf);

    snprintf(buf, sizeof(buf), "Recibidos: %2d", (int)_pdr_recv);
    display_print_small(0, 32, buf);

    if (_pdr_sent > 0) {
        unsigned p10 = (unsigned)_pdr_recv * 1000u / _pdr_sent;
        snprintf(buf, sizeof(buf), "PDR: %u.%u%%", p10 / 10, p10 % 10);
    } else {
        snprintf(buf, sizeof(buf), "PDR: --");
    }
    display_print_small(0, 43, buf);

    if (_pdr_rtt_ms)
        snprintf(buf, sizeof(buf), "RTT ult: %ums", (unsigned)_pdr_rtt_ms);
    else
        snprintf(buf, sizeof(buf), "RTT ult: ---");
    display_print_small(0, 54, buf);

    display_print_small(0, 62, (_pdr_sent >= PDR_TOTAL) ? "[OK]=reiniciar" : "midiendo...");
    display_update();
}

static void _draw_rtt(void)
{
    char buf[24];
    display_clear();
    display_print_small(14, 9, "= T. Respuesta =");

    snprintf(buf, sizeof(buf), "Ultimo:  %5ums", (unsigned)_rtt_last_ms);
    display_print_small(0, 21, buf);

    if (_rtt_count > 0) {
        snprintf(buf, sizeof(buf), "Minimo:  %5ums", (unsigned)_rtt_min_ms);
        display_print_small(0, 32, buf);
        snprintf(buf, sizeof(buf), "Promedio:%5ums", (unsigned)(_rtt_sum_ms / _rtt_count));
        display_print_small(0, 43, buf);
    } else {
        display_print_small(0, 32, "Minimo:  --- ms");
        display_print_small(0, 43, "Promedio:--- ms");
    }

    snprintf(buf, sizeof(buf), "TX:%-4u RX:%-4u SF:%d",
             (unsigned)_rtt_tx_cnt, (unsigned)_rtt_rx_cnt, (int)lora_comm_get_sf());
    display_print_small(0, 54, buf);

    display_print_small(0, 62, "[OK]=salir");
    display_update();
}

static void _draw_gps(void)
{
    char buf[24];
    display_clear();
    display_print_small(14, 9, "= Precision GPS =");

    gps_data_t g = gps_get_data();
    if (g.valid) {
        snprintf(buf, sizeof(buf), "Lat:%+10.5f", g.latitude);
        display_print_small(0, 21, buf);
        snprintf(buf, sizeof(buf), "Lon:%+10.5f", g.longitude);
        display_print_small(0, 32, buf);
        snprintf(buf, sizeof(buf), "Alt: %.1fm", g.altitude);
        display_print_small(0, 43, buf);
        snprintf(buf, sizeof(buf), "Sat:%-3d Spd:%.1fkm/h", g.satellites, g.speed);
        display_print_small(0, 54, buf);
        display_print_small(0, 62, "Fix: 3D (valido)");
    } else {
        display_print_small(0, 21, "Lat: -- sin fix --");
        display_print_small(0, 32, "Lon: -- sin fix --");
        snprintf(buf, sizeof(buf), "Sat: %d  buscando..", g.satellites);
        display_print_small(0, 43, buf);
        display_print_small(0, 54, "Fix: NO VALIDO");
        display_print_small(0, 62, "[OK]=salir");
    }
    display_update();
}

static void _draw_config_sf(void)
{
    char buf[24];
    display_clear();
    display_print_small(22, 9, "= Config LoRa =");

    snprintf(buf, sizeof(buf), "SF: [ %d ]  rng:7-12", (int)_cfg_sf);
    display_print_small(0, 21, buf);

    display_print_small(0, 32, "BW: 125 kHz  CR:4/5");

    snprintf(buf, sizeof(buf), "Pwr: %d dBm  Freq:915", LORA_TX_POWER);
    display_print_small(0, 43, buf);

    snprintf(buf, sizeof(buf), "ToA aprox: ~%ums", (unsigned)_toa_ms(_cfg_sf));
    display_print_small(0, 54, buf);

    display_print_small(0, 62, _cfg_applied ? "Aplicado! [OK]=volver"
                                             : "[^][v]=SF  [OK]=aplica");
    display_update();
}

/* ─────────────────────────────────────────────────────────────────────
 * API pública
 * ───────────────────────────────────────────────────────────────────── */

void test_menu_init(void)
{
    _state       = TM_MENU;
    _cursor      = 0;
    _scroll      = 0;
    _done        = false;
    _cfg_sf      = lora_comm_get_sf();
    _cfg_applied = false;
}

void test_menu_update(void)
{
    gps_update();
    lora_comm_tick();

    /* Recibir paquete LoRa entrante */
    lora_msg_t msg;
    int16_t    rssi;
    float      snr;
    if (lora_comm_receive(&msg, &rssi, &snr))
        _handle_rx(&msg, rssi, snr);

    uint32_t now = millis();

    /* ── Menú principal ──────────────────────────────────────────────── */
    if (_state == TM_MENU) {
        if (btn_pressed(BTN_UP)) {
            _cursor = (_cursor > 0) ? _cursor - 1 : MENU_COUNT - 1;
            if (_cursor < _scroll)
                _scroll = _cursor;
            else if (_cursor >= _scroll + MENU_VISIBLE)
                _scroll = _cursor - MENU_VISIBLE + 1;
        }
        if (btn_pressed(BTN_DOWN)) {
            _cursor = (_cursor < MENU_COUNT - 1) ? _cursor + 1 : 0;
            if (_cursor >= _scroll + MENU_VISIBLE)
                _scroll = _cursor - MENU_VISIBLE + 1;
            else if (_cursor < _scroll)
                _scroll = _cursor;
        }
        if (btn_pressed(BTN_SELECT)) {
            switch (_cursor) {
            case 0:
                _state = TM_ALCANCE;
                _alc_tx = _alc_rx = 0;
                _alc_rssi = 0; _alc_snr = 0.0f;
                _alc_last_tx = 0;
                break;
            case 1:
                _state = TM_PDR;
                _pdr_sent = _pdr_recv = 0;
                _pdr_last_tx = _pdr_tx_ms = _pdr_rtt_ms = 0;
                break;
            case 2:
                _state = TM_RTT;
                _rtt_last_ms = _rtt_sum_ms = _rtt_count = 0;
                _rtt_min_ms  = UINT32_MAX;
                _rtt_waiting = false;
                _rtt_seq     = 0;
                _rtt_last_tx = _rtt_tx_cnt = _rtt_rx_cnt = 0;
                break;
            case 3:
                _state = TM_GPS;
                break;
            case 4:
                _state       = TM_CONFIG_SF;
                _cfg_sf      = lora_comm_get_sf();
                _cfg_applied = false;
                break;
            case 5:
                _state = TM_DEBUG;
                debug_menu_init();
                break;
            case 6:   /* Volver */
                _done = true;
                return;
            }
        }
        _draw_menu();
        return;
    }

    /* ── Alcance/RSSI ────────────────────────────────────────────────── */
    if (_state == TM_ALCANCE) {
        if (btn_pressed(BTN_SELECT)) { _state = TM_MENU; return; }
        if (now - _alc_last_tx >= 2000UL) {
            lora_msg_t ping; _build_ping(&ping, 0);
            lora_comm_send(&ping);
            _alc_tx++;
            _alc_last_tx = now;
        }
        _draw_alcance();
        return;
    }

    /* ── PDR ─────────────────────────────────────────────────────────── */
    if (_state == TM_PDR) {
        if (btn_pressed(BTN_SELECT)) {
            if (_pdr_sent >= PDR_TOTAL) {
                _pdr_sent = _pdr_recv = 0;
                _pdr_last_tx = _pdr_tx_ms = _pdr_rtt_ms = 0;
            } else {
                _state = TM_MENU;
                return;
            }
        }
        if (_pdr_sent < PDR_TOTAL && now - _pdr_last_tx >= 500UL) {
            lora_msg_t ping; _build_ping(&ping, _pdr_sent);
            lora_comm_send(&ping);
            _pdr_tx_ms   = millis();
            _pdr_last_tx = now;
            _pdr_sent++;
        }
        _draw_pdr();
        return;
    }

    /* ── RTT ─────────────────────────────────────────────────────────── */
    if (_state == TM_RTT) {
        if (btn_pressed(BTN_SELECT)) { _state = TM_MENU; return; }
        if (!_rtt_waiting && now - _rtt_last_tx >= 2000UL) {
            _rtt_seq++;
            lora_msg_t ping; _build_ping(&ping, _rtt_seq);
            lora_comm_send(&ping);
            _rtt_tx_ms   = millis();
            _rtt_waiting = true;
            _rtt_last_tx = now;
            _rtt_tx_cnt++;
        }
        /* Timeout PONG: 3 segundos sin respuesta */
        if (_rtt_waiting && millis() - _rtt_tx_ms > 3000UL)
            _rtt_waiting = false;
        _draw_rtt();
        return;
    }

    /* ── GPS Precisión ───────────────────────────────────────────────── */
    if (_state == TM_GPS) {
        if (btn_pressed(BTN_SELECT)) { _state = TM_MENU; return; }
        _draw_gps();
        return;
    }

    /* ── Config SF ───────────────────────────────────────────────────── */
    if (_state == TM_CONFIG_SF) {
        if (btn_pressed(BTN_UP))   { if (_cfg_sf < 12) { _cfg_sf++; _cfg_applied = false; } }
        if (btn_pressed(BTN_DOWN)) { if (_cfg_sf > 7)  { _cfg_sf--; _cfg_applied = false; } }
        if (btn_pressed(BTN_SELECT)) {
            if (_cfg_applied) {
                _state = TM_MENU;
                return;
            }
            lora_comm_set_sf(_cfg_sf);
            _cfg_applied = true;
            alert_beep_short();
        }
        _draw_config_sf();
        return;
    }

    /* ── Debug HW ─────────────────────────────────────────────────────── */
    if (_state == TM_DEBUG) {
        debug_menu_update();
        if (debug_menu_is_done()) {
            _state = TM_MENU;
        }
        return;
    }
}

bool test_menu_is_done(void) { return _done; }
