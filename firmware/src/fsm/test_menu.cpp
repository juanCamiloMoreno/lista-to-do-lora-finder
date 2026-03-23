#include "test_menu.h"
#include "debug_menu.h"
#include "comm/lora_comm.h"
#include "comm/lora_msg.h"
#include "config/system_config.h"
#include "config/nvs_config.h"   /* C-11 */
#include "drivers/display/display.h"
#include "drivers/gps/gps.h"
#include "drivers/btn/btn.h"
#include "drivers/alert/alert.h"
#include "drivers/led/led.h"
#include "drivers/wifi/wifi_scan.h"
#include "drivers/wifi/wifi_udp.h"
#include "drivers/wifi/espnow_comm.h"

#include <Arduino.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <limits.h>

/* ── Estados ─────────────────────────────────────────────────────────── */
typedef enum {
    TM_MENU = 0,
    TM_LORA_MENU,      /* submenu LoRa */
    TM_WIFI_MENU,      /* submenu WiFi */
    TM_ALCANCE,
    TM_PDR,
    TM_RTT,
    TM_PINGPONG,       /* C-08: prueba ping-pong bidireccional */
    TM_GPS,
    TM_CONFIG_SF,
    TM_CONFIG_PARAMS,  /* C-11: configuracion general con NVS  */
    TM_DEBUG,
    TM_WIFI_ALCANCE,   /* test WiFi RSSI caliente/frio */
    TM_WIFI_PINGPONG,  /* test WiFi UDP ping-pong */
    TM_WIFI_ESPNOW,    /* test ESP-NOW ping-pong */
} tm_state_t;

/* ── Menu principal ───────────────────────────────────────────────────── */
#define MENU_COUNT   6
#define MENU_VISIBLE 3
static const char * const _items[MENU_COUNT] = {
    "Pruebas LoRa",
    "Pruebas WiFi",
    "Config SF",
    "Config Params",  /* C-11 */
    "Debug HW",
    "<< Volver",
};

/* ── Menu LoRa ────────────────────────────────────────────────────────── */
#define LORA_MENU_COUNT   6
static const char * const _lora_items[LORA_MENU_COUNT] = {
    "Alcance/RSSI",
    "PDR paquetes",
    "T. Respuesta",
    "Ping-Pong",
    "Precision GPS",
    "<< Volver",
};

/* ── Menu WiFi ────────────────────────────────────────────────────────── */
#define WIFI_MENU_COUNT   4
static const char * const _wifi_items[WIFI_MENU_COUNT] = {
    "WiFi Alcance",
    "WiFi Ping-Pong",
    "WiFi ESP-NOW",
    "<< Volver",
};

/* ── Estado global ───────────────────────────────────────────────────── */
static tm_state_t _state       = TM_MENU;
static int        _cursor      = 0;
static int        _scroll      = 0;
static int        _lora_cursor = 0;
static int        _lora_scroll = 0;
static int        _wifi_cursor = 0;
static int        _wifi_scroll = 0;
static bool       _done        = false;

/* ── Alcance/RSSI ────────────────────────────────────────────────────── */
static uint32_t _alc_tx      = 0;
static uint32_t _alc_rx      = 0;
static int16_t  _alc_rssi    = 0;
static float    _alc_snr     = 0.0f;
static uint32_t _alc_last_tx = 0;
static bool     _alc_started = false;  /* UP para iniciar TX */

/* ── PDR ─────────────────────────────────────────────────────────────── */
#define PDR_TOTAL 20
static uint8_t  _pdr_sent    = 0;
static uint8_t  _pdr_recv    = 0;
static uint32_t _pdr_last_tx = 0;
static uint32_t _pdr_tx_ms   = 0;
static uint32_t _pdr_rtt_ms  = 0;
static bool     _pdr_started = false;  /* UP para iniciar TX */

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
static bool     _rtt_started = false;  /* UP para iniciar TX */

/* ── Ping-Pong (C-08) ─────────────────────────────────────────────────
 * Ambos nodos pueden iniciar y responder.
 * bat_pct  → numero de secuencia
 * rssi_last → RSSI que el nodo emisor midio del otro (RSSI remoto)
 * ─────────────────────────────────────────────────────────────────────── */
static uint8_t  _pp_seq          = 0;
static uint32_t _pp_tx_cnt       = 0;
static uint32_t _pp_rx_cnt       = 0;
static uint32_t _pp_lost         = 0;
static int16_t  _pp_rssi_local   = 0;   /* RSSI medido por este nodo          */
static int8_t   _pp_rssi_remote  = 0;   /* rssi_last del paquete recibido     */
static bool     _pp_auto_mode    = false; /* true = ping automatico tras PONG  */
static bool     _pp_waiting_pong = false; /* esperando PONG del seq actual     */
static uint32_t _pp_tx_ts        = 0;   /* timestamp del ultimo PING enviado  */
static uint32_t _pp_pong_rx_ts   = 0;   /* timestamp del ultimo PONG recibido */

/* ── Config SF ───────────────────────────────────────────────────────── */
static uint8_t  _cfg_sf      = LORA_SF;
static bool     _cfg_applied = false;

/* ── Config Params (C-11) ─────────────────────────────────────────────
 * Editor de parametros operativos con persistencia NVS.
 * _cp_sel: indice del parametro seleccionado (0-4).
 * Los cambios se aplican en RAM inmediatamente y se guardan al salir.
 * ─────────────────────────────────────────────────────────────────────── */
static int      _cp_sel     = 0;   /* parametro seleccionado */
static bool     _cp_saved   = false;

/* ── WiFi Alcance ────────────────────────────────────────────────────── */
static bool     _walc_is_ap = false;
static uint32_t _walc_scans = 0;

/* ── WiFi Ping-Pong UDP ──────────────────────────────────────────────── */
static bool _wpp_started = false;

/* ── WiFi Alcance scan started flag ─────────────────────────────────── */
static bool _walc_scan_started = false;

/* ─────────────────────────────────────────────────────────────────────
 * Helpers
 * ───────────────────────────────────────────────────────────────────── */

/* Estimacion de ToA para 17 bytes (BW=125kHz, CR=4/5, explicit header) */
static uint32_t _toa_ms(uint8_t sf)
{
    uint32_t tsym_us = ((uint32_t)1u << sf) * 8u;
    uint32_t t_pre   = tsym_us * 12u + tsym_us / 4u;
    int de  = (sf >= 11) ? 1 : 0;
    int num = 8 * 17 - 4 * (int)sf + 44;
    int den = 4 * ((int)sf - 2 * de);
    if (den <= 0) den = 4;
    int ns  = ((num + den - 1) / den) * 5 + 8;
    if (ns < 8) ns = 8;
    uint32_t t_pay = (uint32_t)ns * tsym_us;
    return (t_pre + t_pay) / 1000u;
}

/* Intervalo minimo de TX segun SF activo: evita enviar antes de que termine
 * el time-on-air del paquete anterior (C-01 fix para SF != 7). */
static uint32_t _safe_tx_interval_ms(void)
{
    uint32_t toa = _toa_ms(lora_comm_get_sf());
    uint32_t min_interval = toa * 2 + 300UL;   /* 2x ToA + 300 ms margen */
    return (min_interval < 2000UL) ? 2000UL : min_interval;
}

static void _build_ping(lora_msg_t *m, uint8_t seq)
{
    memset(m, 0, sizeof(*m));
    m->msg_type  = (uint8_t)MSG_TEST_PING;
    m->sender_id = lora_comm_device_id();
    m->bat_pct   = seq;
    /* Incluir RSSI local medido del otro nodo para ping-pong bidireccional */
    m->rssi_last = (int8_t)(_pp_rssi_local < -128 ? -128 : _pp_rssi_local);
}

static void _send_pong(uint8_t seq, int16_t measured_rssi)
{
    lora_msg_t p;
    memset(&p, 0, sizeof(p));
    p.msg_type  = (uint8_t)MSG_TEST_PONG;
    p.sender_id = lora_comm_device_id();
    p.bat_pct   = seq;
    /* Reportar al otro nodo el RSSI con que recibimos su paquete */
    p.rssi_last = (int8_t)(measured_rssi < -128 ? -128 : measured_rssi);
    lora_comm_send(&p);
}

/* ─────────────────────────────────────────────────────────────────────
 * Manejo de paquetes recibidos (todos los modos excepto Ping-Pong)
 * ───────────────────────────────────────────────────────────────────── */
static void _handle_rx(const lora_msg_t *msg, int16_t rssi, float snr)
{
    if (msg->msg_type == (uint8_t)MSG_TEST_PING) {
        _send_pong(msg->bat_pct, rssi);
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
 * Manejo de paquetes en modo Ping-Pong (C-08)
 * ───────────────────────────────────────────────────────────────────── */
static void _handle_rx_pingpong(const lora_msg_t *msg, int16_t rssi, float snr)
{
    if (msg->msg_type == (uint8_t)MSG_TEST_PING) {
        /* Recibimos un PING — responder siempre con PONG */
        _pp_rssi_local  = rssi;
        _pp_rssi_remote = msg->rssi_last;
        _pp_rx_cnt++;

        _send_pong(msg->bat_pct, rssi);
        _pp_tx_cnt++;   /* el PONG cuenta como TX */

#if DEBUG_LOG
        Serial.printf("[PP] RX PING seq=%d rssi_loc=%d rssi_rem=%d\n",
                      msg->bat_pct, (int)rssi, (int)msg->rssi_last);
#endif
        return;
    }

    if (msg->msg_type == (uint8_t)MSG_TEST_PONG) {
        /* Recibimos PONG en respuesta a nuestro PING */
        _pp_rssi_local  = rssi;
        _pp_rssi_remote = msg->rssi_last;
        _pp_rx_cnt++;

        /* Detectar perdida por salto de secuencia */
        if (_pp_waiting_pong && msg->bat_pct != _pp_seq) {
            _pp_lost++;
        }
        _pp_waiting_pong = false;
        _pp_pong_rx_ts   = millis();

#if DEBUG_LOG
        Serial.printf("[PP] RX PONG seq=%d rssi_loc=%d rssi_rem=%d\n",
                      msg->bat_pct, (int)rssi, (int)msg->rssi_last);
#endif
    }
}

/* ─────────────────────────────────────────────────────────────────────
 * Helpers de scroll de menu genericos
 * ───────────────────────────────────────────────────────────────────── */

static void _scroll_up(int *cursor, int *scroll, int count, int visible)
{
    *cursor = (*cursor > 0) ? *cursor - 1 : count - 1;
    if (*cursor < *scroll)
        *scroll = *cursor;
    else if (*cursor >= *scroll + visible)
        *scroll = *cursor - visible + 1;
}

static void _scroll_down(int *cursor, int *scroll, int count, int visible)
{
    *cursor = (*cursor < count - 1) ? *cursor + 1 : 0;
    if (*cursor >= *scroll + visible)
        *scroll = *cursor - visible + 1;
    else if (*cursor < *scroll)
        *scroll = *cursor;
}

/* ─────────────────────────────────────────────────────────────────────
 * Dibujo por estado
 * ───────────────────────────────────────────────────────────────────── */

/* Funcion de dibujo de scroll generica */
static void _draw_scroll_menu(const char *title,
                              const char * const *items, int count,
                              int cursor, int scroll, int visible)
{
    display_clear();
    display_print_small(14, 9, title);

    for (int i = 0; i < visible && (scroll + i) < count; i++) {
        int idx = scroll + i;
        int y   = 24 + i * 15;
        int fy  = y - 12;

        if (idx == cursor) {
            display_draw_rbox(0, fy, 128, 14, 3);
            display_set_font_mode(1);
            display_set_draw_color(0);
            display_print_medium(2, y, items[idx]);
            display_set_draw_color(1);
            display_set_font_mode(0);
        } else {
            display_draw_rframe(0, fy, 128, 14, 3);
            display_print_medium(2, y, items[idx]);
        }
    }

    if (scroll > 0)
        display_print_small(120, 9, "^");
    if (scroll + visible < count)
        display_print_small(120, 62, "v");

    display_update();
}

static void _draw_menu(void)
{
    _draw_scroll_menu("= Menu Pruebas =", _items, MENU_COUNT,
                      _cursor, _scroll, MENU_VISIBLE);
}

static void _draw_lora_menu(void)
{
    _draw_scroll_menu("= Pruebas LoRa =", _lora_items, LORA_MENU_COUNT,
                      _lora_cursor, _lora_scroll, MENU_VISIBLE);
}

static void _draw_wifi_menu(void)
{
    _draw_scroll_menu("= Pruebas WiFi =", _wifi_items, WIFI_MENU_COUNT,
                      _wifi_cursor, _wifi_scroll, MENU_VISIBLE);
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

    display_print_small(0, 62, _alc_started ? "TX activo [OK]=salir"
                                            : "[^]=iniciar [OK]=salir");
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

    display_print_small(0, 62, (_pdr_sent >= PDR_TOTAL) ? "[OK]=reiniciar"
                             : _pdr_started             ? "midiendo..."
                                                        : "[^]=iniciar [OK]=salir");
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

    display_print_small(0, 62, _rtt_started ? "TX activo [OK]=salir"
                                           : "[^]=iniciar [OK]=salir");
    display_update();
}

/* C-08: pantalla Ping-Pong */
static void _draw_pingpong(void)
{
    char buf[24];
    display_clear();
    display_print_small(16, 9, "= Ping-Pong =");

    snprintf(buf, sizeof(buf), "TX:%-4u  RX:%-4u", (unsigned)_pp_tx_cnt, (unsigned)_pp_rx_cnt);
    display_print_small(0, 21, buf);

    snprintf(buf, sizeof(buf), "Lost:%-3u  Seq:%d", (unsigned)_pp_lost, (int)_pp_seq);
    display_print_small(0, 32, buf);

    snprintf(buf, sizeof(buf), "RSSI loc:%4d dBm", (int)_pp_rssi_local);
    display_print_small(0, 43, buf);

    snprintf(buf, sizeof(buf), "RSSI rem:%4d dBm", (int)_pp_rssi_remote);
    display_print_small(0, 54, buf);

    if (_pp_auto_mode)
        display_print_small(0, 62, "AUTO [OK]=salir");
    else
        display_print_small(0, 62, "[^]=PING [OK]=salir");
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

/* C-11: pantalla Config Params */
static void _draw_config_params(void)
{
    char buf[24];
    nvs_config_t *c = nvs_config_get();
    display_clear();
    display_print_small(10, 9, "= Config Params =");

    /* 3 parametros: umbral RSSI, potencia TX y SF */
    struct { const char *lbl; int16_t val; const char *unit; } rows[3] = {
        { "RSSI umbral", c->rssi_threshold,   "dBm" },
        { "TX potencia", c->tx_power,          "dBm" },
        { "SF",          c->spreading_factor,  ""    },
    };

    for (int i = 0; i < 3; i++) {
        int y = 22 + i * 13;
        if (i == _cp_sel) {
            snprintf(buf, sizeof(buf), ">%-12s[%d]%s",
                     rows[i].lbl, (int)rows[i].val, rows[i].unit);
        } else {
            snprintf(buf, sizeof(buf), " %-12s %d%s",
                     rows[i].lbl, (int)rows[i].val, rows[i].unit);
        }
        display_print_small(0, y, buf);
    }

    if (_cp_saved)
        display_print_small(0, 62, "Guardado! [OK]=salir");
    else
        display_print_small(0, 62, "[^][v]=val [OK]=sig");
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

/* ── WiFi Alcance ────────────────────────────────────────────────────── */
static void _draw_wifi_alcance(void)
{
    char buf[24];
    display_clear();
    display_print_small(14, 9, "= WiFi Alcance =");

    if (_walc_is_ap) {
        display_print_small(0, 21, "Modo: AP (objetivo)");
        display_print_small(0, 32, "Esperando scanner..");
        snprintf(buf, sizeof(buf), "SSID: %s", WIFI_AP_SSID);
        display_print_small(0, 43, buf);
        display_print_small(0, 54, "");
    } else {
        int16_t rssi = wifi_scan_rssi();
        wifi_hot_t hot = wifi_scan_hot_cold();
        snprintf(buf, sizeof(buf), "RSSI:%4d dBm", (int)rssi);
        display_print_small(0, 21, buf);
        snprintf(buf, sizeof(buf), "[%-12s]", wifi_hot_label(hot));
        display_print_small(0, 32, buf);
        snprintf(buf, sizeof(buf), "Scans: %u", (unsigned)_walc_scans);
        display_print_small(0, 43, buf);
        display_print_small(0, 54, wifi_scan_visible() ? "Target visible" : "Buscando...");
    }

    display_print_small(0, 62, _walc_is_ap ? "[OK]=salir"
                                           : "[^]=ser AP  [OK]=salir");
    display_update();
}

/* ── WiFi UDP Ping-Pong ──────────────────────────────────────────────── */
static void _draw_wifi_pingpong(void)
{
    char buf[24];
    display_clear();
    display_print_small(10, 9, "= WiFi UDP P-P =");

    if (!_wpp_started) {
        display_print_small(0, 21, "No iniciado");
        display_print_small(0, 62, "[^]=AP [v]=STA [OK]=sal");
    } else {
        wifi_udp_role_t role = wifi_udp_role();
        bool conn = wifi_udp_connected();

        if (role == WIFI_UDP_AP) {
            display_print_small(0, 21, "Modo: AP");
        } else {
            display_print_small(0, 21, conn ? "Modo: STA (conectado)"
                                            : "Modo: STA (conectando)");
        }

        snprintf(buf, sizeof(buf), "TX:%-4u RX:%-4u", (unsigned)wifi_udp_tx(), (unsigned)wifi_udp_rx());
        display_print_small(0, 32, buf);

        uint32_t tx = wifi_udp_tx();
        uint32_t rx = wifi_udp_rx();
        unsigned pdr = (tx > 0) ? (unsigned)(rx * 100u / tx) : 0u;
        snprintf(buf, sizeof(buf), "RSSI:%4d  PDR:%3u%%", (int)wifi_udp_rssi(), pdr);
        display_print_small(0, 43, buf);

        snprintf(buf, sizeof(buf), "RTT:%4ums avg:%4ums",
                 (unsigned)wifi_udp_rtt_last_ms(), (unsigned)wifi_udp_rtt_avg_ms());
        display_print_small(0, 54, buf);

        display_print_small(0, 62, "[^]=AP [v]=ping [OK]=sal");
    }
    display_update();
}

/* ── WiFi ESP-NOW ────────────────────────────────────────────────────── */
static void _draw_wifi_espnow(void)
{
    char buf[24];
    display_clear();
    display_print_small(10, 9, "= WiFi ESP-NOW =");

    if (!espnow_peer_found()) {
        display_print_small(10, 32, "Buscando peer...");
        display_print_small(0, 62, "[OK]=salir");
    } else {
        snprintf(buf, sizeof(buf), "TX:%-4u RX:%-4u", (unsigned)espnow_tx(), (unsigned)espnow_rx());
        display_print_small(0, 21, buf);

        uint32_t tx = espnow_tx();
        uint32_t rx = espnow_rx();
        unsigned pdr = (tx > 0) ? (unsigned)(rx * 100u / tx) : 0u;
        snprintf(buf, sizeof(buf), "PDR:%3u%%", pdr);
        display_print_small(0, 32, buf);

        snprintf(buf, sizeof(buf), "RTT:%4ums avg:%4ums",
                 (unsigned)espnow_rtt_last_ms(), (unsigned)espnow_rtt_avg_ms());
        display_print_small(0, 43, buf);

        snprintf(buf, sizeof(buf), "RSSI peer:%4d dBm", (int)espnow_peer_rssi());
        display_print_small(0, 54, buf);

        display_print_small(0, 62, "[^]=ping  [OK]=salir");
    }
    display_update();
}

/* ─────────────────────────────────────────────────────────────────────
 * API publica
 * ───────────────────────────────────────────────────────────────────── */

void test_menu_init(void)
{
    _state       = TM_MENU;
    _cursor      = 0;
    _scroll      = 0;
    _lora_cursor = 0;
    _lora_scroll = 0;
    _wifi_cursor = 0;
    _wifi_scroll = 0;
    _done        = false;
    _cfg_sf      = lora_comm_get_sf();
    _cfg_applied = false;
}

void test_menu_update(void)
{
    btn_update();
    gps_update();
    lora_comm_tick();

    /* Recibir paquete LoRa entrante */
    lora_msg_t msg;
    int16_t    rssi;
    float      snr;
    bool got_pkt = lora_comm_receive(&msg, &rssi, &snr);

    /* Routing de paquetes segun estado activo */
    if (got_pkt) {
        if (_state == TM_PINGPONG)
            _handle_rx_pingpong(&msg, rssi, snr);
        else
            _handle_rx(&msg, rssi, snr);
    }

    uint32_t now = millis();

    /* ── Menu principal ──────────────────────────────────────────────── */
    if (_state == TM_MENU) {
        if (btn_pressed(BTN_UP))
            _scroll_up(&_cursor, &_scroll, MENU_COUNT, MENU_VISIBLE);
        if (btn_pressed(BTN_DOWN))
            _scroll_down(&_cursor, &_scroll, MENU_COUNT, MENU_VISIBLE);
        if (btn_pressed(BTN_SELECT)) {
            btn_flush();
            switch (_cursor) {
            case 0:   /* Pruebas LoRa */
                _state       = TM_LORA_MENU;
                _lora_cursor = 0;
                _lora_scroll = 0;
                break;
            case 1:   /* Pruebas WiFi */
                _state       = TM_WIFI_MENU;
                _wifi_cursor = 0;
                _wifi_scroll = 0;
                break;
            case 2:   /* Config SF */
                _state       = TM_CONFIG_SF;
                _cfg_sf      = lora_comm_get_sf();
                _cfg_applied = false;
                lora_comm_set_state("TM_CONFIG_SF");
                break;
            case 3:   /* Config Params */
                _state    = TM_CONFIG_PARAMS;
                _cp_sel   = 0;
                _cp_saved = false;
                lora_comm_set_state("TM_CONFIG_PAR");
                break;
            case 4:   /* Debug HW */
                _state = TM_DEBUG;
                debug_menu_init();
                break;
            case 5:   /* Volver */
                _done = true;
                return;
            }
        }
        _draw_menu();
        return;
    }

    /* ── Submenu LoRa ────────────────────────────────────────────────── */
    if (_state == TM_LORA_MENU) {
        if (btn_pressed(BTN_UP))
            _scroll_up(&_lora_cursor, &_lora_scroll, LORA_MENU_COUNT, MENU_VISIBLE);
        if (btn_pressed(BTN_DOWN))
            _scroll_down(&_lora_cursor, &_lora_scroll, LORA_MENU_COUNT, MENU_VISIBLE);
        if (btn_pressed(BTN_SELECT)) {
            btn_flush();
            switch (_lora_cursor) {
            case 0:   /* Alcance/RSSI */
                _state = TM_ALCANCE;
                _alc_tx = _alc_rx = 0;
                _alc_rssi = 0; _alc_snr = 0.0f;
                _alc_last_tx = 0;
                _alc_started = false;
                lora_comm_set_state("TM_ALCANCE");
                break;
            case 1:   /* PDR */
                _state = TM_PDR;
                _pdr_sent = _pdr_recv = 0;
                _pdr_last_tx = _pdr_tx_ms = _pdr_rtt_ms = 0;
                _pdr_started = false;
                lora_comm_set_state("TM_PDR");
                break;
            case 2:   /* RTT */
                _state = TM_RTT;
                _rtt_last_ms = _rtt_sum_ms = _rtt_count = 0;
                _rtt_min_ms  = UINT32_MAX;
                _rtt_waiting = false;
                _rtt_seq     = 0;
                _rtt_last_tx = _rtt_tx_cnt = _rtt_rx_cnt = 0;
                _rtt_started = false;
                lora_comm_set_state("TM_RTT");
                break;
            case 3:   /* Ping-Pong */
                _state = TM_PINGPONG;
                _pp_seq = _pp_tx_cnt = _pp_rx_cnt = _pp_lost = 0;
                _pp_rssi_local = _pp_rssi_remote = 0;
                _pp_auto_mode    = false;
                _pp_waiting_pong = false;
                _pp_tx_ts = _pp_pong_rx_ts = 0;
                lora_comm_set_state("TM_PINGPONG");
                break;
            case 4:   /* GPS */
                _state = TM_GPS;
                gps_enable();
                led_set_color(128, 0, 128);
                lora_comm_set_state("TM_GPS");
                break;
            case 5:   /* Volver */
                _state = TM_MENU;
                break;
            }
        }
        _draw_lora_menu();
        return;
    }

    /* ── Submenu WiFi ────────────────────────────────────────────────── */
    if (_state == TM_WIFI_MENU) {
        if (btn_pressed(BTN_UP))
            _scroll_up(&_wifi_cursor, &_wifi_scroll, WIFI_MENU_COUNT, MENU_VISIBLE);
        if (btn_pressed(BTN_DOWN))
            _scroll_down(&_wifi_cursor, &_wifi_scroll, WIFI_MENU_COUNT, MENU_VISIBLE);
        if (btn_pressed(BTN_SELECT)) {
            btn_flush();
            switch (_wifi_cursor) {
            case 0:   /* WiFi Alcance */
                _state             = TM_WIFI_ALCANCE;
                _walc_is_ap        = false;
                _walc_scans        = 0;
                _walc_scan_started = false;
                break;
            case 1:   /* WiFi Ping-Pong */
                _state       = TM_WIFI_PINGPONG;
                _wpp_started = false;
                break;
            case 2:   /* WiFi ESP-NOW */
                _state = TM_WIFI_ESPNOW;
                espnow_init();
                break;
            case 3:   /* Volver */
                _state = TM_MENU;
                break;
            }
        }
        _draw_wifi_menu();
        return;
    }

    /* ── Alcance/RSSI ────────────────────────────────────────────────── */
    if (_state == TM_ALCANCE) {
        if (btn_pressed(BTN_SELECT)) { btn_flush(); _state = TM_LORA_MENU; return; }
        if (btn_pressed(BTN_UP) && !_alc_started) {
            _alc_started = true;
            _alc_last_tx = 0;   /* forzar TX inmediato al iniciar */
        }

        /* Solo el iniciador transmite (el responder solo contesta via _handle_rx) */
        if (_alc_started) {
            uint32_t tx_interval = _safe_tx_interval_ms();
            if (now - _alc_last_tx >= tx_interval) {
                lora_msg_t ping; _build_ping(&ping, 0);
                lora_comm_send(&ping);
                _alc_tx++;
                _alc_last_tx = now;
            }
        }
        _draw_alcance();
        return;
    }

    /* ── PDR ─────────────────────────────────────────────────────────── */
    if (_state == TM_PDR) {
        if (btn_pressed(BTN_SELECT)) {
            if (_pdr_sent >= PDR_TOTAL) {
                /* Reiniciar secuencia */
                _pdr_sent = _pdr_recv = 0;
                _pdr_last_tx = _pdr_tx_ms = _pdr_rtt_ms = 0;
                _pdr_started = false;
            } else {
                btn_flush(); _state = TM_LORA_MENU;
                return;
            }
        }
        if (btn_pressed(BTN_UP) && !_pdr_started) {
            _pdr_started = true;
            _pdr_last_tx = 0;   /* forzar TX inmediato al iniciar */
        }

        /* Solo el iniciador transmite */
        if (_pdr_started && _pdr_sent < PDR_TOTAL && now - _pdr_last_tx >= 500UL) {
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
        if (btn_pressed(BTN_SELECT)) { btn_flush(); _state = TM_LORA_MENU; return; }
        if (btn_pressed(BTN_UP) && !_rtt_started) {
            _rtt_started = true;
            _rtt_last_tx = 0;   /* forzar TX inmediato al iniciar */
        }

        /* Solo el iniciador transmite */
        if (_rtt_started && !_rtt_waiting && now - _rtt_last_tx >= 2000UL) {
            _rtt_seq++;
            lora_msg_t ping; _build_ping(&ping, _rtt_seq);
            lora_comm_send(&ping);
            _rtt_tx_ms   = millis();
            _rtt_waiting = true;
            _rtt_last_tx = now;
            _rtt_tx_cnt++;
        }
        if (_rtt_waiting && millis() - _rtt_tx_ms > 3000UL)
            _rtt_waiting = false;
        _draw_rtt();
        return;
    }

    /* ── Ping-Pong (C-08) ────────────────────────────────────────────── */
    if (_state == TM_PINGPONG) {
        if (btn_pressed(BTN_SELECT)) { btn_flush(); _state = TM_LORA_MENU; return; }

        if (btn_pressed(BTN_UP)) {
            /* Primera pulsacion: activar modo auto; sucesivas: enviar PING manual */
            if (!_pp_auto_mode) {
                _pp_auto_mode = true;
            }
            /* Enviar PING */
            _pp_seq++;
            lora_msg_t ping; _build_ping(&ping, _pp_seq);
            lora_comm_send(&ping);
            _pp_tx_cnt++;
            _pp_tx_ts        = now;
            _pp_waiting_pong = true;
#if DEBUG_LOG
            Serial.printf("[PP] TX PING seq=%d\n", _pp_seq);
#endif
        }

        /* Timeout PONG: 3x ToA sin respuesta → contar como perdido */
        uint32_t pong_timeout = _toa_ms(lora_comm_get_sf()) * 3 + 500UL;
        if (_pp_waiting_pong && (now - _pp_tx_ts) > pong_timeout) {
            _pp_lost++;
            _pp_waiting_pong = false;
        }

        /* Auto-ping: enviar PING siguiente tras delay post-PONG */
        if (_pp_auto_mode && !_pp_waiting_pong && _pp_pong_rx_ts > 0) {
            uint32_t auto_interval = _safe_tx_interval_ms();
            if ((now - _pp_pong_rx_ts) >= auto_interval) {
                _pp_pong_rx_ts = 0;   /* evitar re-trigger */
                _pp_seq++;
                lora_msg_t ping; _build_ping(&ping, _pp_seq);
                lora_comm_send(&ping);
                _pp_tx_cnt++;
                _pp_tx_ts        = now;
                _pp_waiting_pong = true;
#if DEBUG_LOG
                Serial.printf("[PP] AUTO PING seq=%d\n", _pp_seq);
#endif
            }
        }

        _draw_pingpong();
        return;
    }

    /* ── GPS Precision ───────────────────────────────────────────────── */
    if (_state == TM_GPS) {
        if (btn_pressed(BTN_SELECT)) {
            gps_disable();
            led_off();
            btn_flush(); _state = TM_LORA_MENU; return;
        }
        _draw_gps();
        return;
    }

    /* ── Config SF ───────────────────────────────────────────────────── */
    if (_state == TM_CONFIG_SF) {
        if (btn_pressed(BTN_UP))   { if (_cfg_sf < 12) { _cfg_sf++; _cfg_applied = false; } }
        if (btn_pressed(BTN_DOWN)) { if (_cfg_sf > 7)  { _cfg_sf--; _cfg_applied = false; } }
        if (btn_pressed(BTN_SELECT)) {
            if (_cfg_applied) {
                btn_flush(); _state = TM_MENU;
                return;
            }
            lora_comm_set_sf(_cfg_sf);
            nvs_config_get()->spreading_factor = _cfg_sf;
            nvs_config_save();
            _cfg_applied = true;
            alert_beep_short();
        }
        _draw_config_sf();
        return;
    }

    /* ── Config Params (C-11) ────────────────────────────────────────── */
    if (_state == TM_CONFIG_PARAMS) {
        nvs_config_t *c = nvs_config_get();

        if (!_cp_saved) {
            /* UP/DOWN cambian el valor del parametro seleccionado */
            if (btn_pressed(BTN_UP)) {
                switch (_cp_sel) {
                case 0: if (c->rssi_threshold   < -10) c->rssi_threshold++;   break;
                case 1: if (c->tx_power         < 22)  c->tx_power++;         break;
                case 2: if (c->spreading_factor < 12)  c->spreading_factor++; break;
                }
            }
            if (btn_pressed(BTN_DOWN)) {
                switch (_cp_sel) {
                case 0: if (c->rssi_threshold   > -120) c->rssi_threshold--;   break;
                case 1: if (c->tx_power         > 2)    c->tx_power--;         break;
                case 2: if (c->spreading_factor > 7)    c->spreading_factor--; break;
                }
            }
            /* SELECT: avanzar al siguiente parametro; al terminar → guardar */
            if (btn_pressed(BTN_SELECT)) {
                if (_cp_sel < 2) {
                    _cp_sel++;
                } else {
                    /* Ultimo parametro: guardar en NVS y aplicar SF */
                    nvs_config_save();
                    lora_comm_set_sf(c->spreading_factor);
                    _cp_saved = true;
                    alert_beep_short();
                }
            }
        } else {
            /* Ya guardado: cualquier pulsacion vuelve al menu */
            if (btn_pressed(BTN_SELECT) || btn_pressed(BTN_UP) || btn_pressed(BTN_DOWN)) {
                btn_flush(); _state = TM_MENU;
                return;
            }
        }
        _draw_config_params();
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

    /* ── WiFi Alcance ────────────────────────────────────────────────── */
    if (_state == TM_WIFI_ALCANCE) {
        if (btn_pressed(BTN_SELECT)) {
            if (_walc_is_ap)
                wifi_ap_stop();
            else
                wifi_scan_stop();
            btn_flush();
            _state = TM_WIFI_MENU;
            return;
        }
        if (btn_pressed(BTN_UP) && !_walc_is_ap) {
            /* Convertirse en AP */
            wifi_scan_stop();
            wifi_ap_start();
            _walc_is_ap = true;
        }

        /* Tick del scanner */
        if (!_walc_is_ap) {
            if (!_walc_scan_started) {
                wifi_scan_start();
                _walc_scan_started = true;
            }
            wifi_scan_tick();
            if (wifi_scan_visible()) _walc_scans++;
        }

        _draw_wifi_alcance();
        return;
    }

    /* ── WiFi UDP Ping-Pong ──────────────────────────────────────────── */
    if (_state == TM_WIFI_PINGPONG) {
        if (btn_pressed(BTN_SELECT)) {
            if (_wpp_started)
                wifi_udp_stop();
            btn_flush();
            _state = TM_WIFI_MENU;
            return;
        }
        if (!_wpp_started) {
            if (btn_pressed(BTN_UP)) {
                /* Ser AP */
                wifi_udp_start_ap();
                _wpp_started = true;
            } else if (btn_pressed(BTN_DOWN)) {
                /* Ser STA */
                wifi_udp_start_sta();
                _wpp_started = true;
            }
        } else {
            wifi_udp_tick();
            /* STA: DOWN para enviar ping manual */
            if (wifi_udp_role() == WIFI_UDP_STA && btn_pressed(BTN_DOWN))
                wifi_udp_send_ping();
            /* AP no enviar pings manualmente */
        }
        _draw_wifi_pingpong();
        return;
    }

    /* ── WiFi ESP-NOW ────────────────────────────────────────────────── */
    if (_state == TM_WIFI_ESPNOW) {
        if (btn_pressed(BTN_SELECT)) {
            espnow_stop();
            btn_flush();
            _state = TM_WIFI_MENU;
            return;
        }
        espnow_tick();
        if (espnow_peer_found() && btn_pressed(BTN_UP))
            espnow_send_ping();
        _draw_wifi_espnow();
        return;
    }
}

bool test_menu_is_done(void) { return _done; }
