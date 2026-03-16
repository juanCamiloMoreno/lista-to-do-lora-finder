#include "compass.h"
#include "../display/display.h"
#include "../../config/pins.h"
#include "../../config/system_config.h"

#include <Arduino.h>
#include <Wire.h>
#include <Preferences.h>
#include <math.h>

/* QMC5883L / HMC5883L en Wire1 (bus I2C separado del OLED) */
static uint8_t _addr  = 0;
static bool    _isQMC = false;

/* Offsets de calibración hard-iron */
static float _offset_x = 0.0f;
static float _offset_y = 0.0f;

/* ── Helper: lectura cruda sin offset ────────────────────────────────── */

static bool _raw_read(int16_t *x, int16_t *y, int16_t *z)
{
    if (_addr == 0) return false;

    Wire1.beginTransmission(_addr);
    Wire1.write(_isQMC ? 0x00 : 0x03);
    if (Wire1.endTransmission(false) != 0) return false;

    Wire1.requestFrom((int)_addr, 6);
    if (Wire1.available() < 6) return false;

    if (_isQMC) {
        *x = Wire1.read() | (Wire1.read() << 8);
        *y = Wire1.read() | (Wire1.read() << 8);
        *z = Wire1.read() | (Wire1.read() << 8);
    } else {
        *x = (Wire1.read() << 8) | Wire1.read();
        *z = (Wire1.read() << 8) | Wire1.read();
        *y = (Wire1.read() << 8) | Wire1.read();
    }
    return true;
}

/* ── Inicialización ───────────────────────────────────────────────────── */

bool compass_init(void)
{
    Wire1.begin(MAG_SDA, MAG_SCL);
    Wire1.setClock(400000);

    /* Detectar QMC5883L (0x0D) o HMC5883L (0x1E) */
    for (uint8_t addr : {0x0D, 0x1E}) {
        Wire1.beginTransmission(addr);
        if (Wire1.endTransmission() == 0) {
            _addr = addr;
            break;
        }
    }

    if (_addr == 0x0D) {
        _isQMC = true;
        Wire1.beginTransmission(_addr);
        Wire1.write(0x0B); Wire1.write(0x01);
        if (Wire1.endTransmission() != 0) return false;
        delay(10);
        uint8_t ctrl = (0b11 << 6) | (0b01 << 4) | (0b10 << 2) | 0b01;
        Wire1.beginTransmission(_addr);
        Wire1.write(0x09); Wire1.write(ctrl);
        if (Wire1.endTransmission() != 0) return false;

    } else if (_addr == 0x1E) {
        _isQMC = false;
        Wire1.beginTransmission(_addr);
        Wire1.write(0x00); Wire1.write(0x70);
        if (Wire1.endTransmission() != 0) return false;
        Wire1.beginTransmission(_addr);
        Wire1.write(0x01); Wire1.write(0xA0);
        if (Wire1.endTransmission() != 0) return false;
        Wire1.beginTransmission(_addr);
        Wire1.write(0x02); Wire1.write(0x00);
        if (Wire1.endTransmission() != 0) return false;

    } else {
        return false;
    }

    /* Cargar offsets guardados en NVS */
    Preferences prefs;
    prefs.begin("compass", true);
    if (prefs.isKey("ox")) {
        _offset_x = prefs.getFloat("ox", 0.0f);
        _offset_y = prefs.getFloat("oy", 0.0f);
    }
    prefs.end();

    return true;
}

/* ── Calibración ──────────────────────────────────────────────────────── */

bool compass_has_calibration(void)
{
    Preferences prefs;
    prefs.begin("compass", true);
    bool has = prefs.isKey("ox");
    prefs.end();
    return has;
}

bool compass_calibrate(void)
{
    if (_addr == 0) return false;

    float min_x =  1e9f, max_x = -1e9f;
    float min_y =  1e9f, max_y = -1e9f;

    uint32_t start    = millis();
    uint32_t duration = COMPASS_CALIB_MS;
    uint32_t last_ui  = 0;

    while (millis() - start < duration) {
        int16_t rx, ry, rz;
        if (_raw_read(&rx, &ry, &rz)) {
            if (rx < min_x) min_x = rx;
            if (rx > max_x) max_x = rx;
            if (ry < min_y) min_y = ry;
            if (ry > max_y) max_y = ry;
        }

        /* Actualizar pantalla ~5 veces por segundo */
        if (millis() - last_ui >= 200) {
            last_ui = millis();
            uint32_t elapsed = millis() - start;
            int secs_left    = (int)((duration - elapsed) / 1000) + 1;
            int bar_w        = 100;
            int bar_fill     = (int)(bar_w * elapsed / duration);

            display_clear();
            display_print_medium(0, 14, "CALIBRANDO");
            display_print_small (0, 28, "Gira en forma de 8");
            display_print_small (0, 38, "lentamente...");

            /* Barra de progreso */
            display_draw_line(0, 50, bar_w, 50);          /* borde sup */
            display_draw_line(0, 56, bar_w, 56);          /* borde inf */
            display_draw_line(0, 50, 0,     56);          /* borde izq */
            display_draw_line(bar_w, 50, bar_w, 56);      /* borde der */
            for (int i = 1; i < bar_fill; i++)
                display_draw_line(i, 51, i, 55);          /* relleno   */

            /* Segundos restantes */
            char buf[8];
            snprintf(buf, sizeof(buf), "%2ds", secs_left);
            display_print_small(106, 57, buf);

            display_update();
        }

        delay(10);
    }

    /* Calcular y guardar offsets */
    _offset_x = (max_x + min_x) / 2.0f;
    _offset_y = (max_y + min_y) / 2.0f;

    Preferences prefs;
    prefs.begin("compass", false);
    prefs.putFloat("ox", _offset_x);
    prefs.putFloat("oy", _offset_y);
    prefs.end();

    /* Confirmar en pantalla */
    display_clear();
    display_print_medium(0, 20, "Calibrado!");
    display_print_small (0, 35, "Guardado en flash");
    display_update();
    delay(2000);

    return true;
}

/* ── Lectura ──────────────────────────────────────────────────────────── */

bool compass_read(compass_data_t *out)
{
    if (!out) return false;

    int16_t rx, ry, rz;
    if (!_raw_read(&rx, &ry, &rz)) return false;

    out->x = rx;
    out->y = ry;
    out->z = rz;

    /* Aplicar offset de calibración */
    float cx = (float)rx - _offset_x;
    float cy = (float)ry - _offset_y;

    /* Filtro EMA sobre sin/cos (maneja el salto 0°/360°) */
    static float _s      = 0.0f;
    static float _c      = 0.0f;   /* 0,0 = no inicializado */
    static bool  _inited = false;
    float angle = atan2f(cy, cx);

    if (!_inited) {
        /* Primera lectura: cargar directamente sin filtrar ni rechazar */
        _s      = sinf(angle);
        _c      = cosf(angle);
        _inited = true;
    } else {
        /* Outlier rejection: descartar si salta >COMPASS_OUTLIER_DEG */
        float cur_h = atan2f(_s, _c);
        float diff  = angle - cur_h;
        while (diff >  (float)M_PI) diff -= 2.0f * (float)M_PI;
        while (diff < -(float)M_PI) diff += 2.0f * (float)M_PI;
        if (fabsf(diff) < COMPASS_OUTLIER_DEG * (float)M_PI / 180.0f) {
            _s = _s * (1.0f - COMPASS_FILTER_ALPHA) + sinf(angle) * COMPASS_FILTER_ALPHA;
            _c = _c * (1.0f - COMPASS_FILTER_ALPHA) + cosf(angle) * COMPASS_FILTER_ALPHA;
        }
    }

    float h = atan2f(_s, _c);
    if (h < 0) h += 2.0f * (float)M_PI;
    out->heading = h * 180.0f / (float)M_PI;
    return true;
}

/* ── Helpers de dibujo ────────────────────────────────────────────────── */

void compass_draw_arrow(int cx, int cy, float angle_deg, int length)
{
    float rad = angle_deg * (float)M_PI / 180.0f;

    int x_tip  = cx + (int)roundf(length * cosf(rad));
    int y_tip  = cy + (int)roundf(length * sinf(rad));
    int x_tail = cx - (int)roundf(length * cosf(rad));
    int y_tail = cy - (int)roundf(length * sinf(rad));

    display_draw_line(x_tail, y_tail, x_tip, y_tip);

    int   tip = 6;
    float aL  = (angle_deg + 150.0f) * (float)M_PI / 180.0f;
    float aR  = (angle_deg - 150.0f) * (float)M_PI / 180.0f;

    display_draw_line(x_tip, y_tip,
                      x_tip + (int)roundf(tip * cosf(aL)),
                      y_tip + (int)roundf(tip * sinf(aL)));
    display_draw_line(x_tip, y_tip,
                      x_tip + (int)roundf(tip * cosf(aR)),
                      y_tip + (int)roundf(tip * sinf(aR)));

    display_draw_pixel(cx, cy);
}

void compass_draw_arrow_thick(int cx, int cy, float angle_deg, int length)
{
    float rad = angle_deg * (float)M_PI / 180.0f;

    int x_tip  = cx + (int)(length * cosf(rad));
    int y_tip  = cy + (int)(length * sinf(rad));
    int x_tail = cx - (int)(length * cosf(rad));
    int y_tail = cy - (int)(length * sinf(rad));

    float perpRad = rad + (float)M_PI / 2.0f;
    int dx = (int)roundf(cosf(perpRad));
    int dy = (int)roundf(sinf(perpRad));

    display_draw_line(x_tail,    y_tail,    x_tip,    y_tip);
    display_draw_line(x_tail+dx, y_tail+dy, x_tip+dx, y_tip+dy);
    display_draw_line(x_tail-dx, y_tail-dy, x_tip-dx, y_tip-dy);

    int   tp = 7;
    float aL = (angle_deg + 150.0f) * (float)M_PI / 180.0f;
    float aR = (angle_deg - 150.0f) * (float)M_PI / 180.0f;

    display_draw_triangle(x_tip, y_tip,
                          x_tip + (int)(tp * cosf(aL)),
                          y_tip + (int)(tp * sinf(aL)),
                          x_tip + (int)(tp * cosf(aR)),
                          y_tip + (int)(tp * sinf(aR)));
    display_draw_disc(cx, cy, 2);
}

void compass_draw_north_arrow(int cx, int cy, float angle_deg, int length)
{
    float rad  = angle_deg * (float)M_PI / 180.0f;
    int x_tip  = cx + (int)roundf(length * cosf(rad));
    int y_tip  = cy + (int)roundf(length * sinf(rad));

    display_draw_line(cx, cy, x_tip, y_tip);

    int   tip = 4;
    float aL  = (angle_deg + 150.0f) * (float)M_PI / 180.0f;
    float aR  = (angle_deg - 150.0f) * (float)M_PI / 180.0f;

    display_draw_line(x_tip, y_tip,
                      x_tip + (int)roundf(tip * cosf(aL)),
                      y_tip + (int)roundf(tip * sinf(aL)));
    display_draw_line(x_tip, y_tip,
                      x_tip + (int)roundf(tip * cosf(aR)),
                      y_tip + (int)roundf(tip * sinf(aR)));
}

void compass_draw_ui(float heading_deg, int cx, int cy, int radius)
{
    display_draw_circle(cx, cy, radius);
    display_draw_circle(cx, cy, radius - 1);

    for (int a = 0; a < 360; a += 30) {
        float r = a * (float)M_PI / 180.0f;
        int x1 = cx + (int)((radius - 4) * cosf(r));
        int y1 = cy + (int)((radius - 4) * sinf(r));
        int x2 = cx + (int)((radius - 1) * cosf(r));
        int y2 = cy + (int)((radius - 1) * sinf(r));
        display_draw_line(x1, y1, x2, y2);
    }

    compass_draw_arrow(cx, cy, heading_deg + 180.0f, radius - 6);
}
