#include <string.h>
#include <stdlib.h>
#include <printf.h>

#include "library.h"
#include "minunit.h"

static char* test_parse_null();

static char* test_get_first_key_at();

static char* test_get_first_key();

static char* test_parse_simple_json();

static char *test_nested_json();

int tests_run = 0;

static char * all_tests() {
    mu_run_test(test_parse_null);
    mu_run_test(test_get_first_key_at);
    mu_run_test(test_get_first_key);
    mu_run_test(test_parse_simple_json);
    mu_run_test(test_nested_json);
    return EXIT_SUCCESS;
}

int main(int argc, char **argv) {
    char *result = all_tests();
    if (result != EXIT_SUCCESS) {
        printf("%s\n", result);
    }
    else {
        printf("ALL TESTS PASSED\n");
    }
    printf("Tests run: %d\n", tests_run);

    return result != EXIT_SUCCESS;
}

static char* test_parse_null() {
    Json *json_obj = parse_json(NULL);
    mu_assert("error, the json_obj should be null", json_obj == NULL);
    clean_json(json_obj);
    return EXIT_SUCCESS;
}

static char* test_get_first_key_at() {
    const char* str = " \"a key\" : \"then a value\"";
    char *key = get_first_key_in_string(str);
    mu_assert_not_null("test_get_first_key_at", key);
    mu_assert_strings_equals("test_get_first_key_at", key, "a key");
    free(key);

    mu_assert_null("test_get_first_key_at", get_first_key_in_string("\"\""));

    return EXIT_SUCCESS;
}

static char* test_get_first_key() {
    const char *json_str = "{\"name\":\"Paul\", \"age\":20, \"city\":\"Paris\"}";
    Json *json_obj = parse_json(json_str);

    mu_assert_not_null("test_get_first_key", json_obj);
    mu_assert_strings_equals("test_get_first_key", json_obj->keys[0], "name");

    clean_json(json_obj);
    return EXIT_SUCCESS;
}

static char* test_parse_simple_json() {
    const char *json_str = "{\"name\":\"Paul\", \"age\":20, \"city\":\"Paris\"}";
    Json *json_obj = parse_json(json_str);

    mu_assert("error, the json_obj should be valid ", json_obj != NULL);
    mu_assert("error, json_obj->type != 'o'", json_obj->type == 'o');
    mu_assert("error, json_obj->nb_elements != 3", json_obj->nb_elements == 3);
    mu_assert("error, key[0] != name", strcmp(json_obj->keys[0], "name") == 0);
    mu_assert("error, value[0] != Paul", strcmp(json_obj->values[0].string, "Paul") == 0);
    mu_assert("error, key[1] != age", strcmp(json_obj->keys[1], "age") == 0);
    mu_assert("error, value[1] != 20", json_obj->values[1].number == 20);
    mu_assert("error, key[2] != city", strcmp(json_obj->keys[2], "city") == 0);
    mu_assert("error, value[2] != Paris", strcmp(json_obj->values[2].string, "Paris") == 0);

    clean_json(json_obj);
    return EXIT_SUCCESS;
}

static char *test_nested_json() {
    const char *json_str =
            "{"
            "\"name\":\"Paul\","
            "\"age\":20,"
            "\"city\":\"Paris\","
            "\"friends\":[\"Anaelle\",\"Noam\", \"Tom\"],"
            "\"address\":{"
            "\"street\":\"1 rue du Pain\","
            "\"zipcode\":\"12345\""
            "}"
            "}";

    Json *json_obj = parse_json(json_str);

    mu_assert("error, the json_obj should be valid", json_obj != NULL);
    mu_assert("error, json_obj->type != 'o'", json_obj->type == 'o');
    mu_assert("error, json_obj->nb_elements != 5", json_obj->nb_elements == 5);

    // simple fields
    mu_assert("error, key[0] != name", strcmp(json_obj->keys[0], "name") == 0);
    mu_assert("error, value[0] != Paul", strcmp(json_obj->values[0].string, "Paul") == 0);
    mu_assert("error, key[1] != age", strcmp(json_obj->keys[1], "age") == 0);
    mu_assert("error, value[1] != 20", json_obj->values[1].number == 20);
    mu_assert("error, key[2] != city", strcmp(json_obj->keys[2], "city") == 0);
    mu_assert("error, value[2] != Paris", strcmp(json_obj->values[2].string, "Paris") == 0);

    // array
    mu_assert("error, key[3] != friends", strcmp(json_obj->keys[3], "friends") == 0);
    mu_assert("error, value[3].type != 'a'", json_obj->values[3].type == 'a');
    mu_assert("error, value[3].nb_elements != 2", json_obj->values[3].nb_elements == 3);
    mu_assert("error, value[3].values[0] != Anaelle", strcmp(json_obj->values[3].values[0].string, "Anaelle") == 0);
    mu_assert("error, value[3].values[1] != Noam", strcmp(json_obj->values[3].values[1].string, "Noam") == 0);
    mu_assert("error, value[3].values[2] != Tom", strcmp(json_obj->values[3].values[2].string, "Tom") == 0);

    // nested
    mu_assert("error, key[4] != address", strcmp(json_obj->keys[4], "address") == 0);
    mu_assert("error, value[4].type != 'o'", json_obj->values[4].type == 'o');
    mu_assert("error, value[4].nb_elements != 2", json_obj->values[4].nb_elements == 2);
    mu_assert("error, value[4].keys[0] != street", strcmp(json_obj->values[4].keys[0], "street") == 0);
    mu_assert("error, value[4].values[0] != 1 rue du Pain", strcmp(json_obj->values[4].values[0].string, "1 rue du Pain") == 0);
    mu_assert("error, value[4].keys[1] != zipcode", strcmp(json_obj->values[4].keys[1], "zipcode") == 0);
    mu_assert("error, value[4].values[1] != 12345", strcmp(json_obj->values[4].values[1].string, "12345") == 0);

    clean_json(json_obj);
    return EXIT_SUCCESS;
}