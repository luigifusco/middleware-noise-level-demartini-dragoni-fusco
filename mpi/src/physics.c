#include <math.h>

float distance(float x0, float y0, float x1, float y1) {
    float dx = x1 - x0;
    float dy = y1 - y0;
    return sqrtf(dx * dx + dy * dy);
}

double noise_decay(double power, float distance) {
    return power / (double)(distance * distance);
}

double dbm_to_lin(float dbm) {
    return 0.001 * pow(10.0, dbm / 10.0);
}

float lin_to_dbm(double lin) {
    return 10.0 * log10(lin / 0.001);
}