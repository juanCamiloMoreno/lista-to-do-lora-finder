#include "lora_comm.h"
#include "drivers/lora/lora.h"
#include "drivers/led/led.h"
#include "drivers/alert/alert.h"
#include "config/system_config.h"

#include <Arduino.h>
#include <string.h>

static uint32_t     _device_id  = 0;
static const char  *_state_name = "INIT";  /* nombre del estado FSM actual */

void lora_comm_init(void)
{
    uint64_t mac = ESP.getEfuseMac();
    /* Combinar los 6 bytes del MAC en 4 bytes únicos */
    _device_id = (uint32_t)(mac & 0xFFFFFFFFULL) ^
                 (uint32_t)((mac >> 32) & 0xFFFF);
    lora_start_rx();
}

void lora_comm_tick(void)
{
    /* Intervalo 10 s (era 5 s): SF12 tiene ToA ~1.5 s; con 5 s podíamos
     * reiniciar RX en medio de una recepción y perder el paquete + su RSSI.
     * Con 10 s el riesgo de interferencia se reduce significativamente. */
    static uint32_t _last_hb = 0;
    if (millis() - _last_hb >= 10000) {
        _last_hb = millis();
        lora_start_rx();
    }
}

uint32_t lora_comm_device_id(void) { return _device_id; }

bool    lora_comm_set_sf(uint8_t sf) { return lora_set_sf(sf); }
uint8_t lora_comm_get_sf(void)       { return lora_get_sf();   }

/* Actualiza el nombre de estado que aparece en los logs TX/RX */
void lora_comm_set_state(const char *state_name)
{
    _state_name = state_name ? state_name : "?";
}

bool lora_comm_send(const lora_msg_t *msg)
{
    bool ok = lora_send((const uint8_t *)msg, sizeof(lora_msg_t));
    if (ok) {
        led_flash_tx();
        alert_tx();
#if DEBUG_LOG
        Serial.printf("[TX] t=%lu | SF=%d | type=0x%02X | state=%s\n",
                      (unsigned long)millis(), lora_get_sf(),
                      msg->msg_type, _state_name);
#endif
    }
    lora_start_rx();
    return ok;
}

bool lora_comm_receive(lora_msg_t *msg_out, int16_t *rssi_out, float *snr_out)
{
    uint8_t buf[sizeof(lora_msg_t)];
    int16_t rssi = 0;
    float   snr  = 0.0f;

    int n = lora_receive(buf, sizeof(buf), &rssi, &snr);
    if (n != (int)sizeof(lora_msg_t)) return false;

    memcpy(msg_out, buf, sizeof(lora_msg_t));
    if (rssi_out) *rssi_out = rssi;
    if (snr_out)  *snr_out  = snr;

    led_flash_rx();
#if DEBUG_LOG
    /* Advertencia si RSSI es cero — posible bug de lectura (H-06) */
    if (rssi == 0) {
        Serial.printf("[RX] t=%lu | SF=%d | RSSI=0 (WARN: posible fallo lectura SF!=7) | type=0x%02X | state=%s\n",
                      (unsigned long)millis(), lora_get_sf(),
                      msg_out->msg_type, _state_name);
    } else {
        Serial.printf("[RX] t=%lu | SF=%d | RSSI=%d | SNR=%.1f | type=0x%02X | state=%s\n",
                      (unsigned long)millis(), lora_get_sf(),
                      (int)rssi, snr, msg_out->msg_type, _state_name);
    }
#endif
    return true;
}
