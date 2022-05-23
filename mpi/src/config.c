#include<stdio.h>
#include<stdlib.h>
#include<errno.h>
#include<string.h>

#include "toml/toml.h"
#include "config.h"

static void error(const char* msg, const char* msg1)
{
    fprintf(stderr, "ERROR: %s %s\n", msg, msg1?msg1:"");
    exit(1);
}


toml_table_t* config_load(char* path) {
    FILE* fp;
    char errbuf[200];

    // 1. Read and parse toml file
    fp = fopen(path, "r");
    if (!fp) {
        error("cannot open toml config - ", strerror(errno));
    }

    toml_table_t* conf = toml_parse_file(fp, errbuf, sizeof(errbuf));
    fclose(fp);

    if (!conf) {
        error("cannot parse - ", errbuf);
    }

    return conf;
}

char* toml_string_get(toml_table_t* table, char* key) {
    toml_datum_t d = toml_string_in(table, key);
    if (!d.ok) {
        error("cannot read string key", key);
    }
    return d.u.s;
}

float toml_double_get(toml_table_t* table, char* key) {
    toml_datum_t d = toml_double_in(table, key);
    if (!d.ok) {
        error("cannot read double key", key);
    }
    return d.u.d;
}

long int toml_int_get(toml_table_t* table, char* key) {
    toml_datum_t d = toml_int_in(table, key);
    if (!d.ok) {
        error("cannot read int key", key);
    }
    return d.u.i;
}

toml_table_t* toml_table_get(toml_table_t* table, char* key) {
    toml_table_t* t = toml_table_in(table, key);
    if (!t) {
        error("cannot read table key", key);
    }
    return t;
}

toml_array_t* toml_array_get(toml_table_t* table, char* key) {
    toml_array_t* t = toml_array_in(table, key);
    if (!t) {
        error("cannot read array key", key);
    }
    return t;
}
