#ifndef __COMM_H__
#define __COMM_H__

#include <librdkafka/rdkafka.h>
#include "sim.h"

typedef struct reading_msg_t {
    char* id;
    float noise;
    long ts;
} reading_msg_t;

char* format_msg_json(const reading_msg_t *msg);

rd_kafka_t* kafka_build_producer(char* broker);

int kafka_send_reading(rd_kafka_t* producer, reading_msg_t* reading, char* topic);

int kafka_close(rd_kafka_t* producer);

#endif