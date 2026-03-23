#include "fsm_target.h"
#include "drivers/wifi/wifi_scan.h"
#include "comm/lora_comm.h"
#include "nav/navigation.h"
#include "config/system_config.h"
#include "drivers/display/display.h"
#include "drivers/compass/compass.h"
#include "drivers/gps/gps.h"
#include "drivers/battery/battery.h"
#include "drivers/btn/btn.h"
#include "drivers/alert/alert.h"

#include <Arduino.h>
#include <stdio.h>

/* ── Contexto interno ─────────────────────────────────────────────────── */

static target_state_t _state     = TARGET_DONE;
static uint32_t       _peer_id   = 0;   /* sender_id del buscador           */
static uint32_t       _timer     = 0;   /* timestamp de inicio de estado     */
static uint32_t       _last_tx   = 0;   /* último BEACON enviado             */
static uint32_t       _last_rx   = 0;   /* último paquete del buscador       */
static uint32_t       _alert_ts  = 0;   /* para alertas periódicas           */
static uint32_t       _sos_ts    = 0;   /* último SOS_ALERT enviado          */

/* RSSI local  = lo que este nodo (objetivo) mide del buscador */
static int16_t        _peer_rssi        = 0;
/* RSSI remoto = lo que el buscador reporta haber medido de nosotros (C-07) */
static int8_t         _peer_rssi_remote = 0;

/* Para detección de hold UP → SOS */
static uint32_t _up_hold_start = 0;
static bool     _up_holding    = false;

/* ── Helpers ──────────────────────────────────────────────────────────── */

static lora_msg_t _build_msg(lora_msg_type_t type)
{
    lora_msg_t m = {};
    m.msg_type  = (uint8_t)type;
    m.sender_id = lora_comm_device_id();

    gps_data_t g = gps_get_data();
    m.lat_i      = g.valid ? (int32_t)(g.latitude  * 1e6) : 0;
    m.lon_i      = g.valid ? (int32_t)(g.longitude * 1e6) : 0;

    compass_data_t c;
    compass_read(&c);
    m.heading_x10 = (int16_t)(c.heading * 10.0f);

    battery_data_t b;
    battery_read(&b);
    m.bat_pct   = b.percent;
    m.rssi_last = (int8_t)(_peer_rssi < -128 ? -128 : _peer_rssi);
    return m;
}

/* Helper: dibuja ícono de batería en esquina superior izquierda (C-05) */
static void _draw_battery_corner(void)
{
    battery_data_t bat;
    battery_read(&bat);
    battery_draw_icon(0, 0, &bat);
}

/* Detecta si BTN_UP lleva >2 s sostenido */
static bool _sos_triggered(void)
{
    if (btn_held(BTN_UP)) {
        if (!_up_holding) {
            _up_holding    = true;
            _up_hold_start = millis();
        } else if (millis() - _up_hold_start >= 2000) {
            _up_holding = false;
            return true;
        }
    } else {
        _up_holding = false;
    }
    return false;
}

/* ── Pantallas ────────────────────────────────────────────────────────── */

static void _draw_alerting(void)
{
    display_clear();

    _draw_battery_corner();   /* C-05 */

    display_print_medium(15, 14, "! Te buscan !");
    display_print_small (0,  28, "Alguien necesita");
    display_print_small (0,  38, "encontrarte.");
    display_print_small (0,  50, "[OK] = Aceptar");
    display_print_small (0,  60, "Ignora: timeout");
    display_update();
}

static void _draw_beacon(void)
{
    char buf[24];
    display_clear();

    _draw_battery_corner();   /* C-05 */

    display_print_small(20, 8, "Modo baliza");

    gps_data_t g = gps_get_data();
    if (g.valid) {
        snprintf(buf, sizeof(buf), "GPS  Sat:%d", g.satellites);
        display_print_small(0, 22, buf);
        snprintf(buf, sizeof(buf), "%.5f", g.latitude);
        display_print_small(0, 33, buf);
        snprintf(buf, sizeof(buf), "%.5f", g.longitude);
        display_print_small(0, 43, buf);
    } else {
        snprintf(buf, sizeof(buf), "GPS-- Sat:%d", g.satellites);
        display_print_small(0, 22, buf);
        /* RSSI local (lo que este nodo mide del buscador) */
        snprintf(buf, sizeof(buf), "RSSI loc: %d", (int)_peer_rssi);
        display_print_small(0, 33, buf);
        /* RSSI remoto (lo que el buscador mide de nosotros) — C-07 */
        snprintf(buf, sizeof(buf), "RSSI rem: %d", (int)_peer_rssi_remote);
        display_print_small(0, 43, buf);
    }

    display_print_small(0,  57, "[UP 2s]=SOS  [OK]=Fin");
    display_update();
}

static void _draw_sos(void)
{
    display_clear();

    _draw_battery_corner();   /* C-05 */

    display_print_medium(15, 16, "! SOS ACTIVO !");
    display_print_small (0,  30, "Alertando al");
    display_print_small (0,  40, "buscador...");
    display_print_small (0,  57, "[OK]=Cancelar SOS");
    display_update();
}

static void _draw_reunited(void)
{
    display_clear();

    _draw_battery_corner();   /* C-05 */

    display_print_medium(10, 20, "! REUNIDOS !");
    display_print_small (0,  40, "Mision completada");
    display_update();
}

/* ── API pública ──────────────────────────────────────────────────────── */

void fsm_target_init(const lora_msg_t *first_msg, int16_t rssi)
{
    _peer_id          = first_msg->sender_id;
    _peer_rssi        = rssi;
    _peer_rssi_remote = first_msg->rssi_last;   /* C-07 */
    _state            = TARGET_ALERTING;
    _timer            = millis();
    _alert_ts         = 0;
    _last_tx          = millis();
    _last_rx          = millis();
    _up_holding       = false;

    lora_comm_set_state("TGT_ALERTING");

    /* Alerta inmediata al usuario */
    alert_error();
}

void fsm_target_update(void)
{
    gps_update();
    lora_comm_tick();

    lora_msg_t msg;
    int16_t    rssi;
    float      snr;
    bool got = lora_comm_receive(&msg, &rssi, &snr);

    /* Filtrar paquetes de otros dispositivos */
    if (got && msg.sender_id != _peer_id) got = false;

    switch (_state) {

    /* ── ALERTANDO ─────────────────────────────────────────────────────── */
    case TARGET_ALERTING:
        /* Alerta periódica al usuario (cada 3 s) */
        if (millis() - _alert_ts >= 3000) {
            _alert_ts = millis();
            alert_beep_double();
            alert_vib_short();
        }

        /* [SELECT] = usuario acepta */
        if (btn_pressed(BTN_SELECT)) {
            /* Enviar ACK */
            lora_msg_t ack = _build_msg(MSG_ACK);
            lora_comm_send(&ack);
            _last_tx = millis();
            wifi_ap_start();
            _state   = TARGET_BEACON;
            lora_comm_set_state("TGT_BEACON");
            return;
        }

        /* Timeout: nadie pulsó [SELECT] */
        if (millis() - _timer > ALERT_ACK_TIMEOUT_MS) {
            wifi_ap_stop();
            _state = TARGET_DONE;
            return;
        }

        _draw_alerting();
        break;

    /* ── BALIZA ────────────────────────────────────────────────────────── */
    case TARGET_BEACON:
        /* Procesar STATUS del buscador */
        if (got) {
            _last_rx          = millis();
            _peer_rssi        = rssi;
            _peer_rssi_remote = msg.rssi_last;   /* C-07 */
            if (msg.msg_type == (uint8_t)MSG_REUNITE_CONFIRM) {
                wifi_ap_stop();
                _state = TARGET_REUNITED;
                _timer = millis();
                lora_comm_set_state("TGT_REUNITED");
                alert_beep_double();
                alert_vib_long();
                return;
            }
        }

        /* Enviar BEACON periódico */
        if (millis() - _last_tx >= STATUS_INTERVAL_MS) {
            lora_msg_t b = _build_msg(MSG_BEACON);
            lora_comm_send(&b);
            _last_tx = millis();
        }

        /* [SELECT] = terminar sesión */
        if (btn_pressed(BTN_SELECT)) {
            lora_msg_t rc = _build_msg(MSG_REUNITE_CONFIRM);
            lora_comm_send(&rc);
            wifi_ap_stop();
            _state = TARGET_DONE;
            return;
        }

        /* Hold UP 2 s → SOS */
        if (_sos_triggered()) {
            _state  = TARGET_SOS;
            _sos_ts = 0;
            lora_comm_set_state("TGT_SOS");
            alert_error();
            return;
        }

        _draw_beacon();
        break;

    /* ── SOS ───────────────────────────────────────────────────────────── */
    case TARGET_SOS:
        /* Enviar SOS_ALERT periódico (cada 1 s) */
        if (millis() - _sos_ts >= SOS_INTERVAL_MS) {
            _sos_ts = millis();
            lora_msg_t sos = _build_msg(MSG_SOS_ALERT);
            lora_comm_send(&sos);
            alert_beep_short();
        }

        /* Procesar STATUS del buscador (para confirmar que escucha) */
        if (got) {
            _last_rx          = millis();
            _peer_rssi        = rssi;
            _peer_rssi_remote = msg.rssi_last;   /* C-07 */
        }

        /* [SELECT] = cancelar SOS */
        if (btn_pressed(BTN_SELECT)) {
            lora_msg_t cancel = _build_msg(MSG_SOS_CANCEL);
            lora_comm_send(&cancel);
            _state = TARGET_BEACON;
            lora_comm_set_state("TGT_BEACON");
            return;
        }

        _draw_sos();
        break;

    /* ── REUNIDOS ──────────────────────────────────────────────────────── */
    case TARGET_REUNITED:
        _draw_reunited();
        if (millis() - _timer > REUNITED_SHOW_MS) {
            wifi_ap_stop();
            _state = TARGET_DONE;
        }
        break;

    case TARGET_DONE:
        break;
    }
}

target_state_t fsm_target_get_state(void) { return _state; }
bool           fsm_target_is_done(void)   { return _state == TARGET_DONE; }
