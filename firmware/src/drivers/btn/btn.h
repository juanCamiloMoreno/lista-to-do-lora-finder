#pragma once
#include <stdint.h>
#include <stdbool.h>

typedef enum {
    BTN_UP     = 0,
    BTN_DOWN   = 1,
    BTN_SELECT = 2,
    BTN_COUNT  = 3,
} btn_id_t;

void btn_init(void);

/* Fallback por polling — detecta flancos si el ISR no disparó.
 * Llamar una vez por ciclo (ej. al inicio de role_manager_update). */
void btn_update(void);

/* Devuelve true UNA sola vez por pulsación (flanco de bajada con debounce) */
bool btn_pressed(btn_id_t btn);

/* Devuelve true mientras el botón está sostenido (>500 ms) */
bool btn_held(btn_id_t btn);

/* Lectura directa del pin (LOW = presionado) — solo diagnóstico */
bool btn_raw_low(btn_id_t btn);
