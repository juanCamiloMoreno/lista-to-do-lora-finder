#include "lora_comm.h"
#include "drivers/lora/lora.h"
#include "drivers/led/led.h"
#include "drivers/alert/alert.h"

#include <Arduino.h>
#include <string.h>

static uint32_t _device_id = 0;

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
    static uint32_t _last_hb = 0;
    if (millis() - _last_hb >= 5000) {
        _last_hb = millis();
        lora_start_rx();
    }
}

uint32_t lora_comm_device_id(void) { return _device_id; }

bool    lora_comm_set_sf(uint8_t sf) { return lora_set_sf(sf); }
uint8_t lora_comm_get_sf(void)       { return lora_get_sf();   }

bool lora_comm_send(const lora_msg_t *msg)
{
    bool ok = lora_send((const uint8_t *)msg, sizeof(lora_msg_t));
    if (ok) {
        led_flash_tx();
        alert_tx();
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
    return true;
}
