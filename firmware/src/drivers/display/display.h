#ifndef DISPLAY_H
#define DISPLAY_H

#include <stdbool.h>
#include <stdint.h>

/* ── Ciclo de vida ───────────────────────────────────────────────────── */
bool display_init(void);
void display_clear(void);
void display_update(void);   /* flush buffer → pantalla */
void display_off(void);      /* apaga el panel (modo reposo) */
void display_on(void);       /* enciende el panel */

/* ── Texto ───────────────────────────────────────────────────────────── */
void display_print_small (int x, int y, const char *text); /* font 5x7  */
void display_print_medium(int x, int y, const char *text); /* font 7x14 */

/* Alias legacy */
#define display_print(col, row, text) display_print_small(col, row, text)

/* ── Primitivas gráficas ─────────────────────────────────────────────── */
void display_draw_circle  (int x,  int y,  int r);
void display_draw_frame   (int x,  int y,  int w,  int h); /* rectángulo vacío  */
void display_draw_box      (int x,  int y,  int w,  int h); /* rectángulo lleno  */
void display_set_draw_color(uint8_t c);  /* 0=negro, 1=blanco (normal)        */
void display_set_font_mode (uint8_t m);  /* 0=opaco, 1=transparente           */
void display_draw_line    (int x1, int y1, int x2, int y2);
void display_draw_triangle(int x1, int y1, int x2, int y2, int x3, int y3);
void display_draw_disc    (int x,  int y,  int r);
void display_draw_pixel   (int x,  int y);

#endif /* DISPLAY_H */
