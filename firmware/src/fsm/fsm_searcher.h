#pragma once
#include <stdbool.h>

/* ─────────────────────────────────────────────────────────────────────────
 * FSM del rol BUSCADOR (Searcher / Node A).
 * Se activa cuando el usuario presiona [SELECT] en pantalla de inicio.
 * ───────────────────────────────────────────────────────────────────────── */

typedef enum {
    SEARCHER_SEARCHING,    /* Enviando SEARCH_START, esperando ACK          */
    SEARCHER_NAV_GPS,      /* Navegando con GPS (distancia + rumbo reales)  */
    SEARCHER_NAV_RSSI,     /* Navegando por RSSI (sin GPS)                  */
    SEARCHER_SIGNAL_LOST,  /* Sin paquetes durante SIGNAL_LOST_MS           */
    SEARCHER_REUNITED,     /* Reunión detectada                             */
    SEARCHER_SOS,          /* Recibió SOS del objetivo                      */
    SEARCHER_DONE,         /* Sesión terminada — volver a ROLE_NONE         */
} searcher_state_t;

void             fsm_searcher_init(void);
void             fsm_searcher_update(void);
searcher_state_t fsm_searcher_get_state(void);
bool             fsm_searcher_is_done(void);
