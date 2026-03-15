#include "display.h"
#include "../../config/pins.h"
#include "../../config/system_config.h"

#include <Arduino.h>
#include <Wire.h>   /* para Wire.setClock() compartido con el compás */
#include <U8g2lib.h>

/* SSD1306 128x64 — I2C hardware, buffer completo (pines explícitos) */
static U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R2, OLED_RST, OLED_SCL, OLED_SDA);

/* ── Ciclo de vida ────────────────────────────────────────────────────── */

bool display_init(void)
{
    /* Encender VEXT (alimenta OLED y sensores) */
    pinMode(VEXT_ENABLE, OUTPUT);
    digitalWrite(VEXT_ENABLE, LOW);   /* LOW = ON */
    delay(100);

    /* U8g2 maneja Wire internamente (pines en el constructor) */
    u8g2.begin();
    u8g2.setContrast(255);

    /* Dejar Wire disponible para el compás (mismo bus) */
    Wire.setClock(I2C_CLOCK_HZ);

    return true;
}

void display_clear(void)  { u8g2.clearBuffer(); }
void display_update(void) { u8g2.sendBuffer();  }

/* ── Texto ────────────────────────────────────────────────────────────── */

void display_print_small(int x, int y, const char *text)
{
    u8g2.setFont(u8g2_font_5x7_tr);
    u8g2.drawStr(x, y, text);
}

void display_print_medium(int x, int y, const char *text)
{
    u8g2.setFont(u8g2_font_7x14_tf);
    u8g2.drawStr(x, y, text);
}

/* ── Primitivas gráficas ──────────────────────────────────────────────── */

void display_draw_circle(int x, int y, int r)
    { u8g2.drawCircle(x, y, r); }

void display_draw_line(int x1, int y1, int x2, int y2)
    { u8g2.drawLine(x1, y1, x2, y2); }

void display_draw_triangle(int x1, int y1, int x2, int y2, int x3, int y3)
    { u8g2.drawTriangle(x1, y1, x2, y2, x3, y3); }

void display_draw_disc(int x, int y, int r)
    { u8g2.drawDisc(x, y, r); }

void display_draw_pixel(int x, int y)
    { u8g2.drawPixel(x, y); }
