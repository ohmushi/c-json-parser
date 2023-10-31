#include "library.h"

#include <stdio.h>
#include <string.h>

Json no_json();

Json empty_json_object();

Json json_string(char *string);


NextValueInString get_next_string_value_in_string(const char *string);

NextValueInString get_next_number_value_in_string(const char *string);

Json json_number(long strtol);

Json *parse_json(const char *json_string) {
    if (json_string == NULL) return NULL;
    // TODO trim left
    if (json_string[0] != '{') return NULL;
    Json *json_ptr = malloc(sizeof(Json));
    *json_ptr = empty_json_object();

    KeyValuePairParsed key_value_pair = parse_key_value_pair(json_string);
    if(key_value_pair.key == NULL) return json_ptr;

    json_ptr->nb_elements = 1;
    json_ptr->keys = malloc(sizeof(char *));
    json_ptr->keys[json_ptr->nb_elements - 1] = key_value_pair.key;

    json_ptr->values = malloc(sizeof(Json));
    json_ptr->values[json_ptr->nb_elements - 1] = key_value_pair.value;

    while (expect_next_value(key_value_pair.end)) {
        key_value_pair = parse_key_value_pair(key_value_pair.end);
        json_ptr->nb_elements += 1;

        json_ptr->keys = realloc(json_ptr->keys, sizeof(char *) * json_ptr->nb_elements);
        json_ptr->values = realloc(json_ptr->values, sizeof(Json) * json_ptr->nb_elements);

        json_ptr->keys[json_ptr->nb_elements - 1] = key_value_pair.key;
        json_ptr->values[json_ptr->nb_elements - 1] = key_value_pair.value;

    };
    printf("\n nb keys : %d", json_ptr->nb_elements);

    return json_ptr;
}

Json no_json() {
    return (Json) {
            .type = 'x',
            .number = 0,
            .string = NULL,
            .nb_elements = 0,
            .keys = NULL,
            .values = NULL,
    };
}

Json empty_json_object() {
    Json json = no_json();
    json.type = 'o';
    return json;
}

Json json_string(char *string) {
    Json json = no_json();
    json.type = 's';
    json.string = string;
    return json;
}

Json json_number(long number) {
    Json json = no_json();
    json.type = 'n';
    json.number = number;
    return json;
}


StringParsed get_first_string_between_double_quote(const char *string) {
    char *start = (char *) string;
    while (start != NULL && *start != '\0' && *start != '"') {
        start += 1;
    }
    start += 1;
    char *end = start;
    while (end != NULL && *end != '\0' && *end != '"') {
        end += 1;
    }
    StringParsed parsed = {.value = NULL, .start = start, .end = end};
    if (end <= start || *end == '\0') return parsed;

    uint16_t key_len = end - start;
    char *key = malloc(sizeof(char) * key_len + 1);
    strncpy(key, start, key_len);
    key[key_len] = '\0';
    parsed.value = key;
    return parsed;
}

void free_json(Json *json) {
    clean_json(json);
    free(json);
}

void clean_json(Json *json) {
    if (json == NULL) return;
    if (json->string != NULL) {
        free(json->string);
        json->string = NULL;
    }
    if (json->keys != NULL) {
        for (uint8_t i = 0; i < json->nb_elements; i++) {
            if (json->keys[i] != NULL) {
                free(json->keys[i]);
                json->keys[i] = NULL;
            }
            if (json->values != NULL) {
                //clean_json(&json->values[i]);
            }
        }
        free(json->keys);
        json->keys = NULL;
    }
}

KeyValuePairParsed parse_key_value_pair(const char *string) {
    KeyValuePairParsed parsed = {
            .key = NULL,
            .start = NULL,
            .end = NULL,
    };
    StringParsed key = get_first_string_between_double_quote(string);
    if (key.value == NULL) return parsed;

    NextValueInString value = get_next_value_in_string(key.end + 2);
    parsed.key = key.value;
    parsed.start = key.start - 1;
    parsed.end = value.end + 1;
    parsed.value = value.json;

    return parsed;
}

NextValueInString get_next_value_in_string(const char *string) {
    // TODO obj, array, number, null
    char type = get_type_of_next_value(string);
    switch (type) {
        case 's':
            return get_next_string_value_in_string(string);
        case 'n':
            return get_next_number_value_in_string(string);
            // TODO array
            // TODO obj
            // TODO null
        default:
            return (NextValueInString) {.end = NULL, .start = NULL, .json = no_json()};
    }
}

NextValueInString get_next_string_value_in_string(const char *string) {
    StringParsed value = get_first_string_between_double_quote(string);
    return (NextValueInString) {
            .start = value.start,
            .end = value.end,
            .json = json_string(value.value),
    };
}

NextValueInString get_next_number_value_in_string(const char *string) {
    char *start = (char *) string;
    while (*start != '\0' && *start != '-' && (*start < '0' || *start > '9')) {
        start++;
    }

    char *end;
    Json value = json_number(strtol(start, &end, 10));

    if(end == NULL || end <= start) return (NextValueInString) {.start = NULL, .end = NULL, .json = no_json()};
    return (NextValueInString) {.start = start, .end = end-1, .json = value};
}

char get_type_of_next_value(const char *string) {
    char *i = (char *) string;
    while (*i != '\0') {
        switch (*i) {
            case '[':
                return 'a';
            case '{':
                return 'o';
            case '"':
                return 's';
            case 'n':
                if (strncmp(i, "null", 4) == 0) return '\0';
                break;
            default:
                if (*i >= '0' && *i <= '9') return 'n';
        }
        i++;
    }
    return 'x';
}

bool expect_next_value(const char *string) {
    char *c = (char *) string;
    while (*c != '\0') {
        if (!is_white_space(*c)) return *c == ',';
        c++;
    }
    return false;
}

bool is_white_space(const char c) {
    switch (c) {
        case ' ':
        case '\n':
        case '\t':
        case '\r':
            return true;
        default:
            return false;
    }
}

