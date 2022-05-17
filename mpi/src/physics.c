#include <math.h>
#include "physics.h"
#include "utils.h"

float distance(float x0, float y0, float x1, float y1) {
    float dx = x1 - x0;
    float dy = y1 - y0;
    return sqrtf(dx * dx + dy * dy);
}

float distance_ang(float lat0, float lon0, float lat1, float lon1) {
    const float l1 = lat0 * PI / 180; // phi, lambda in radians
    const float l2 = lat1 * PI / 180;
    const float dlat = (lat1-lat0) * PI / 180;
    const float dlon = (lon1-lon0) * PI / 180;

    const float a = sinf(dlat / 2) * sinf(dlat / 2) + cosf(l1) * cosf(l2) * sinf(dlon / 2) * sinf(dlon / 2);
    const float c = 2 * atan2(sqrtf(a), sqrtf(1-a));

    return EARTH_RADIUS * c; // in metres
}

double noise_decay(double power, float distance) {
    if (distance < 1.0) {
        distance = 1.0;
    }
    return power / (double)(distance * distance);
}

double dbm_to_lin(float dbm) {
    return 0.001 * pow(10.0, dbm / 10.0);
}

float lin_to_dbm(double lin) {
    return 10.0 * log10(lin / 0.001);
}