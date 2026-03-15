#pragma once

/* Driver de alertas — buzzer activo (GPIO7) + motor vibrador (GPIO15)
 *
 * Todas las funciones son NO bloqueantes: programan la alerta y
 * regresan de inmediato. Llamar alert_tick() en cada ciclo de app_run(). */

void alert_init(void);
void alert_tick(void);      /* Llamar en app_run() cada ciclo */

/* Alertas individuales */
void alert_beep_short(void);       /* 1 pitido corto  ~80 ms  */
void alert_beep_long(void);        /* 1 pitido largo  ~400 ms */
void alert_beep_double(void);      /* 2 pitidos cortos         */
void alert_vib_short(void);        /* 1 vibración corta ~100 ms */
void alert_vib_long(void);         /* 1 vibración larga ~400 ms */

/* Alertas combinadas (buzzer + vibrador simultáneos) */
void alert_rx(void);               /* Paquete LoRa recibido    */
void alert_tx(void);               /* Paquete LoRa enviado     */
void alert_error(void);            /* Error — 3 pitidos rápidos */
