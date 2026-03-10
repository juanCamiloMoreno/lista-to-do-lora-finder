#ifndef GPS_H
#define GPS_H

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

#endif /* GPS_H */
