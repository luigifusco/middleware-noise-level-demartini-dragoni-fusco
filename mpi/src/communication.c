#include<mqtt.h>
#include<string.h>
#include<stdlib.h>
#include<stdio.h>
#include<communication.h>

char* format_msg(sensor_msg_t *msg) {
    const FMT = "{\"noise\":%f,\"ts\":%ld,\"reliability\":%f,\"lat\":%f,\"lon\":%f}";
    const S = 128;
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