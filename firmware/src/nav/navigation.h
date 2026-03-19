#pragma once

/* ─────────────────────────────────────────────────────────────────────────
 * Cálculo de distancia y rumbo entre dos coordenadas GPS.
 * ───────────────────────────────────────────────────────────────────────── */

/* Distancia en metros entre dos puntos GPS (Haversine) */
float nav_distance_m(float lat1, float lon1, float lat2, float lon2);

/* Rumbo desde punto 1 hacia punto 2 en grados (0 = Norte, sentido horario) */
float nav_bearing_deg(float lat1, float lon1, float lat2, float lon2);

/* Nombre cardinal abreviado de un rumbo (N, NE, E, SE, S, SW, W, NW) */
const char *nav_cardinal(float bearing_deg);
