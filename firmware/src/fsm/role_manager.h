#pragma once
#include <stdbool.h>

/* ─────────────────────────────────────────────────────────────────────────
 * Gestor de roles en tiempo de ejecución.
 *
 * Ambos dispositivos corren el MISMO binario.  El rol se asigna así:
 *   • [SELECT] presionado → este dispositivo es BUSCADOR, envía SEARCH_START
 *   • Se recibe MSG_SEARCH_START → este dispositivo es OBJETIVO, envía ACK
 *
 * Al terminar la sesión (REUNITED → DONE), el rol vuelve a ROLE_NONE y el
 * dispositivo puede asumir cualquier rol en la próxima sesión.
 * ───────────────────────────────────────────────────────────────────────── */

typedef enum {
    ROLE_NONE      = 0,  /* Pantalla de inicio — esperando activación */
    ROLE_SEARCHER  = 1,  /* Ejecutando FSM del Buscador               */
    ROLE_TARGET    = 2,  /* Ejecutando FSM del Objetivo               */
    ROLE_TEST_MENU = 3,  /* Menú de pruebas de campo                  */
} node_role_t;

void        role_manager_init(void);
void        role_manager_update(void);
node_role_t role_manager_get_role(void);
