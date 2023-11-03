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

    long number; // if number, 0/1 if boolean
    char *string; // if string

    // if array or node
    size_t nb_elements;
    char **keys;
    Json *values;
};

typedef struct KeyValuePair KeyValuePair;
struct KeyValuePair {
    char *key;
    Json value;
};

typedef enum JsonParsedType JsonParsedType;
enum JsonParsedType {
    j_array_p,
    j_object_p,
    j_string_p,
    j_key_value_pair_p,
    j_empty_p
};
typedef struct Parsed Parsed;
struct Parsed {
    char *start;
    char *end;

    JsonParsedType type;
    union {
        char *string;
        Json node;
        KeyValuePair key_value_pair;
    };
};


typedef struct NextValue NextValue;
struct NextValue {
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

Parsed parse_key_value_pair(const char *string);

NextValue get_next_value(const char *string);

JsonTokenType get_type_of_next_value(const char *string);

bool expect_next_value(const char *string);

bool is_white_space(const char c);

void push_key_value_pair_in_object(char *key, Json value, Json *json);

void push_value_in_array(Json value, Json *json);

Parsed parse_json_object(const char *string);

Parsed parse_json_array(const char *string);

NextValue get_next_null_value(const char *string);

NextValue get_next_boolean_value(const char *string);


#endif //C_JSON_PARSER_LIBRARY_H
