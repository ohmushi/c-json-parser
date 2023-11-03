#include "library.h"

#include <stdio.h>
#include <string.h>
#include "stdbool.h"

Json no_json();

NextValue get_next_string_value(const char *string);

NextValue get_next_number_value(const char *string);

Json json_number(long number);

NextValue get_next_object_value(const char *string);

NextValue get_next_array_value(const char *string);

Parsed not_parsed();

char *get_next_non_white_char(char *string);

NextValue no_next_value();

Json *parse_json(const char *json_string) {
    if (json_string == NULL) return NULL;
    JsonTokenType type = get_type_of_next_value(json_string);
    if (type != j_object && type != j_array) return NULL;
    Json *node = malloc(sizeof(Json));
    NextValue root = get_next_value(json_string);
    if (root.json.type != j_object && root.json.type != j_array) return NULL;
    *node = root.json;

    return node;
}

Parsed parse_json_object(const char *string) {
    Json node = empty_json_object();
    char *start = get_next_non_white_char((char *) string);
    if (*start != '{') return not_parsed();

    char *next_non_white_char = get_next_non_white_char(start + 1);
    if (*next_non_white_char == '}')
        return (Parsed) {.start = start, .node = empty_json_object(), .end = next_non_white_char, .type = j_object_p};

    Parsed kvp = parse_key_value_pair(string);
    if (kvp.type == j_empty_p || kvp.key_value_pair.key == NULL) return not_parsed();

    push_key_value_pair_in_object(kvp.key_value_pair.key, kvp.key_value_pair.value, &node);
    while (expect_next_value(kvp.end)) {
        kvp = parse_key_value_pair(kvp.end);
        if (kvp.key_value_pair.key == NULL) break;
        push_key_value_pair_in_object(kvp.key_value_pair.key, kvp.key_value_pair.value, &node);
    }

    char *end = get_next_non_white_char(kvp.end);
    if (*end != '}') return not_parsed();

    return (Parsed) {.start = start, .node = node, .end = end, .type = j_object_p};
}

char *get_next_non_white_char(char *string) {
    while (*string != '\0' && is_white_space(*string)) string++;
    return string;
}

Json no_json() {
    return (Json) {
            .type = j_empty,
            .number = 0,
            .string = NULL,
            .nb_elements = 0,
            .keys = NULL,
            .values = NULL,
    };
}

Json empty_json_object() {
    Json json = no_json();
    json.type = j_object;
    return json;
}

Json empty_json_array() {
    Json json = no_json();
    json.type = j_array;
    return json;
}

Json json_string(char *string) {
    Json json = no_json();
    json.type = j_string;
    json.string = string;
    return json;
}

Json json_number(long number) {
    Json json = no_json();
    json.type = j_number;
    json.number = number;
    return json;
}

Json json_null() {
    Json json = no_json();
    json.type = j_null;
    return json;
}

Json json_boolean(bool boolean) {
    Json json = no_json();
    json.type = j_boolean;
    json.number = boolean;
    return json;
}

Parsed get_first_string_between_double_quote(const char *string) {
    char *start = (char *) string;
    while (start != NULL && *start != '\0' && *start != '"') {
        start += 1;
    }
    start += 1;
    char *end = start;
    while (end != NULL && *end != '\0' && *end != '"') {
        end += 1;
    }
    if (end <= start || *end == '\0') return not_parsed();

    uint16_t key_len = end - start;
    char *key = malloc(sizeof(char) * key_len + 1);
    strncpy(key, start, key_len);
    key[key_len] = '\0';
    return (Parsed) {.start = start, .end = end, .type = j_string_p, .string = key};
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

Parsed parse_key_value_pair(const char *string) {
    Parsed key = get_first_string_between_double_quote(string);
    if (key.type != j_string_p || key.string == NULL) return not_parsed();

    char *dot = get_next_non_white_char(key.end + 1);
    if (*dot != ':') return not_parsed();

    NextValue value = get_next_value(dot + 1);
    if (value.json.type == j_empty) return not_parsed();

    return (Parsed) {
            .type = j_key_value_pair_p,
            .start = key.start - 1,
            .end = value.end + 1,
            .key_value_pair = (KeyValuePair) {.key = key.string, .value = value.json},
    };
}

NextValue get_next_value(const char *string) {
    JsonTokenType type = get_type_of_next_value(string);
    switch (type) {
        case j_string:
            return get_next_string_value(string);
        case j_number:
            return get_next_number_value(string);
        case j_object:
            return get_next_object_value(string);
        case j_array:
            return get_next_array_value(string);
        case j_null:
            return get_next_null_value(string);
        case j_boolean:
            return get_next_boolean_value(string);
        default:
            return no_next_value();
    }
}

NextValue no_next_value() { return (NextValue) {.end = NULL, .start = NULL, .json = no_json()}; }

NextValue get_next_string_value(const char *string) {
    Parsed value = get_first_string_between_double_quote(string);
    if (value.type != j_string_p) return no_next_value();

    return (NextValue) {
            .start = value.start,
            .end = value.end,
            .json = json_string(value.string),
    };
}

NextValue get_next_number_value(const char *string) {
    char *start = get_next_non_white_char((char *) string);

    char *end;
    Json value = json_number(strtol(start, &end, 10));

    if (end == NULL || end <= start) return no_next_value();
    return (NextValue) {.start = start, .end = end - 1, .json = value};
}

NextValue get_next_object_value(const char *string) {
    char *c = get_next_non_white_char((char *) string);
    if (*c != '{') return no_next_value();

    Parsed obj = parse_json_object(c);
    return (NextValue) {.start = obj.start, .json = obj.node, .end = obj.end};
}

NextValue get_next_array_value(const char *string) {
    char *c = get_next_non_white_char((char *) string);
    if (*c != '[') return no_next_value();

    Parsed array = parse_json_array(c);
    return (NextValue) {.start = array.start, .json = array.node, .end = array.end};
}

NextValue get_next_null_value(const char *string) {
    char *c = get_next_non_white_char((char *) string);
    if (*c == '\0' || strncmp(c, "null", 4) != 0)
        return no_next_value();

    return (NextValue) {.start = c, .json = json_null(), .end = c + 3};
}

NextValue get_next_boolean_value(const char *string) {
    char *c = get_next_non_white_char((char *) string);
    if (*c != 't' && *c != 'f') return no_next_value();
    if (strncmp(c, "true", 4) == 0)
        return (NextValue) {.start = c, .end = c + 3, .json = json_boolean(true)};
    else if (strncmp(c, "false", 5) == 0)
        return (NextValue) {.start = c, .end = c + 4, .json = json_boolean(false)};
    else return no_next_value();

}


JsonTokenType get_type_of_next_value(const char *string) {
    char *c = (char *) string;
    while (*c != '\0') {
        switch (*c) {
            case '[':
                return j_array;
            case '{':
                return j_object;
            case '"':
                return j_string;
            case 'n':
                if (strncmp(c, "null", 4) == 0) return j_null;
                break;
            case 't':
            case 'f':
                if (strncmp(c, "true", 4) == 0 || strncmp(c, "false", 5) == 0) return j_boolean;
                break;
            default:
                if (*c >= '0' && *c <= '9') return j_number;
        }
        c++;
    }
    return j_empty;
}

bool expect_next_value(const char *string) {
    char *c = get_next_non_white_char((char *) string);
    return *c == ',';
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

void push_key_value_pair_in_object(char *key, Json value, Json *json) {
    if (json->type != j_object) return;
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

void push_value_in_array(Json value, Json *json) {
    if (json->type != j_array) return;

    json->nb_elements += 1;
    json->keys = NULL;
    if (json->nb_elements == 1) {
        json->values = malloc(sizeof(Json));
    } else if (json->nb_elements > 1) {
        json->values = realloc(json->values, sizeof(Json) * json->nb_elements);
    } else return;

    json->values[json->nb_elements - 1] = value;
}

Parsed parse_json_array(const char *string) {
    Json node = empty_json_array();
    char *start = get_next_non_white_char((char *) string);
    if (*start != '[') return not_parsed();

    char *close_array = get_next_non_white_char(start + 1);
    if (*close_array == ']')
        return (Parsed) {.start = start, .node = empty_json_array(), .end = close_array, .type = j_array_p};


    NextValue next = get_next_value(start + 1);
    if (next.json.type == j_empty) return not_parsed();
    push_value_in_array(next.json, &node);
    while (expect_next_value(next.end + 1)) {
        char *comma = get_next_non_white_char(next.end + 1);
        next = get_next_value(comma + 1);
        if (next.json.type == j_empty) return not_parsed();
        push_value_in_array(next.json, &node);
    }

    char *end = get_next_non_white_char(next.end + 1);
    if (*end != ']') return not_parsed();

    return (Parsed) {.start = start, .end = end, .type = j_array_p, .node = node};
}

Parsed not_parsed() {
    return (Parsed) {.start = NULL, .node = no_json(), .end = NULL, .type = j_empty_p};
}
