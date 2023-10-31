#include "library.h"

#include <stdio.h>
#include <string.h>

Json no_json();


NextValueInString get_next_string_value_in_string(const char *string);

NextValueInString get_next_number_value_in_string(const char *string);

Json json_number(long strtol);

NextValueInString get_next_object_value_in_string(const char *string);

Json parse_json_object(const char *string);

Json *parse_json(const char *json_string) {
    if (json_string == NULL) return NULL;
    char type = get_type_of_next_value(json_string);
    if (type != 'o') return NULL;
    Json *node = malloc(sizeof(Json));
    *node = parse_json_object(json_string);

    return node;
}

Json parse_json_object(const char *string) {
    Json node = empty_json_object();
    KeyValuePairParsed key_value_pair = parse_key_value_pair(string);
    if (key_value_pair.key == NULL) return node;

    push_key_value_pair_in_json(key_value_pair.key, key_value_pair.value, &node);
    while (expect_next_value(key_value_pair.end)) {
        key_value_pair = parse_key_value_pair(key_value_pair.end);
        if(key_value_pair.key == NULL) break;
        push_key_value_pair_in_json(key_value_pair.key, key_value_pair.value, &node);
    }
    return node;
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

    // TODO check the ':' between key and value

    NextValueInString value = get_next_value_in_string(key.end + 2);
    if (value.json.type == 'x')
        return (KeyValuePairParsed) {.start = NULL, .key = NULL, .value = no_json(), .end = NULL};
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
            //case 'o':
            //return get_next_object_value_in_string(string);
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

    if (end == NULL || end <= start) return (NextValueInString) {.start = NULL, .end = NULL, .json = no_json()};
    return (NextValueInString) {.start = start, .end = end - 1, .json = value};
}

NextValueInString get_next_object_value_in_string(const char *string) {
    char *c = (char *) string;
    while (*c != '\0') {
        // TODO check white space
        if (*c == '{') break;
        c++;
    }
    printf("\nstring: [%s]\n", c);
    NextValueInString result;
    return result;
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

void push_key_value_pair_in_json(char *key, Json value, Json *json) {
    json->nb_elements += 1;
    if (json->nb_elements == 1) {
        json->keys = malloc(sizeof(char *));
        json->values = malloc(sizeof(Json));
    } else if (json->nb_elements > 1) {
        json->keys = realloc(json->keys, sizeof(char *) * json->nb_elements);
        json->values = realloc(json->values, sizeof(Json) * json->nb_elements);
    } else return;

    json->keys[json->nb_elements - 1] = key;
    json->values[json->nb_elements - 1] = value;
}
