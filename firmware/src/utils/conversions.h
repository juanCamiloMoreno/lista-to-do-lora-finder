#ifndef CONVERSIONS_H
#define CONVERSIONS_H

double deg_to_rad(double deg);
double rad_to_deg(double rad);

/* GPS coordinate conversions */
double nmea_to_decimal(double nmea_coord, char direction);

/* Distance between two GPS points (metres, Haversine) */
double gps_distance_m(double lat1, double lon1, double lat2, double lon2);

/* Bearing from point 1 to point 2 (degrees) */
double gps_bearing_deg(double lat1, double lon1, double lat2, double lon2);

#endif /* CONVERSIONS_H */
