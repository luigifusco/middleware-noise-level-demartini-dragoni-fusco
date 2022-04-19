#define P 100
#define V 10
#define W 500
#define H 800
#define N_SENSORS 16
#define N_P 40
#define N_V 80
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
