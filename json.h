#ifndef C_JSON_PARSER_JSON_H
#define C_JSON_PARSER_JSON_H

#include "stdlib.h"

typedef enum JsonTokenType JsonTokenType;
enum JsonTokenType {
    j_array,
    j_object,
    j_string,
    j_number,
    j_boolean,
    j_null,
    j_empty,
};

typedef struct Json Json;
struct Json {
    JsonTokenType type;

    long number; // if number, 0/1 if boolean
    char *string; // if string

    // if array or node
    size_t nb_elements;
    char **keys;
    Json *values;
};


#endif //C_JSON_PARSER_JSON_H
