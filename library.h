#ifndef C_JSON_PARSER_LIBRARY_H
#define C_JSON_PARSER_LIBRARY_H

#include "stdlib.h"

typedef struct Json Json;
struct Json {
    /**
     * a: array
     * o: object
     * s: value
     * n: number
     * \0: null
     */
    char type;

    // if number
    int number;

    // if value
    char *string;

    // if array or object
    size_t nb_elements;
    char **keys;
    Json *values;
};

typedef struct StringParsed StringParsed;
struct StringParsed {
    char *value;
    char* start;
    char* end;
};


/**
 * @example : { "a key" : "a value" }
 * - key = 'a key'
 * - value->string = 'a value'
 * - start = a key" : "a value" }
 * - end = " }
 *
 * @property start : pointer to first char of the value
 *      - '{' for an object
 *      - '[' for an array
 *      - the first char for a string
 *      - the first digit for a number
 *
 * @property end : pointer to last char of the value
 *      - '}' for an object
 *      - ']' for an array
 *      - the last char for a string
 *      - the last digit for a number
 */
typedef struct KeyValuePairParsed KeyValuePairParsed;
struct KeyValuePairParsed{
    char* key;
    Json *value;
    char* start;
    const char *end;
};

typedef struct NextValueInString NextValueInString;
struct NextValueInString {
    char type;
    char *start;
    char *end;
};

/**
 * @brief Parses a value and returns a Json struct
 *
 * @return The parsed Json object (gives ownership)
 * @errors Returns NULL in the following cases:
 * - bad .json format
 * - cannot allocate memory
 * @param json_string
 */
Json *parse_json(const char *json_string);

void clean_json(Json *json);

StringParsed get_first_string_between_double_quote(const char *string);

KeyValuePairParsed parse_key_value_pair(const char* string);

//NextValueInString get_next_value_in_string(const char * string);

#endif //C_JSON_PARSER_LIBRARY_H
