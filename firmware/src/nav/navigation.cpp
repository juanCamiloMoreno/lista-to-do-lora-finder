#include "navigation.h"
#include <math.h>

#define DEG2RAD  0.01745329251994f
#define EARTH_R  6371000.0f

float nav_distance_m(float lat1, float lon1, float lat2, float lon2)
{
    float dlat = (lat2 - lat1) * DEG2RAD;
    float dlon = (lon2 - lon1) * DEG2RAD;
    float a = sinf(dlat / 2.0f) * sinf(dlat / 2.0f) +
              cosf(lat1 * DEG2RAD) * cosf(lat2 * DEG2RAD) *
              sinf(dlon / 2.0f) * sinf(dlon / 2.0f);
    return 2.0f * EARTH_R * asinf(sqrtf(a));
}

float nav_bearing_deg(float lat1, float lon1, float lat2, float lon2)
{
    float rlat1 = lat1 * DEG2RAD;
    float rlat2 = lat2 * DEG2RAD;
    float dlon  = (lon2 - lon1) * DEG2RAD;
    float y = sinf(dlon) * cosf(rlat2);
    float x = cosf(rlat1) * sinf(rlat2) -
               sinf(rlat1) * cosf(rlat2) * cosf(dlon);
    float b = atan2f(y, x) / DEG2RAD;
    if (b < 0.0f) b += 360.0f;
    return b;
}

const char *nav_cardinal(float deg)
{
    static const char *names[] = { "N","NE","E","SE","S","SW","W","NW" };
    int idx = (int)((deg + 22.5f) / 45.0f) % 8;
    return names[idx];
}
