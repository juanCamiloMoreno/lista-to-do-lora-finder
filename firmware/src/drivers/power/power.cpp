#include "power.h"
#include "config/pins.h"

#include <esp_sleep.h>
#include <driver/gpio.h>
#include <Arduino.h>

static uint32_t _total_sleep_ms = 0;

/* ── Inicialización — llamar una vez en app_init() ───────────────────── */

void power_init(void)
{
    /* Botones: INPUT_PULLUP → normalmente HIGH, presionado = LOW */
    gpio_wakeup_enable((gpio_num_t)BTN_UP_PIN,     GPIO_INTR_LOW_LEVEL);
    gpio_wakeup_enable((gpio_num_t)BTN_DOWN_PIN,   GPIO_INTR_LOW_LEVEL);
    gpio_wakeup_enable((gpio_num_t)BTN_SELECT_PIN, GPIO_INTR_LOW_LEVEL);

    /* LoRa DIO1: SX1262 lo pone HIGH cuando llega un paquete */
    gpio_wakeup_enable((gpio_num_t)LORA_DIO1, GPIO_INTR_HIGH_LEVEL);

    esp_sleep_enable_gpio_wakeup();

    Serial.println("[power] light-sleep wakeup configurado (btn + DIO1 + timer)");
}

/* ── Light sleep con timeout ─────────────────────────────────────────── */

void power_idle_sleep(uint32_t max_ms)
{
    /* Timer: wakeup máximo para refrescar pantalla/GPS */
    esp_sleep_enable_timer_wakeup((uint64_t)max_ms * 1000ULL);

    uint32_t t0 = millis();
    esp_light_sleep_start();
    _total_sleep_ms += millis() - t0;

    /* Deshabilitar timer para que no interfiera en el próximo wakeup */
    esp_sleep_disable_wakeup_source(ESP_SLEEP_WAKEUP_TIMER);
}

uint32_t power_total_sleep_ms(void) { return _total_sleep_ms; }
