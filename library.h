#ifndef C_JSON_PARSER_LIBRARY_H
#define C_JSON_PARSER_LIBRARY_H

#include "stdlib.h"

typedef struct Json Json;
struct Json {
    /**
     * a: array
     * o: object
     * s: string
     * n: number
     */
    char type;

    // if number
    int number;

    // if string
    char *string;

    // if array or object
    size_t nb_elements;
    char **keys;
    Json *values;
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

void clean_json(Json *json);

char *get_first_key_in_string(const char *string);


#endif //C_JSON_PARSER_LIBRARY_H
