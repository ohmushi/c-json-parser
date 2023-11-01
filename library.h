#ifndef C_JSON_PARSER_LIBRARY_H
#define C_JSON_PARSER_LIBRARY_H

#include "stdlib.h"
#include "stdbool.h"

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

    // if number, 0 or 1 if boolean
    long number;

    // if string
    char *string;

    // if array or node
    size_t nb_elements;
    char **keys;
    Json *values;
};

typedef struct KeyValuePair KeyValuePair;
struct KeyValuePair{
    char* key;
    Json value;
};

typedef struct Parsed Parsed;
struct Parsed {
    char *start;
    char *end;

    /**
     * a: array
     * o: node
     * s: string
     * p: key_value_pair
     * x: empty
     */
    char type;
    union {
        char* string;
        Json node;
        KeyValuePair key_value_pair;
    };
};


typedef struct NextValueInString NextValueInString;
struct NextValueInString {
    Json json;
    char *start;
    char *end;
};

/**
 * @brief Parses a string and returns a Json struct
 *
 * @return The parsed Json node (gives ownership)
 * @errors Returns NULL in the following cases:
 * - bad .json format
 * - cannot allocate memory
 * @param json_string
 */
Json *parse_json(const char *json_string);

void free_json(Json *json);

void clean_json(Json *json);

Json empty_json_object();

Json empty_json_array();

Json json_string(char *string);

Parsed get_first_string_between_double_quote(const char *string);

Parsed parse_key_value_pair(const char* string);

NextValueInString get_next_value_in_string(const char *string);

JsonTokenType get_type_of_next_value(const char *string);

bool expect_next_value(const char* string);

bool is_white_space(const char c);

void push_key_value_pair_in_json(char *key, Json value, Json *json);

void push_value_in_json(Json value, Json* json);

Parsed parse_json_object(const char *string);

Parsed parse_json_array(const char* string);


#endif //C_JSON_PARSER_LIBRARY_H
