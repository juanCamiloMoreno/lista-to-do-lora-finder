#pragma once
#include <stdint.h>
#include <stdbool.h>

void     espnow_init(void);          /* Inicializar ESP-NOW + auto-discovery */
void     espnow_stop(void);
void     espnow_tick(void);          /* Llamar en loop */
void     espnow_send_ping(void);     /* Enviar ping al peer */

bool     espnow_peer_found(void);    /* Peer descubierto via broadcast */
uint32_t espnow_tx(void);
uint32_t espnow_rx(void);
uint32_t espnow_rtt_last_ms(void);
uint32_t espnow_rtt_avg_ms(void);
int16_t  espnow_peer_rssi(void);     /* RSSI del peer (del callback) */
