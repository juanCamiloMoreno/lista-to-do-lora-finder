#ifndef GPS_H
#define GPS_H

#include <stdint.h>
#include <stdbool.h>

typedef struct {
    double latitude;
    double longitude;
    double altitude;
    float  speed;       /* km/h */
    uint8_t satellites;
    bool   valid;
} gps_data_t;

bool gps_init(void);
bool gps_update(void);
gps_data_t gps_get_data(void);

/* Control del pin enable del NEO-M8N (GPS_EN_PIN en pins.h).
 * gps_enable()  → HIGH: módulo encendido (comportamiento por defecto al boot).
 * gps_disable() → LOW:  módulo apagado para ahorrar energía.
 * El pin se configura en gps_init(); si el hardware no lo usa es inocuo. */
void gps_enable(void);
void gps_disable(void);
bool gps_is_enabled(void);

#endif /* GPS_H */
