#include "gps.h"
#include "../../config/system_config.h"
#include "../../config/pins.h"

static gps_data_t _gps_data = {0};

bool gps_init(void)
{
    /* TODO: initialize serial port for GPS module */
    return false;
}

bool gps_update(void)
{
    /* TODO: read NMEA sentences and parse position */
    return false;
}

gps_data_t gps_get_data(void)
{
    return _gps_data;
}
