#ifndef DISPLAY_H
#define DISPLAY_H

#include <stdbool.h>
#include <stdint.h>

/* ── Ciclo de vida ───────────────────────────────────────────────────── */
bool display_init(void);
void display_clear(void);
void display_update(void);   /* flush buffer → pantalla */

/* ── Texto ───────────────────────────────────────────────────────────── */
void display_print_small (int x, int y, const char *text); /* font 5x7  */
void display_print_medium(int x, int y, const char *text); /* font 7x14 */

/* Alias legacy */
#define display_print(col, row, text) display_print_small(col, row, text)

/* ── Primitivas gráficas ─────────────────────────────────────────────── */
void display_draw_circle  (int x,  int y,  int r);
void display_draw_line    (int x1, int y1, int x2, int y2);
void display_draw_triangle(int x1, int y1, int x2, int y2, int x3, int y3);
void display_draw_disc    (int x,  int y,  int r);
void display_draw_pixel   (int x,  int y);

#endif /* DISPLAY_H */
