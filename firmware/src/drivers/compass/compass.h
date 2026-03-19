#ifndef COMPASS_H
#define COMPASS_H

#include <stdbool.h>

typedef struct {
    float heading;  /* grados magnéticos 0-360 (N=0, E=90) */
    int   x;        /* valor crudo magnetómetro X */
    int   y;
    int   z;
} compass_data_t;

bool compass_init(void);
bool compass_read(compass_data_t *out);

/* Calibración hard-iron — bloquea ~15 s, guarda en flash */
bool compass_calibrate(void);
/* true si ya existe calibración guardada en NVS */
bool compass_has_calibration(void);

/* Dibuja la UI de brújula en el display (requiere display_init previo) */
void compass_draw_ui(float heading_deg, int cx, int cy, int radius);

/* Flecha delgada (1 px) apuntando en 'angle_deg' desde el centro */
void compass_draw_arrow(int cx, int cy, float angle_deg, int length);

/* Flecha corta SOLO del centro al extremo (sin cola) — indicador de Norte */
void compass_draw_north_arrow(int cx, int cy, float angle_deg, int length);

/* Flecha gruesa (3 px) apuntando en 'angle_deg' */
void compass_draw_arrow_thick(int cx, int cy, float angle_deg, int length);

#endif /* COMPASS_H */
