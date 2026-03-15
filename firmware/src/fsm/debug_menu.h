#pragma once
#include <stdbool.h>

/* ─────────────────────────────────────────────────────────────────────────
 * Menú de diagnóstico de hardware — accesible desde el Menú de Pruebas.
 *
 * Sub-tests:
 *   1. Botones  — muestra cada pulsación en tiempo real
 *   2. Brújula  — heading + valores crudos X/Y/Z
 *   3. LED RGB  — cicla colores con UP/DOWN
 *   4. GPS      — coordenadas, satélites, altitud
 *   5. LoRa     — envía paquete con UP, muestra RSSI/SNR recibidos
 * ───────────────────────────────────────────────────────────────────────── */

void debug_menu_init(void);
void debug_menu_update(void);
bool debug_menu_is_done(void);
