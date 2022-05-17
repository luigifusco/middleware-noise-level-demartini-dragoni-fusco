#ifndef __CONFIG_H__
#define __CONFIG_H__

#include "toml/toml.h"

toml_table_t* config_load(char* path);

char* toml_string_get(toml_table_t* table, char* key);
float toml_double_get(toml_table_t* table, char* key);
long int toml_int_get(toml_table_t* table, char* key);
toml_table_t* toml_table_get(toml_table_t* table, char* key);
toml_array_t* toml_array_get(toml_table_t* table, char* key);

#endif