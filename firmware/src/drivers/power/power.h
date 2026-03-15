#pragma once
#include <stdint.h>

/* ─────────────────────────────────────────────────────────────────────────
 * Gestión de energía — light sleep durante ROLE_NONE
 *
 * Fuentes de wakeup configuradas:
 *   • GPIO LOW  — BTN_UP, BTN_DOWN, BTN_SELECT (pull-up, presionado = LOW)
 *   • GPIO HIGH — LORA_DIO1  (SX1262 lo pone HIGH al recibir un paquete)
 *   • Timer     — max_ms ms para refrescar GPS + pantalla
 *
 * Consumo estimado en light sleep:
 *   ~2-5 mA CPU + ~1.5 mA SX1262 RX + ~0.5 mA OLED ≈ 5 mA total
 *   vs ~180 mA en plena operación
 * ───────────────────────────────────────────────────────────────────────── */

void     power_init(void);
void     power_idle_sleep(uint32_t max_ms); /* duerme hasta max_ms o evento */
uint32_t power_total_sleep_ms(void);        /* diagnóstico: tiempo total dormido */
