#ifndef __COMM_H__
#define __COMM_H__

#define W 500
#define H 800
#define N_P 100
#define N_V 10
#define N_SENSORS 16
#define NOISE_P 40
#define NOISE_V 90
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