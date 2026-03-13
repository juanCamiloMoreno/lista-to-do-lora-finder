#ifndef LORA_H
#define LORA_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

/* ─────────────────────────────────────────────────────────────────────────
 * Driver LoRa — SX1262 via RadioLib
 * Heltec WiFi LoRa 32 V4
 * ───────────────────────────────────────────────────────────────────────── */

/* Inicializa el radio SX1262 con los parámetros de system_config.h.
 * Devuelve true si OK. */
bool lora_init(void);

/* Transmite un paquete. Bloqueante hasta TX done (~100–500 ms).
 * Devuelve true si OK. */
bool lora_send(const uint8_t *data, size_t len);

/* Pone el radio en modo recepción continua.
 * Llama a lora_receive() en cada iteración del loop para recoger paquetes. */
bool lora_start_rx(void);

/* Recoge un paquete recibido (no bloqueante).
 * Retorna bytes leídos, 0 si no hay paquete, -1 en error.
 * rssi_out y snr_out pueden ser NULL si no interesan. */
int  lora_receive(uint8_t *buf, size_t max_len,
                  int16_t *rssi_out, float *snr_out);

/* true si el radio fue inicializado correctamente */
bool lora_is_ready(void);

#endif /* LORA_H */
