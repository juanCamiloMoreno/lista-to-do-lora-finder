#include "compass.h"
#include "../../config/system_config.h"
#include "../../config/pins.h"

bool compass_init(void)
{
    /* TODO: initialize HMC5883L / QMC5883L over I2C */
    return false;
}

bool compass_read(compass_data_t *out)
{
    /* TODO: read magnetometer registers and compute heading */
    if (!out) return false;
    return false;
}
