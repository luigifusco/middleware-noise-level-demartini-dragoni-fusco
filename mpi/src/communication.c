// #include<mqtt.h>
#include<string.h>
#include<stdlib.h>
#include<stdio.h>
#include<time.h>

#include "sim.h"

typedef struct sensor_msg_t {
    float lat;
    float lon;
    float noise;
    float reliability;
    long ts;
} sensor_msg_t;

char* format_msg(const sensor_msg_t *msg) {
    const char* FMT = "{\"noise\":%f,\"ts\":%ld,\"reliability\":%f,\"lat\":%f,\"lon\":%f}";
    const size_t S = 128;
    char* buf = (char*)malloc(S);
    size_t n = snprintf(buf, S, FMT, msg->noise, msg->ts, msg->reliability, msg->lat, msg->lon);

    if (n > S - 1) {
        size_t size = n + 1;
        buf = realloc(buf, size);
        size_t n = snprintf(buf, size, FMT, msg->noise, msg->ts, msg->reliability, msg->lat, msg->lon);

        if (n < 0 || n > size - 1) {
            fprintf(stderr, "ERROR: error writing msg to JSON string");
        }
    }

    return buf;
}

void send_readings(const sensor_t* sensors, size_t n) {
    for (int i = 0; i < n; i++) {
        const sensor_t* s = &sensors[i];
        sensor_msg_t msg = {s->x, s->y, lin_to_dbm(s->noise_mw), 0.5, time(NULL)};
        char* json = format_msg(&msg);
        // mqtt_publish(client, "sensor/data", json, strlen(json), MQTT_PUBLISH_QOS_1);
        fprintf(stdout, "%03d: ", i);
        fputs(json, stdout);
        putc('\n', stdout);
        free(json);
    }
}
