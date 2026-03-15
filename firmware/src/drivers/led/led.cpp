#include "led.h"
#include "config/pins.h"
#include <Adafruit_NeoPixel.h>

static Adafruit_NeoPixel _led(1, RGB_LED_PIN, NEO_GRB + NEO_KHZ800);

void led_init(void)
{
    _led.begin();
    _led.setBrightness(60);   /* 0-255; 60 es visible sin deslumbrar */
    _led.clear();
    _led.show();
}

static void _flash(uint8_t r, uint8_t g, uint8_t b)
{
    _led.setPixelColor(0, _led.Color(r, g, b));
    _led.show();
    delay(80);
    _led.clear();
    _led.show();
}

void led_flash_tx(void) { _flash(0,   0, 200); }   /* Azul  */
void led_flash_rx(void) { _flash(0, 200,   0); }   /* Verde */
