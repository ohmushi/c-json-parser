#include "library.h"

#include <stdio.h>
#include <string.h>

Json *empty_json_object();

Json *json_string(char *string);

char get_type_of_next_value(char *string);

Json *parse_json(const char *json_string) {
    if(json_string == NULL) return NULL;
    // TODO trim left
    if(json_string[0] != '{') return NULL;
    Json *json_ptr = empty_json_object();
    json_ptr->nb_elements += 1;
    StringParsed key = get_first_string_between_double_quote(json_string + 1);
    if(json_ptr->keys == NULL) json_ptr->keys = malloc(sizeof(char*) * 1);
    else json_ptr->keys = realloc(json_ptr->keys, sizeof(char*) * json_ptr->nb_elements);
    json_ptr->keys[json_ptr->nb_elements-1] = key.value;
    return json_ptr;
}

Json *empty_json_object() {
    Json *obj = malloc(sizeof(Json));
    *obj = (Json){
            .type = 'o',
            .number = 0,
            .string = NULL,
            .nb_elements = 0,
            .keys = NULL,
            .values = NULL,
    };
    return obj;
}

Json *json_string(char *string) {
    Json *obj = malloc(sizeof(Json));
    *obj = (Json){
            .type = 's',
            .number = 0,
            .string = string,
            .nb_elements = 0,
            .keys = NULL,
            .values = NULL,
    };
    return obj;
}

StringParsed get_first_string_between_double_quote(const char *string) {
    printf("\nget_first_string_between_double_quote string: [%s]\n", string);
    uint16_t len = strlen(string);
    char* start = (char*)string;
    while(start != NULL && *start != '\0' && *start != '"') {
        start += 1;
    }
    start += 1;
    char* end = start;
    while(end != NULL && *end != '\0' && *end != '"') {
        end += 1;
    }
    StringParsed parsed = {.value = NULL, .start = start, .end = end};
    if(end <= start || *end == '\0') return parsed;

    uint16_t key_len = end - start;
    char *key = malloc(sizeof(char) * key_len+1);
    strncpy(key, start, key_len);
    key[key_len] = '\0';
    parsed.value = key;
    return parsed;
}

void clean_json(Json *json) {
    if(json == NULL) return;
    if(json->string != NULL) {
        free(json->string);
        json->string = NULL;
    }
    if(json->keys != NULL) {
        for(uint8_t i = 0; i < json->nb_elements; i++) {
            if(json->keys[i] != NULL) {
                free(json->keys[i]);
                json->keys[i] = NULL;
            }
            if(json->values != NULL) {
                clean_json(&json->values[i]);
            }
        }
        free(json->keys);
        json->keys = NULL;
    }
    free(json);
}

KeyValuePairParsed parse_key_value_pair(const char* string) {
    KeyValuePairParsed parsed = {
            .key = NULL,
            .value = NULL,
            .start = NULL,
            .end = NULL,
    };
    StringParsed key = get_first_string_between_double_quote(string);
    if(key.value == NULL) return parsed;

    StringParsed value = get_first_string_between_double_quote(key.end+2);
    parsed.key = key.value;
    parsed.start = key.start - 1;
    parsed.end = value.end + 1;
    parsed.value = json_string(value.value);

    return parsed;
}

char get_type_of_next_value(char *string) {
    // TODO
    return 's';
}

