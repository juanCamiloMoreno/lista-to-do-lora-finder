#pragma once
#include "lora_msg.h"
#include <stdbool.h>
#include <stdint.h>

/* ─────────────────────────────────────────────────────────────────────────
 * Capa de protocolo sobre el driver LoRa de bajo nivel.
 * Serializa/deserializa lora_msg_t y gestiona LED + alerta en cada TX/RX.
 * ───────────────────────────────────────────────────────────────────────── */

/* Inicializar: pone el radio en RX continuo */
void     lora_comm_init(void);

/* Latido periódico: re-arma RX cada 5 s — llamar en cada loop */
void     lora_comm_tick(void);

/* Enviar un mensaje (bloqueante ~100-500 ms, luego vuelve a RX) */
bool     lora_comm_send(const lora_msg_t *msg);

/* Recoger un mensaje recibido (no bloqueante).
 * Devuelve true si hay paquete nuevo; rssi_out y snr_out pueden ser NULL. */
bool     lora_comm_receive(lora_msg_t *msg_out,
                           int16_t   *rssi_out,
                           float     *snr_out);

/* ID único de este dispositivo (últimos 4 bytes del MAC de la flash) */
uint32_t lora_comm_device_id(void);
