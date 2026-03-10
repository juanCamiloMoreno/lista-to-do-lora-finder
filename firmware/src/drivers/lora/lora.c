#include "lora.h"
#include "../../config/system_config.h"
#include "../../config/pins.h"

bool lora_init(void)
{
    /* TODO: initialize SX1262 / LoRa radio */
    return false;
}

bool lora_send(const uint8_t *data, size_t len)
{
    /* TODO: transmit packet */
    (void)data; (void)len;
    return false;
}

int lora_receive(uint8_t *buf, size_t max_len)
{
    /* TODO: receive packet, return bytes read or -1 */
    (void)buf; (void)max_len;
    return -1;
}
