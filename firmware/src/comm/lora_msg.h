#pragma once
#include <stdint.h>

/* ─────────────────────────────────────────────────────────────────────────
 * Protocolo de mensajes LoRa — struct empaquetado de 17 bytes.
 * Ambas placas usan el mismo binario y el mismo protocolo.
 * ───────────────────────────────────────────────────────────────────────── */

typedef enum : uint8_t {
    MSG_SEARCH_START    = 0x01,  /* Buscador → broadcast: inicia búsqueda  */
    MSG_ACK             = 0x02,  /* Objetivo → Buscador: acepta búsqueda   */
    MSG_STATUS          = 0x03,  /* Buscador → Objetivo: posición + rumbo  */
    MSG_BEACON          = 0x04,  /* Objetivo → Buscador: posición + rumbo  */
    MSG_SOS_ALERT       = 0x05,  /* Objetivo → Buscador: emergencia        */
    MSG_SOS_CANCEL      = 0x06,  /* Objetivo → Buscador: cancela SOS       */
    MSG_REUNITE_CONFIRM = 0x07,  /* Cualquiera → otro: confirmación reunión */
} lora_msg_type_t;

typedef struct __attribute__((packed)) {
    uint8_t  msg_type;     /* lora_msg_type_t                            */
    uint32_t sender_id;    /* ID único del dispositivo (últimos 4B MAC)  */
    int32_t  lat_i;        /* latitud  × 1 000 000  (0 si sin GPS)       */
    int32_t  lon_i;        /* longitud × 1 000 000  (0 si sin GPS)       */
    int16_t  heading_x10;  /* rumbo × 10  (0–3599)                       */
    uint8_t  bat_pct;      /* porcentaje batería 0–100                   */
    int8_t   rssi_last;    /* último RSSI recibido en dBm (0 = ninguno)  */
} lora_msg_t;              /* = 17 bytes                                  */

static_assert(sizeof(lora_msg_t) == 17, "lora_msg_t debe ser 17 bytes");
