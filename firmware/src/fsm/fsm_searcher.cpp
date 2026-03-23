#include "fsm_searcher.h"
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
#include <math.h>

/* ── Contexto interno ─────────────────────────────────────────────────── */

static searcher_state_t _state     = SEARCHER_DONE;
static uint32_t         _peer_id   = 0;   /* sender_id del objetivo         */
static int               _retries  = 0;   /* reintentos de SEARCH_START     */
static uint32_t         _timer     = 0;   /* timestamp de inicio de estado  */
static uint32_t         _last_tx   = 0;   /* último envío STATUS            */
static uint32_t         _last_rx   = 0;   /* último paquete recibido        */

/* Posición del objetivo (del último BEACON/ACK) */
static float   _tgt_lat   = 0.0f;
static float   _tgt_lon   = 0.0f;
static bool    _tgt_gps   = false;

/* Telemetría RSSI — local = medido por este nodo; remoto = reportado en paquete */
static int16_t _tgt_rssi        = 0;   /* RSSI local (lo que este nodo mide) */
static int16_t _prev_rssi       = 0;   /* para hot/cold                      */
static int8_t  _tgt_rssi_remote = 0;   /* rssi_last del paquete (C-07)       */

/* Distancia y rumbo calculados */
static float   _dist_m    = 0.0f;
static float   _bearing   = 0.0f;

/* Estado de SOS */
static uint32_t _sos_alert_ts = 0;

/* ── Suavizado RSSI para detección "encontrado" (C-09) ───────────────── */
/* Media móvil + contador de confirmaciones + histéresis para evitar
 * falsas detecciones con un solo paquete fuerte (H-02). */
static int16_t  _rssi_history[RSSI_AVG_WINDOW];
static uint8_t  _rssi_hist_idx   = 0;
static uint8_t  _rssi_hist_count = 0;
static uint8_t  _rssi_confirm    = 0;   /* confirmaciones por encima del umbral */
static bool     _rssi_above      = false; /* estado actual de histéresis         */

static void _rssi_history_push(int16_t rssi)
{
    _rssi_history[_rssi_hist_idx] = rssi;
    _rssi_hist_idx = (_rssi_hist_idx + 1) % RSSI_AVG_WINDOW;
    if (_rssi_hist_count < RSSI_AVG_WINDOW) _rssi_hist_count++;

    if (rssi > RSSI_THRESHOLD_FOUND) {
        if (_rssi_confirm < RSSI_CONFIRM_COUNT) _rssi_confirm++;
    } else {
        _rssi_confirm = 0;
    }
}

static void _rssi_history_reset(void)
{
    _rssi_hist_idx   = 0;
    _rssi_hist_count = 0;
    _rssi_confirm    = 0;
    _rssi_above      = false;
}

static int16_t _rssi_avg(void)
{
    if (_rssi_hist_count == 0) return _tgt_rssi;
    int32_t sum = 0;
    for (uint8_t i = 0; i < _rssi_hist_count; i++) sum += _rssi_history[i];
    return (int16_t)(sum / _rssi_hist_count);
}

/* ── Helpers de mensajes ──────────────────────────────────────────────── */

static lora_msg_t _build_msg(lora_msg_type_t type)
{
    lora_msg_t m = {};
    m.msg_type    = (uint8_t)type;
    m.sender_id   = lora_comm_device_id();

    gps_data_t g  = gps_get_data();
    m.lat_i       = g.valid ? (int32_t)(g.latitude  * 1e6) : 0;
    m.lon_i       = g.valid ? (int32_t)(g.longitude * 1e6) : 0;

    compass_data_t c;
    compass_read(&c);
    m.heading_x10 = (int16_t)(c.heading * 10.0f);

    battery_data_t b;
    battery_read(&b);
    m.bat_pct     = b.percent;
    m.rssi_last   = (int8_t)(_tgt_rssi < -128 ? -128 : _tgt_rssi);
    return m;
}

/* Helper: dibuja ícono de batería en esquina superior izquierda (C-05) */
static void _draw_battery_corner(void)
{
    battery_data_t bat;
    battery_read(&bat);
    battery_draw_icon(0, 0, &bat);
}

/* ── Pantallas de cada estado ─────────────────────────────────────────── */

static void _draw_searching(void)
{
    char buf[24];
    display_clear();

    _draw_battery_corner();   /* C-05 */

    display_print_medium(0, 14, "Buscando...");
    snprintf(buf, sizeof(buf), "Intento %d/%d", _retries, SEARCH_MAX_RETRIES);
    display_print_small(0, 28, buf);

    uint32_t elapsed = millis() - _timer;
    uint32_t total   = (uint32_t)SEARCH_MAX_RETRIES * SEARCH_RETRY_MS;
    int bar_w  = 110;
    int filled = (int)((long)bar_w * elapsed / total);
    if (filled > bar_w) filled = bar_w;

    display_draw_line(0, 38, bar_w, 38);
    display_draw_line(0, 44, bar_w, 44);
    display_draw_line(0, 38, 0,     44);
    display_draw_line(bar_w, 38, bar_w, 44);
    for (int i = 1; i < filled; i++)
        display_draw_line(i, 39, i, 43);

    display_print_small(0, 57, "[OK]=Cancelar");
    display_update();
}

static void _draw_nav(bool is_gps)
{
    char buf[24];
    display_clear();

    _draw_battery_corner();   /* C-05 */

    if (is_gps) {
        /* Distancia */
        if (_dist_m < 1000.0f)
            snprintf(buf, sizeof(buf), "%.0fm", _dist_m);
        else
            snprintf(buf, sizeof(buf), "%.2fkm", _dist_m / 1000.0f);
        display_print_medium(0, 22, buf);

        /* Rumbo texto */
        snprintf(buf, sizeof(buf), "%s %.0f", nav_cardinal(_bearing), _bearing);
        display_print_small(0, 36, buf);

        /* Flecha apuntando al objetivo */
        compass_draw_arrow_thick(NAV_ARROW_CX, NAV_ARROW_CY,
                                 _bearing - 90.0f,   /* conv: 0=Este en display */
                                 NAV_ARROW_LEN);
    } else {
        /* Modo RSSI */
        snprintf(buf, sizeof(buf), "RSSI: %d", (int)_tgt_rssi);
        display_print_medium(0, 22, buf);

        int diff = (int)_tgt_rssi - (int)_prev_rssi;
        if      (diff >  3) display_print_small(0, 36, "CALIENTE >");
        else if (diff < -3) display_print_small(0, 36, "< FRIO");
        else                display_print_small(0, 36, "  IGUAL");

        /* RSSI remoto (lo que el objetivo midió de nosotros) — C-07 */
        snprintf(buf, sizeof(buf), "Rem:%d", (int)_tgt_rssi_remote);
        display_print_small(0, 48, buf);
    }

    /* RSSI local en esquina inferior derecha; RSSI remoto en RSSI mode ya mostrado */
    snprintf(buf, sizeof(buf), "L:%d", (int)_tgt_rssi);
    display_print_small(78, 57, buf);

    display_print_small(0, 57, "[OK]=Fin");
    display_update();
}

static void _draw_signal_lost(void)
{
    char buf[24];
    display_clear();

    _draw_battery_corner();   /* C-05 */

    display_print_medium(0, 16, "Senal perdida");
    uint32_t secs = (millis() - _last_rx) / 1000;
    snprintf(buf, sizeof(buf), "Hace %lus", (unsigned long)secs);
    display_print_small(0, 30, buf);
    snprintf(buf, sizeof(buf), "Ultimo RSSI: %d", (int)_tgt_rssi);
    display_print_small(0, 42, buf);
    display_print_small(0, 57, "[OK]=Cancelar");
    display_update();
}

static void _draw_reunited(void)
{
    display_clear();

    _draw_battery_corner();   /* C-05 */

    display_print_medium(10, 20, "!REUNIDOS!");
    char buf[20];
    if (_tgt_gps)
        snprintf(buf, sizeof(buf), "Dist: %.0fm", _dist_m);
    else
        snprintf(buf, sizeof(buf), "RSSI: %d dBm", (int)_tgt_rssi);
    display_print_small(0, 38, buf);
    display_print_small(0, 57, "[OK]=Terminar");
    display_update();
}

static void _draw_sos(void)
{
    display_clear();

    _draw_battery_corner();   /* C-05 */

    display_print_medium(20, 16, "! SOS !");
    display_print_small(0, 30, "El objetivo");
    display_print_small(0, 40, "necesita ayuda");
    display_print_small(0, 57, "[OK]=Entendido");
    display_update();
}

/* ── Transiciones ─────────────────────────────────────────────────────── */

static void _go_searching(void)
{
    _state    = SEARCHER_SEARCHING;
    _retries  = 0;
    _timer    = millis();
    _last_rx  = millis();
    _tgt_gps  = false;
    _tgt_rssi = 0;
    _tgt_rssi_remote = 0;
    _rssi_history_reset();

    lora_comm_set_state("SRCH_SEARCHING");
    lora_msg_t m = _build_msg(MSG_SEARCH_START);
    lora_comm_send(&m);
    _retries = 1;
    _last_tx = millis();
}

static void _go_nav(int16_t rssi, const lora_msg_t *ack)
{
    _prev_rssi = rssi;
    _tgt_rssi  = rssi;
    _tgt_rssi_remote = ack->rssi_last;   /* C-07: RSSI que el objetivo midió */
    _peer_id   = ack->sender_id;

    _tgt_lat   = ack->lat_i / 1e6f;
    _tgt_lon   = ack->lon_i / 1e6f;
    _tgt_gps   = (ack->lat_i != 0 || ack->lon_i != 0);

    _last_rx   = millis();
    _last_tx   = millis();
    _rssi_history_reset();
    _rssi_history_push(rssi);

    if (_tgt_gps) {
        gps_data_t my = gps_get_data();
        if (my.valid) {
            _dist_m  = nav_distance_m((float)my.latitude,  (float)my.longitude,
                                       _tgt_lat, _tgt_lon);
            _bearing = nav_bearing_deg((float)my.latitude,  (float)my.longitude,
                                        _tgt_lat, _tgt_lon);
        }
        _state = SEARCHER_NAV_GPS;
        lora_comm_set_state("SRCH_NAV_GPS");
    } else {
        _state = SEARCHER_NAV_RSSI;
        lora_comm_set_state("SRCH_NAV_RSSI");
    }
}

static void _go_reunited(void)
{
    _state = SEARCHER_REUNITED;
    _timer = millis();
    alert_beep_double();
    alert_vib_long();

    lora_comm_set_state("SRCH_REUNITED");
    lora_msg_t m = _build_msg(MSG_REUNITE_CONFIRM);
    lora_comm_send(&m);
}

/* ── Actualización de navegación ──────────────────────────────────────── */

static void _update_nav_data(const lora_msg_t *msg, int16_t rssi)
{
    _prev_rssi       = _tgt_rssi;
    _tgt_rssi        = rssi;
    _tgt_rssi_remote = msg->rssi_last;   /* C-07 */

    /* Actualizar historia RSSI para suavizado (C-09) */
    _rssi_history_push(rssi);

    bool new_gps = (msg->lat_i != 0 || msg->lon_i != 0);
    if (new_gps) {
        _tgt_lat = msg->lat_i / 1e6f;
        _tgt_lon = msg->lon_i / 1e6f;
        _tgt_gps = true;
    }

    if (_tgt_gps) {
        gps_data_t my = gps_get_data();
        if (my.valid) {
            _dist_m  = nav_distance_m((float)my.latitude,  (float)my.longitude,
                                       _tgt_lat, _tgt_lon);
            _bearing = nav_bearing_deg((float)my.latitude,  (float)my.longitude,
                                        _tgt_lat, _tgt_lon);
        }
    }
}

/* ── Detección de reunión con suavizado RSSI (C-09) ──────────────────── */

static bool _check_reunited(void)
{
    /* GPS: distancia física < umbral */
    if (_tgt_gps) {
        gps_data_t my = gps_get_data();
        if (my.valid && _dist_m < REUNITE_DIST_M) return true;
    }

    /* RSSI: requiere media móvil + confirmaciones + histéresis
     * Evita detección prematura con un solo paquete fuerte (H-02). */
    if (_rssi_hist_count >= RSSI_AVG_WINDOW) {
        int16_t avg       = _rssi_avg();
        int     threshold = _rssi_above
                            ? (RSSI_THRESHOLD_FOUND - RSSI_HYSTERESIS)   /* salida */
                            : RSSI_THRESHOLD_FOUND;                       /* entrada */

        if (avg > threshold && _rssi_confirm >= RSSI_CONFIRM_COUNT) {
            _rssi_above = true;
            return true;
        }
        if (avg <= (RSSI_THRESHOLD_FOUND - RSSI_HYSTERESIS)) {
            _rssi_above = false;   /* restablecer histéresis */
        }
    }
    return false;
}

/* ── API pública ──────────────────────────────────────────────────────── */

void fsm_searcher_init(void)
{
    _go_searching();
}

void fsm_searcher_update(void)
{
    gps_update();
    lora_comm_tick();

    lora_msg_t msg;
    int16_t    rssi;
    float      snr;
    bool got = lora_comm_receive(&msg, &rssi, &snr);

    switch (_state) {

    /* ── BUSCANDO ──────────────────────────────────────────────────────── */
    case SEARCHER_SEARCHING:
        /* Cancelar con SELECT */
        if (btn_pressed(BTN_SELECT)) { _state = SEARCHER_DONE; return; }

        /* Reintentar SEARCH_START */
        if (millis() - _last_tx >= SEARCH_RETRY_MS) {
            if (_retries >= SEARCH_MAX_RETRIES) {
                /* UC-04: timeout sin respuesta */
                _state = SEARCHER_DONE;
                display_clear();
                display_print_medium(0, 24, "Sin respuesta");
                display_print_small (0, 40, "Nadie encontrado");
                display_update();
                delay(2500);
                return;
            }
            lora_msg_t m = _build_msg(MSG_SEARCH_START);
            lora_comm_send(&m);
            _retries++;
            _last_tx = millis();
        }

        /* Esperar ACK */
        if (got && msg.msg_type == (uint8_t)MSG_ACK) {
            _go_nav(rssi, &msg);
            alert_beep_short();
        }

        _draw_searching();
        break;

    /* ── NAVEGACIÓN GPS ────────────────────────────────────────────────── */
    case SEARCHER_NAV_GPS:
    case SEARCHER_NAV_RSSI: {
        bool nav_gps = (_state == SEARCHER_NAV_GPS);

        /* Cancelar */
        if (btn_pressed(BTN_SELECT)) { _state = SEARCHER_DONE; return; }

        /* Paquete entrante */
        if (got && msg.sender_id == _peer_id) {
            _last_rx = millis();
            if (msg.msg_type == (uint8_t)MSG_BEACON) {
                _update_nav_data(&msg, rssi);
                /* Actualizar estado FSM según disponibilidad GPS */
                if (_tgt_gps && _state == SEARCHER_NAV_RSSI) {
                    _state = SEARCHER_NAV_GPS;
                    lora_comm_set_state("SRCH_NAV_GPS");
                }
                if (!_tgt_gps && _state == SEARCHER_NAV_GPS) {
                    _state = SEARCHER_NAV_RSSI;
                    lora_comm_set_state("SRCH_NAV_RSSI");
                }
                if (_check_reunited()) { _go_reunited(); return; }
            } else if (msg.msg_type == (uint8_t)MSG_SOS_ALERT) {
                _state = SEARCHER_SOS;
                _sos_alert_ts = 0;
                lora_comm_set_state("SRCH_SOS");
                alert_error();
                return;
            } else if (msg.msg_type == (uint8_t)MSG_REUNITE_CONFIRM) {
                _go_reunited(); return;
            }
        }

        /* Señal perdida */
        if (millis() - _last_rx > SIGNAL_LOST_MS) {
            _state = SEARCHER_SIGNAL_LOST;
            _timer = millis();
            lora_comm_set_state("SRCH_LOST");
            return;
        }

        /* Enviar STATUS periódico */
        if (millis() - _last_tx >= STATUS_INTERVAL_MS) {
            lora_msg_t m = _build_msg(MSG_STATUS);
            lora_comm_send(&m);
            _last_tx = millis();
        }

        _draw_nav(nav_gps);
        break;
    }

    /* ── SEÑAL PERDIDA ─────────────────────────────────────────────────── */
    case SEARCHER_SIGNAL_LOST:
        if (btn_pressed(BTN_SELECT)) { _state = SEARCHER_DONE; return; }

        /* Paquete recuperado */
        if (got && msg.sender_id == _peer_id) {
            _last_rx = millis();
            _update_nav_data(&msg, rssi);
            _state = _tgt_gps ? SEARCHER_NAV_GPS : SEARCHER_NAV_RSSI;
            lora_comm_set_state(_tgt_gps ? "SRCH_NAV_GPS" : "SRCH_NAV_RSSI");
            return;
        }

        /* STATUS periódico para que el objetivo nos siga */
        if (millis() - _last_tx >= STATUS_INTERVAL_MS) {
            lora_msg_t m = _build_msg(MSG_STATUS);
            lora_comm_send(&m);
            _last_tx = millis();
        }

        /* Rendirse */
        if (millis() - _timer > SIGNAL_GIVEUP_MS) {
            _state = SEARCHER_DONE;
            return;
        }

        _draw_signal_lost();
        break;

    /* ── REUNIDOS ──────────────────────────────────────────────────────── */
    case SEARCHER_REUNITED:
        _draw_reunited();
        if (btn_pressed(BTN_SELECT) ||
            millis() - _timer > REUNITED_SHOW_MS) {
            _state = SEARCHER_DONE;
        }
        break;

    /* ── SOS ───────────────────────────────────────────────────────────── */
    case SEARCHER_SOS:
        /* Alerta periódica cada 5 s */
        if (millis() - _sos_alert_ts >= 5000) {
            _sos_alert_ts = millis();
            alert_error();
        }

        /* Paquete entrante */
        if (got && msg.sender_id == _peer_id) {
            _last_rx = millis();
            _update_nav_data(&msg, rssi);
            if (msg.msg_type == (uint8_t)MSG_SOS_CANCEL) {
                _state = _tgt_gps ? SEARCHER_NAV_GPS : SEARCHER_NAV_RSSI;
                lora_comm_set_state(_tgt_gps ? "SRCH_NAV_GPS" : "SRCH_NAV_RSSI");
                return;
            }
        }

        /* STATUS periódico */
        if (millis() - _last_tx >= STATUS_INTERVAL_MS) {
            lora_msg_t m = _build_msg(MSG_STATUS);
            lora_comm_send(&m);
            _last_tx = millis();
        }

        /* [SELECT] = "entendido", seguir navegando hacia el SOS */
        if (btn_pressed(BTN_SELECT)) {
            _state = _tgt_gps ? SEARCHER_NAV_GPS : SEARCHER_NAV_RSSI;
            lora_comm_set_state(_tgt_gps ? "SRCH_NAV_GPS" : "SRCH_NAV_RSSI");
        }

        _draw_sos();
        break;

    case SEARCHER_DONE:
        break;
    }
}

searcher_state_t fsm_searcher_get_state(void) { return _state; }
bool             fsm_searcher_is_done(void)    { return _state == SEARCHER_DONE; }
