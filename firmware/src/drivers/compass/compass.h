#ifndef COMPASS_H
#define COMPASS_H

#include <stdbool.h>

typedef struct {
    float heading;  /* degrees 0-360 */
    float x;
    float y;
    float z;
} compass_data_t;

bool compass_init(void);
bool compass_read(compass_data_t *out);

#endif /* COMPASS_H */
