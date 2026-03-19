#pragma once
#include <stdbool.h>

/* ─────────────────────────────────────────────────────────────────────────
 * Menú de pruebas de campo — métricas de tesis (UC eval):
 *   1. Alcance/RSSI    — RSSI+SNR en tiempo real (ambos dispositivos aquí)
 *   2. PDR paquetes    — envía 20 pkts, mide tasa de entrega
 *   3. T. Respuesta    — mide RTT ping-pong (round-trip time)
 *   4. Precisión GPS   — calidad de fix GPS
 *   5. Config SF       — cambia Spreading Factor 7-12 en caliente
 *
 * Acceso: mantener presionado BTN_UP en la pantalla de inicio.
 * Salida: cursor sobre "<< Volver" + [OK], o [OK] en el test activo.
 * ───────────────────────────────────────────────────────────────────────── */

void test_menu_init(void);
void test_menu_update(void);
bool test_menu_is_done(void);   /* true = usuario eligió "Volver" */
