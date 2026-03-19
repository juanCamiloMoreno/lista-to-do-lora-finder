#pragma once
#include "comm/lora_msg.h"
#include <stdbool.h>
#include <stdint.h>

/* ─────────────────────────────────────────────────────────────────────────
 * FSM del rol OBJETIVO (Target / Node B).
 * Se activa cuando este dispositivo recibe un MSG_SEARCH_START.
 * ───────────────────────────────────────────────────────────────────────── */

typedef enum {
    TARGET_ALERTING,  /* Alertando al usuario — espera [SELECT] para aceptar */
    TARGET_BEACON,    /* Enviando balizas periódicas al buscador              */
    TARGET_SOS,       /* Modo SOS activo — enviando alertas de emergencia     */
    TARGET_REUNITED,  /* Reunión confirmada                                   */
    TARGET_DONE,      /* Sesión terminada — volver a ROLE_NONE                */
} target_state_t;

/* Inicializar el FSM con el primer mensaje recibido y su RSSI */
void          fsm_target_init(const lora_msg_t *first_msg, int16_t rssi);
void          fsm_target_update(void);
target_state_t fsm_target_get_state(void);
bool          fsm_target_is_done(void);
