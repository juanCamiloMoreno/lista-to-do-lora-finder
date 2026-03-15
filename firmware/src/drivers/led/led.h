#pragma once
#include <stdint.h>

/* LED RGB WS2812B — destellos de estado LoRa */

void led_init(void);
void led_flash_tx(void);                          /* Destello azul  — paquete enviado  */
void led_flash_rx(void);                          /* Destello verde — paquete recibido */
void led_set_color(uint8_t r, uint8_t g, uint8_t b); /* Color fijo hasta nuevo cambio */
void led_off(void);                               /* Apagar LED                        */
