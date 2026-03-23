#include "nvs_config.h"
#include "system_config.h"

#include <Preferences.h>
#include <Arduino.h>

/* Namespace NVS — máximo 15 caracteres */
#define NVS_NS   "lf_cfg"

static Preferences  _prefs;
static nvs_config_t _cfg;

/* ── Carga con defaults ──────────────────────────────────────────────── */

void nvs_config_init(void)
{
    _prefs.begin(NVS_NS, /*readOnly=*/true);

    _cfg.rssi_threshold   = (int16_t)_prefs.getShort("rssi_thr",  RSSI_THRESHOLD_FOUND);
    _cfg.rssi_avg_window  = _prefs.getUChar("rssi_win",  RSSI_AVG_WINDOW);
    _cfg.rssi_confirm     = _prefs.getUChar("rssi_cnt",  RSSI_CONFIRM_COUNT);
    _cfg.tx_power         = (int8_t)_prefs.getChar ("tx_pwr",   LORA_TX_POWER);
    _cfg.spreading_factor = _prefs.getUChar("sf",        LORA_SF);

    _prefs.end();

    /* Sanear rangos para evitar valores fuera de bounds en firmware */
    if (_cfg.rssi_threshold < -120 || _cfg.rssi_threshold > -10)
        _cfg.rssi_threshold = RSSI_THRESHOLD_FOUND;
    if (_cfg.rssi_avg_window < 1 || _cfg.rssi_avg_window > 30)
        _cfg.rssi_avg_window = RSSI_AVG_WINDOW;
    if (_cfg.rssi_confirm < 1 || _cfg.rssi_confirm > 20)
        _cfg.rssi_confirm = RSSI_CONFIRM_COUNT;
    if (_cfg.tx_power < 2 || _cfg.tx_power > 22)
        _cfg.tx_power = LORA_TX_POWER;
    if (_cfg.spreading_factor < 7 || _cfg.spreading_factor > 12)
        _cfg.spreading_factor = LORA_SF;

    Serial.printf("[nvs] cfg: rssi_thr=%d win=%d cnt=%d pwr=%d sf=%d\n",
                  _cfg.rssi_threshold, _cfg.rssi_avg_window, _cfg.rssi_confirm,
                  _cfg.tx_power, _cfg.spreading_factor);
}

/* ── Persistir en NVS ────────────────────────────────────────────────── */

void nvs_config_save(void)
{
    _prefs.begin(NVS_NS, /*readOnly=*/false);

    _prefs.putShort("rssi_thr", _cfg.rssi_threshold);
    _prefs.putUChar("rssi_win", _cfg.rssi_avg_window);
    _prefs.putUChar("rssi_cnt", _cfg.rssi_confirm);
    _prefs.putChar ("tx_pwr",   _cfg.tx_power);
    _prefs.putUChar("sf",       _cfg.spreading_factor);

    _prefs.end();
    Serial.println("[nvs] config guardada");
}

/* ── Acceso a config en RAM ──────────────────────────────────────────── */

nvs_config_t *nvs_config_get(void)
{
    return &_cfg;
}
