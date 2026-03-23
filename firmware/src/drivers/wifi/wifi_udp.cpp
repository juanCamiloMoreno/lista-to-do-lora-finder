#include "wifi_udp.h"
#include <Arduino.h>
#include <WiFi.h>
#include <WiFiUdp.h>
#include <string.h>

/* ── Estructura de paquete UDP ──────────────────────────────────────── */
#pragma pack(push, 1)
typedef struct {
    uint8_t  type;   /* 0 = ping, 1 = pong */
    uint32_t seq;
    uint32_t ts_ms;
} udp_pkt_t;
#pragma pack(pop)

#define UDP_PKT_PING  0u
#define UDP_PKT_PONG  1u

/* Timeout de conexion STA en ms */
#define STA_CONNECT_TIMEOUT_MS  8000u
/* Intervalo de reintento de conexion STA */
#define STA_RETRY_INTERVAL_MS   3000u

/* ── Estado interno ─────────────────────────────────────────────────── */
static WiFiUDP          _udp;
static wifi_udp_role_t  _role          = WIFI_UDP_IDLE;
static bool             _connected     = false;
static IPAddress        _remote_ip;          /* IP del ultimo paquete recibido (AP) */
static uint32_t         _tx_count      = 0;
static uint32_t         _rx_count      = 0;
static uint32_t         _seq_out       = 0;
static uint32_t         _rtt_last      = 0;
static uint32_t         _rtt_sum       = 0;
static uint32_t         _rtt_count_val = 0;
static uint32_t         _ping_ts       = 0;  /* timestamp del ultimo ping enviado */
static bool             _waiting_pong  = false;
static uint32_t         _sta_connect_start = 0;
static uint32_t         _sta_last_retry    = 0;

/* ── Helpers ────────────────────────────────────────────────────────── */

static void _send_pong(IPAddress to_ip, uint32_t seq, uint32_t orig_ts)
{
    udp_pkt_t pkt;
    pkt.type   = UDP_PKT_PONG;
    pkt.seq    = seq;
    pkt.ts_ms  = orig_ts;
    _udp.beginPacket(to_ip, WIFI_UDP_PORT);
    _udp.write((const uint8_t *)&pkt, sizeof(pkt));
    _udp.endPacket();
    _tx_count++;
}

/* ── API pública ────────────────────────────────────────────────────── */

void wifi_udp_start_ap(void)
{
    wifi_udp_stop();
    WiFi.mode(WIFI_AP);
    WiFi.softAP(WIFI_UDP_SSID, WIFI_UDP_PASS);
    _udp.begin(WIFI_UDP_PORT);
    _role      = WIFI_UDP_AP;
    _connected = true;   /* AP siempre "conectado" */
    _tx_count  = _rx_count = 0;
    _seq_out   = 0;
    _rtt_last  = _rtt_sum = _rtt_count_val = 0;
    _waiting_pong = false;
}

void wifi_udp_start_sta(void)
{
    wifi_udp_stop();
    WiFi.mode(WIFI_STA);
    WiFi.begin(WIFI_UDP_SSID, WIFI_UDP_PASS);
    _role      = WIFI_UDP_STA;
    _connected = false;
    _tx_count  = _rx_count = 0;
    _seq_out   = 0;
    _rtt_last  = _rtt_sum = _rtt_count_val = 0;
    _waiting_pong    = false;
    _ping_ts         = 0;
    _sta_connect_start = millis();
    _sta_last_retry    = millis();
}

void wifi_udp_stop(void)
{
    if (_role != WIFI_UDP_IDLE) {
        _udp.stop();
        WiFi.disconnect(true);
        WiFi.mode(WIFI_OFF);
    }
    _role      = WIFI_UDP_IDLE;
    _connected = false;
}

void wifi_udp_tick(void)
{
    if (_role == WIFI_UDP_IDLE) return;

    uint32_t now = millis();

    /* ── STA: verificar/esperar conexion ─── */
    if (_role == WIFI_UDP_STA && !_connected) {
        if (WiFi.status() == WL_CONNECTED) {
            _udp.begin(WIFI_UDP_PORT);
            _connected = true;
        } else {
            /* Reintentar si pasó el timeout */
            if (now - _sta_last_retry >= STA_RETRY_INTERVAL_MS) {
                WiFi.disconnect();
                WiFi.begin(WIFI_UDP_SSID, WIFI_UDP_PASS);
                _sta_last_retry = now;
            }
        }
        return;
    }

    /* ── Procesar paquetes entrantes ─── */
    int len = _udp.parsePacket();
    if (len >= (int)sizeof(udp_pkt_t)) {
        udp_pkt_t pkt;
        _udp.read((uint8_t *)&pkt, sizeof(pkt));

        if (pkt.type == UDP_PKT_PING) {
            /* AP: responder pong */
            _rx_count++;
            if (_role == WIFI_UDP_AP) {
                _remote_ip = _udp.remoteIP();
                _send_pong(_remote_ip, pkt.seq, pkt.ts_ms);
            }
        } else if (pkt.type == UDP_PKT_PONG) {
            /* STA: medir RTT */
            _rx_count++;
            if (_waiting_pong) {
                uint32_t rtt = now - _ping_ts;
                _rtt_last      = rtt;
                _rtt_sum      += rtt;
                _rtt_count_val++;
                _waiting_pong  = false;
            }
        }
    }
}

void wifi_udp_send_ping(void)
{
    if (_role != WIFI_UDP_STA || !_connected) return;

    _seq_out++;
    udp_pkt_t pkt;
    pkt.type  = UDP_PKT_PING;
    pkt.seq   = _seq_out;
    pkt.ts_ms = (uint32_t)millis();

    /* El AP esta en la IP de gateway del STA */
    IPAddress gw = WiFi.gatewayIP();
    _udp.beginPacket(gw, WIFI_UDP_PORT);
    _udp.write((const uint8_t *)&pkt, sizeof(pkt));
    _udp.endPacket();

    _tx_count++;
    _ping_ts      = (uint32_t)millis();
    _waiting_pong = true;
}

bool            wifi_udp_connected(void)   { return _connected; }
uint32_t        wifi_udp_tx(void)          { return _tx_count;  }
uint32_t        wifi_udp_rx(void)          { return _rx_count;  }

int16_t wifi_udp_rssi(void)
{
    if (_role == WIFI_UDP_STA && _connected)
        return (int16_t)WiFi.RSSI();
    return 0;
}

uint32_t wifi_udp_rtt_last_ms(void) { return _rtt_last; }

uint32_t wifi_udp_rtt_avg_ms(void)
{
    if (_rtt_count_val == 0) return 0;
    return _rtt_sum / _rtt_count_val;
}

wifi_udp_role_t wifi_udp_role(void) { return _role; }
