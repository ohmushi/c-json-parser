#include "library.h"

#include <stdio.h>
#include <string.h>

Json *parse_json(const char *json_string) {
    if(json_string == NULL) return NULL;
    if(json_string[0] != '{') return NULL;
    Json *json_ptr = malloc(sizeof(Json));
    *json_ptr = (Json){
            .type = 'o',
            .number = 0,
            .string = NULL,
            .nb_elements = 0,
            .keys = NULL,
            .values = NULL,
    };
    json_ptr->nb_elements += 1;
    char* key = get_first_key_in_string(json_string + 1);
    if(json_ptr->keys == NULL) json_ptr->keys = malloc(sizeof(char*) * 1);
    else json_ptr->keys = realloc(json_ptr->keys, sizeof(char*) * json_ptr->nb_elements);
    return json_ptr;
}

char *get_first_key_in_string(const char *string) {
    uint16_t len = strlen(string);
    char cpy[len];
    strcpy(cpy, string);
    char* start = cpy;
    while(start != NULL && *start != '\0' && *start != '"') {
        start += 1;
    }
    start += 1;
    char* end = start;
    while(end != NULL && *end != '\0' && *end != '"') {
        end += 1;
    }
    if(end <= start) return NULL;
    uint16_t key_len = end - start;
    char *key = malloc(sizeof(char) * key_len+1);
    strncpy(key, start, key_len);
    key[key_len] = '\0';
    return key;
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