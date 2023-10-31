#ifndef C_JSON_PARSER_LIBRARY_H
#define C_JSON_PARSER_LIBRARY_H

#include "stdlib.h"
#include "stdbool.h"

typedef struct Json Json;
struct Json {
    /**
     * a: array
     * o: object
     * s: string
     * n: number
     * b: boolean
     * \0: null
     *
     * x: empty
     */
    char type;

    // if number, 0 or 1 if boolean
    long number;

    // if string
    char *string;

    // if array or object
    size_t nb_elements;
    char **keys;
    Json *values;
};

typedef struct Parsed Parsed;
struct Parsed {
    char *start;
    char *end;
    char type;
    union {
        char* string;
    };
};


/**
 * @example : { "a key" : "a string" }
 * - key = 'a key'
 * - string->string = 'a string'
 * - start = 'a key" : "a string" }'
 * - end = '" }'
 *
 * @property start : pointer to first char of the string
 *      - '{' for an object
 *      - '[' for an array
 *      - the first char for a string
 *      - the first digit for a number
 *
 * @property end : pointer to last char of the string
 *      - '}' for an object
 *      - ']' for an array
 *      - the last char for a string
 *      - the last digit for a number
 */
typedef struct KeyValuePairParsed KeyValuePairParsed;
struct KeyValuePairParsed{
    char* key;
    Json value;
    char* start;
    const char *end;
};


// TODO Make one struct Parsed with union, not one struct per type parsed
/**
 * @example : ': {"k": "v"}, '
 * - start = '{"k": "v"}, '
 * - end = '}, '
 * - object
 *      - type = 's'
 *      - key[0] = 'k'
 *      - values[0].string = 'v'
 */
typedef struct ObjectParsed ObjectParsed;
struct ObjectParsed{
    Json object;
    char* start;
    char *end;
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
 * @return The parsed Json object (gives ownership)
 * @errors Returns NULL in the following cases:
 * - bad .json format
 * - cannot allocate memory
 * @param json_string
 */
Json *parse_json(const char *json_string);

void free_json(Json *json);

void clean_json(Json *json);

Json empty_json_object();

Json json_string(char *string);

Parsed get_first_string_between_double_quote(const char *string);

KeyValuePairParsed parse_key_value_pair(const char* string);

NextValueInString get_next_value_in_string(const char *string);

/**
 * @errors no string is detected return 'x'
 * @param string
 * @return 'a' -> array, 'o' -> object,
 * 's' -> string, 'n' -> number, '\0' -> null
 */
char get_type_of_next_value(const char *string);

bool expect_next_value(const char* string);

bool is_white_space(const char c);

void push_key_value_pair_in_json(char *key, Json value, Json *json);

#endif //C_JSON_PARSER_LIBRARY_H
