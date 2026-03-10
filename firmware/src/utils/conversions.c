#include "conversions.h"
#include <math.h>

#define EARTH_RADIUS_M  6371000.0

double deg_to_rad(double deg) { return deg * M_PI / 180.0; }
double rad_to_deg(double rad) { return rad * 180.0 / M_PI; }

double nmea_to_decimal(double nmea_coord, char direction)
{
    int    degrees = (int)(nmea_coord / 100);
    double minutes = nmea_coord - degrees * 100;
    double decimal = degrees + minutes / 60.0;
    if (direction == 'S' || direction == 'W') decimal = -decimal;
    return decimal;
}

double gps_distance_m(double lat1, double lon1, double lat2, double lon2)
{
    double dLat = deg_to_rad(lat2 - lat1);
    double dLon = deg_to_rad(lon2 - lon1);
    double a = sin(dLat / 2) * sin(dLat / 2) +
               cos(deg_to_rad(lat1)) * cos(deg_to_rad(lat2)) *
               sin(dLon / 2) * sin(dLon / 2);
    return EARTH_RADIUS_M * 2.0 * atan2(sqrt(a), sqrt(1.0 - a));
}

double gps_bearing_deg(double lat1, double lon1, double lat2, double lon2)
{
    double dLon  = deg_to_rad(lon2 - lon1);
    double rlat1 = deg_to_rad(lat1);
    double rlat2 = deg_to_rad(lat2);
    double y     = sin(dLon) * cos(rlat2);
    double x     = cos(rlat1) * sin(rlat2) - sin(rlat1) * cos(rlat2) * cos(dLon);
    double bearing = rad_to_deg(atan2(y, x));
    return fmod(bearing + 360.0, 360.0);
}
