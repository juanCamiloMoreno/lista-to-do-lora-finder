#include "espnow_comm.h"
#include <Arduino.h>
#include <esp_now.h>
#include <WiFi.h>
#include <string.h>

/* ── Tipos de paquete ───────────────────────────────────────────────── */
#define ESPNOW_TYPE_DISC  0u
#define ESPNOW_TYPE_PING  1u
#define ESPNOW_TYPE_PONG  2u

#pragma pack(push, 1)
typedef struct {
    uint8_t  type;
    uint32_t seq;
    uint32_t ts_ms;
    uint8_t  mac[6];
} espnow_pkt_t;
#pragma pack(pop)

/* Intervalo de envio de discovery en ms */
#define DISC_INTERVAL_MS  500u

static const uint8_t _broadcast_mac[6] = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};

/* ── Estado interno ─────────────────────────────────────────────────── */
static bool     _initialized   = false;
static bool     _peer_found    = false;
static uint8_t  _peer_mac[6]   = {0};
static int16_t  _peer_rssi     = 0;

static uint32_t _tx_count      = 0;
static uint32_t _rx_count      = 0;
static uint32_t _seq_out       = 0;
static uint32_t _rtt_last      = 0;
static uint32_t _rtt_sum       = 0;
static uint32_t _rtt_count_val = 0;
static uint32_t _ping_ts       = 0;
static bool     _waiting_pong  = false;

static uint32_t _last_disc_tx  = 0;

/* Datos copiados desde callbacks (ISR-safe via flag) */
static volatile bool    _rx_ready    = false;
static espnow_pkt_t     _rx_buf;
static uint8_t          _rx_mac_buf[6];
static volatile int16_t _rx_rssi_buf = 0;

/* ── Callbacks (deben ser estaticos) ────────────────────────────────── */

static void _on_send(const uint8_t *mac_addr, esp_now_send_status_t status)
{
    (void)mac_addr;
    (void)status;
}

/* Firma compatible con Arduino ESP32 framework 3.x (ESP-IDF 5.x) */
static void _on_recv(const uint8_t *mac_addr, const uint8_t *data, int len)
{
    if (len < (int)sizeof(espnow_pkt_t)) return;
    if (_rx_ready) return;   /* descartar si el anterior no fue procesado */

    memcpy(&_rx_buf, data, sizeof(espnow_pkt_t));
    memcpy(_rx_mac_buf, mac_addr, 6);
    _rx_rssi_buf = 0;   /* RSSI no disponible en esta versión del callback */
    _rx_ready    = true;
}

/* ── Helpers internos ───────────────────────────────────────────────── */

static void _send_pkt(const uint8_t *dest_mac, uint8_t type,
                      uint32_t seq, uint32_t ts)
{
    espnow_pkt_t pkt;
    pkt.type  = type;
    pkt.seq   = seq;
    pkt.ts_ms = ts;
    /* Incluir nuestra propia MAC para que el receptor nos pueda agregar */
    WiFi.macAddress(pkt.mac);
    esp_now_send(dest_mac, (const uint8_t *)&pkt, sizeof(pkt));
    _tx_count++;
}

/* ── API publica ────────────────────────────────────────────────────── */

void espnow_init(void)
{
    espnow_stop();

    WiFi.mode(WIFI_STA);
    WiFi.disconnect();

    if (esp_now_init() != ESP_OK) return;

    esp_now_register_send_cb(_on_send);
    esp_now_register_recv_cb(_on_recv);

    /* Registrar peer broadcast para poder enviar discovery */
    esp_now_peer_info_t peer_info = {};
    memcpy(peer_info.peer_addr, _broadcast_mac, 6);
    peer_info.channel = 0;
    peer_info.encrypt = false;
    esp_now_add_peer(&peer_info);

    _initialized   = true;
    _peer_found    = false;
    memset(_peer_mac, 0, 6);
    _peer_rssi     = 0;
    _tx_count      = 0;
    _rx_count      = 0;
    _seq_out       = 0;
    _rtt_last      = 0;
    _rtt_sum       = 0;
    _rtt_count_val = 0;
    _ping_ts       = 0;
    _waiting_pong  = false;
    _last_disc_tx  = 0;
    _rx_ready      = false;
}

void espnow_stop(void)
{
    if (_initialized) {
        esp_now_deinit();
        WiFi.disconnect(true);
        WiFi.mode(WIFI_OFF);
        _initialized = false;
    }
    _peer_found = false;
    _rx_ready   = false;
}

void espnow_tick(void)
{
    if (!_initialized) return;

    uint32_t now = millis();

    /* ── Enviar discovery periodico hasta encontrar peer ─── */
    if (!_peer_found && (now - _last_disc_tx >= DISC_INTERVAL_MS)) {
        _send_pkt(_broadcast_mac, ESPNOW_TYPE_DISC, 0, now);
        /* No contar el DISC en _tx_count de datos */
        if (_tx_count > 0) _tx_count--;
        _last_disc_tx = now;
    }

    /* ── Procesar paquetes recibidos ─── */
    if (!_rx_ready) return;

    /* Copiar atomicamente */
    espnow_pkt_t pkt;
    uint8_t      src_mac[6];
    int16_t      rssi;

    memcpy(&pkt,    &_rx_buf,      sizeof(pkt));
    memcpy(src_mac, _rx_mac_buf,   6);
    rssi = _rx_rssi_buf;
    _rx_ready = false;

    _peer_rssi = rssi;

    if (pkt.type == ESPNOW_TYPE_DISC) {
        /* Guardar peer, responder con DISC para que sepa nuestra MAC */
        if (!_peer_found) {
            memcpy(_peer_mac, src_mac, 6);
            /* Agregar como peer unicast */
            esp_now_peer_info_t pi = {};
            memcpy(pi.peer_addr, src_mac, 6);
            pi.channel = 0;
            pi.encrypt = false;
            if (!esp_now_is_peer_exist(src_mac))
                esp_now_add_peer(&pi);
            _peer_found = true;
            /* Responder discovery para que el otro nos agregue */
            _send_pkt(src_mac, ESPNOW_TYPE_DISC, 0, millis());
            if (_tx_count > 0) _tx_count--;
        }
        return;
    }

    /* Asegurarse de que el peer esta registrado */
    if (!_peer_found) {
        memcpy(_peer_mac, src_mac, 6);
        esp_now_peer_info_t pi = {};
        memcpy(pi.peer_addr, src_mac, 6);
        pi.channel = 0;
        pi.encrypt = false;
        if (!esp_now_is_peer_exist(src_mac))
            esp_now_add_peer(&pi);
        _peer_found = true;
    }

    if (pkt.type == ESPNOW_TYPE_PING) {
        _rx_count++;
        /* Responder pong */
        _send_pkt(src_mac, ESPNOW_TYPE_PONG, pkt.seq, pkt.ts_ms);
        return;
    }

    if (pkt.type == ESPNOW_TYPE_PONG) {
        _rx_count++;
        if (_waiting_pong) {
            uint32_t rtt = millis() - _ping_ts;
            _rtt_last      = rtt;
            _rtt_sum      += rtt;
            _rtt_count_val++;
            _waiting_pong  = false;
        }
    }
}

void espnow_send_ping(void)
{
    if (!_initialized || !_peer_found) return;
    _seq_out++;
    _ping_ts      = millis();
    _waiting_pong = true;
    _send_pkt(_peer_mac, ESPNOW_TYPE_PING, _seq_out, _ping_ts);
}

bool     espnow_peer_found(void)   { return _peer_found;    }
uint32_t espnow_tx(void)           { return _tx_count;      }
uint32_t espnow_rx(void)           { return _rx_count;      }
uint32_t espnow_rtt_last_ms(void)  { return _rtt_last;      }
int16_t  espnow_peer_rssi(void)    { return _peer_rssi;     }

uint32_t espnow_rtt_avg_ms(void)
{
    if (_rtt_count_val == 0) return 0;
    return _rtt_sum / _rtt_count_val;
}
