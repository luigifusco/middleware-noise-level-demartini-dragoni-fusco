#ifndef __PHYSICS_H__
#define __PHYSICS_H__

#define EARTH_RADIUS 6371000.0

float distance(float x0, float y0, float x1, float y1);

float distance_ang(float lat0, float lon0, float lat1, float lon1);

double noise_decay(double power, float distance);

double dbm_to_lin(float dbm);

float lin_to_dbm(double lin);

#endif