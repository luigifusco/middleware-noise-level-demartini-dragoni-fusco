#ifndef __COMM_H__
#define __COMM_H__

#define LAT_0 45.0
#define LON_0 9.0
#define LAT_1 45.01
#define LON_1 9.01

#define N_P 15000
#define N_V 250
#define N_SENSORS 16
#define NOISE_P 40.0
#define NOISE_V 90.0
#define D_P 10
#define D_V 50
#define V_P 3
#define V_V 20
#define TS 5

typedef enum entity_kind {
    PERSON,
    VEHICLE
} entity_kind;

typedef struct entity_t {
    entity_kind kind;
    float x;
    float y;
    float x_v;
    float y_v;
} entity_t;

typedef struct sensor_t {
    char* id;
    float x;
    float y;
    double noise_mw;
} sensor_t;

typedef struct bounds_t {
    float x0;
    float x1;
    float y0;
    float y1;
} bounds_t;

#endif