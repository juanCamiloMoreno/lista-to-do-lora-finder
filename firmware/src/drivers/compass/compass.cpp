#include "compass.h"
#include "../display/display.h"
#include "../../config/pins.h"
#include "../../config/system_config.h"

#include <Arduino.h>
#include <QMC5883LCompass.h>
#include <math.h>

static QMC5883LCompass _compass;

/* ── Inicialización ───────────────────────────────────────────────────── */

bool compass_init(void)
{
    /* Wire ya iniciado en display_init(); QMC5883L usa la misma instancia */
    _compass.init();
    return true;
}

/* ── Lectura ──────────────────────────────────────────────────────────── */

bool compass_read(compass_data_t *out)
{
    if (!out) return false;

    _compass.read();
    out->x = _compass.getX();
    out->y = _compass.getY();
    out->z = _compass.getZ();

    /* Heading usando atan2(Y, X) — normalizado a 0..360° */
    float h = atan2f((float)out->y, (float)out->x);
    if (h < 0)             h += 2.0f * (float)M_PI;
    if (h >= 2.0f * (float)M_PI) h -= 2.0f * (float)M_PI;
    out->heading = h * 180.0f / (float)M_PI;
    return true;
}

/* ── Helpers de dibujo ────────────────────────────────────────────────── */

/* Flecha delgada (1 px) — igual a dibujarFlecha() de los ejemplos */
void compass_draw_arrow(int cx, int cy, float angle_deg, int length)
{
    float rad = angle_deg * (float)M_PI / 180.0f;

    int x_tip  = cx + (int)roundf(length * cosf(rad));
    int y_tip  = cy + (int)roundf(length * sinf(rad));
    int x_tail = cx - (int)roundf(length * cosf(rad));
    int y_tail = cy - (int)roundf(length * sinf(rad));

    display_draw_line(x_tail, y_tail, x_tip, y_tip);

    /* Punta de flecha */
    int   tip  = 6;
    float aL   = (angle_deg + 150.0f) * (float)M_PI / 180.0f;
    float aR   = (angle_deg - 150.0f) * (float)M_PI / 180.0f;

    display_draw_line(x_tip, y_tip,
                      x_tip + (int)roundf(tip * cosf(aL)),
                      y_tip + (int)roundf(tip * sinf(aL)));
    display_draw_line(x_tip, y_tip,
                      x_tip + (int)roundf(tip * cosf(aR)),
                      y_tip + (int)roundf(tip * sinf(aR)));

    display_draw_pixel(cx, cy);
}

/* Flecha gruesa (3 px) — igual a dibujarFlechaGruesa() de los ejemplos */
void compass_draw_arrow_thick(int cx, int cy, float angle_deg, int length)
{
    float rad = angle_deg * (float)M_PI / 180.0f;

    int x_tip  = cx + (int)(length * cosf(rad));
    int y_tip  = cy + (int)(length * sinf(rad));
    int x_tail = cx - (int)(length * cosf(rad));
    int y_tail = cy - (int)(length * sinf(rad));

    /* Vector perpendicular para grosor de 3 px */
    float perpRad = rad + (float)M_PI / 2.0f;
    int dx = (int)roundf(cosf(perpRad));
    int dy = (int)roundf(sinf(perpRad));

    display_draw_line(x_tail,    y_tail,    x_tip,    y_tip);
    display_draw_line(x_tail+dx, y_tail+dy, x_tip+dx, y_tip+dy);
    display_draw_line(x_tail-dx, y_tail-dy, x_tip-dx, y_tip-dy);

    /* Triángulo punta */
    int   tp  = 7;
    float aL  = (angle_deg + 150.0f) * (float)M_PI / 180.0f;
    float aR  = (angle_deg - 150.0f) * (float)M_PI / 180.0f;
    int   x_L = x_tip + (int)(tp * cosf(aL));
    int   y_L = y_tip + (int)(tp * sinf(aL));
    int   x_R = x_tip + (int)(tp * cosf(aR));
    int   y_R = y_tip + (int)(tp * sinf(aR));

    display_draw_triangle(x_tip, y_tip, x_L, y_L, x_R, y_R);
    display_draw_disc(cx, cy, 2);
}

/* ── UI completa de brújula ───────────────────────────────────────────── */

void compass_draw_ui(float heading_deg, int cx, int cy, int radius)
{
    /* Doble círculo para dar sensación de grosor */
    display_draw_circle(cx, cy, radius);
    display_draw_circle(cx, cy, radius - 1);

    /* Marcas cada 30° */
    for (int a = 0; a < 360; a += 30) {
        float r = a * (float)M_PI / 180.0f;
        int x1 = cx + (int)((radius - 4) * cosf(r));
        int y1 = cy + (int)((radius - 4) * sinf(r));
        int x2 = cx + (int)((radius - 1) * cosf(r));
        int y2 = cy + (int)((radius - 1) * sinf(r));
        display_draw_line(x1, y1, x2, y2);
    }

    /* Flecha: norte fijo en pantalla → invertir heading */
    compass_draw_arrow(cx, cy, 360.0f - heading_deg, radius - 6);
}
