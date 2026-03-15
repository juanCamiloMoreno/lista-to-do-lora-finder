#pragma once
#include <stdbool.h>

/* ─────────────────────────────────────────────────────────────────────────
 * Modo reposo de pantalla — ROLE_NONE
 *
 * Si no hay actividad (botón o paquete LoRa) durante STANDBY_TIMEOUT_MS,
 * se apaga el panel OLED (setPowerSave). El CPU y el LoRa siguen activos.
 * Cualquier botón o paquete entrante reactiva la pantalla.
 * ───────────────────────────────────────────────────────────────────────── */

void power_standby_init(void);       /* llamar al entrar en ROLE_NONE      */
void power_standby_reset(void);      /* hay actividad → reiniciar timeout  */
void power_standby_force(void);      /* apagar pantalla inmediatamente     */
void power_standby_tick(void);       /* llamar cada ciclo de ROLE_NONE     */
bool power_standby_active(void);     /* true = pantalla apagada            */
