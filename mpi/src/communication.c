#include<string.h>
#include<stdlib.h>
#include<stdio.h>
#include<time.h>

#include <librdkafka/rdkafka.h>

#include "sim.h"
#include "communication.h"

char* format_msg(const reading_msg_t *msg) {
    const char* FMT = "{\"id\"=\"%s\",\"noise\"=%f,\"ts\"=%ld}";
    const size_t S = 64;
    char* buf = (char*)malloc(S);
    size_t n = snprintf(buf, S, FMT, msg->id, msg->noise, msg->ts);

    if (n > S - 1) {
        size_t size = n + 1;
        buf = realloc(buf, size);
        size_t n = snprintf(buf, size, FMT, msg->id, msg->noise, msg->ts);

        if (n < 0 || n > size - 1) {
            fprintf(stderr, "ERROR: error writing msg to JSON string\n");
        }
    }

    return buf;
}

/* Optional per-message delivery callback (triggered by poll() or flush())
 * when a message has been successfully delivered or permanently
 * failed delivery (after retries).
 */
static void dr_msg_cb (rd_kafka_t *kafka_handle,
                       const rd_kafka_message_t *rkmessage,
                       void *opaque) {
    if (rkmessage->err) {
        fprintf(stderr, "Message delivery failed: %s\n", rd_kafka_err2str(rkmessage->err));
    }
}

rd_kafka_t* kafka_build_producer(char* broker) {
    char errstr[512];

    // Prepare configuration
    rd_kafka_conf_t *conf = rd_kafka_conf_new();

    rd_kafka_conf_res_t result = rd_kafka_conf_set(
        conf,
        "bootstrap.servers",
        broker,
        errstr,
        sizeof(errstr)
    );

    if (result != RD_KAFKA_CONF_OK) {
        fprintf(stderr, "%s\n", errstr);
        exit(1);
    }

    // Install a delivery-error callback.
    rd_kafka_conf_set_dr_msg_cb(conf, dr_msg_cb);

    // Create the Producer instance.
    rd_kafka_t *producer = rd_kafka_new(RD_KAFKA_PRODUCER, conf, errstr, sizeof(errstr));
    if (!producer) {
        fprintf(stderr, "Failed to create new producer: %s\n", errstr);
        exit(101);
    }
    // Configuration object is now owned, and freed, by the rd_kafka_t instance.
    conf = NULL;

    return producer;
}

int kafka_send_reading(rd_kafka_t* producer, reading_msg_t* reading) {
    rd_kafka_resp_err_t err;

    const char* topic = "poi-data"; // TODO: read from config

    char* key = strdup(reading->id);
    const size_t key_len = strlen(key);

    const char* value = format_msg(reading);
    const size_t value_len = strlen(value);

    err = rd_kafka_producev(producer,
                            RD_KAFKA_V_TOPIC(topic),
                            RD_KAFKA_V_MSGFLAGS(RD_KAFKA_MSG_F_COPY),
                            RD_KAFKA_V_KEY((void*)key, key_len),
                            RD_KAFKA_V_VALUE((void*)value, value_len),
                            RD_KAFKA_V_OPAQUE(NULL),
                            RD_KAFKA_V_END);

    if (err) {
        fprintf(stderr, "Failed to produce to topic %s: %s\n", topic, rd_kafka_err2str(err));
        return 0;
    } else {
        fprintf(stdout, "Produced event to topic %s: key = %12s value = %12s\n", topic, key, value);
    }

    rd_kafka_poll(producer, 0);

    return 1;
}

int kafka_close(rd_kafka_t* producer) {
    fprintf(stdout, "flushing final messages..");
    rd_kafka_flush(producer, 10 * 1000);

    if (rd_kafka_outq_len(producer) > 0) {
        fprintf(stderr, "%d message(s) were not delivered", rd_kafka_outq_len(producer));
    }

    rd_kafka_destroy(producer);
    producer = NULL;

    return 1;
}
