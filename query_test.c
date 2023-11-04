#include <string.h>
#include <stdlib.h>
#include <printf.h>

#include "query.h"
#include "minunit.h"

static char *test_parse_json_empty();

static char *test_query_person_name();

static char *test_query_person_age();

static char *test_path_not_start_with_slash();

static char *test_get_next_key();

static char *test_query_not_found();

static char *test_query_array();

static char *test_nested_object();

int tests_run = 0;

static char *all_tests() {

    mu_run_test(test_parse_json_empty);
    mu_run_test(test_query_person_name);
    mu_run_test(test_path_not_start_with_slash);
    mu_run_test(test_get_next_key);
    mu_run_test(test_query_person_age);
    mu_run_test(test_query_not_found);
    mu_run_test(test_query_array);
    mu_run_test(test_nested_object);

    return EXIT_SUCCESS;
}


int main(int argc, char **argv) {
    (void) argc; (void) argv;
    char *result = all_tests();
    if (result != EXIT_SUCCESS) {
        printf("%s\n", result);
    } else {
        printf("ALL TESTS PASSED\n");
    }
    printf("Tests run: %d\n", tests_run);

    return result != EXIT_SUCCESS;
}

/*
 * {
 *   "name":"Paul",
 *   "age":20,
 *   "city":"Paris",
 *   "friends":["Anaelle","Noam", "Tom"],
 *   "address":{
 *     "street":"1 rue du Pain",
 *     "zipcode":"12345"
 *   }
 * };
 */
Json person() {
    Json person = {
            .type = j_object,
            .nb_elements = 5,
            .keys = malloc(sizeof(char *) * 5),
            .values = malloc(sizeof(Json) * 5),
    };
    person.keys[0] = "name";
    person.values[0] = (Json) {.type = j_string,.string = "Paul"};

    person.keys[1] = "age";
    person.values[1] = (Json) {.type = j_number,.number = 20};

    person.keys[2] = "city";
    person.values[2] = (Json) {.type = j_string,.string = "Paris"};

    person.keys[3] = "friends";
    person.values[3] = (Json) {
            .type = j_array,
            .nb_elements = 3,
            .values = malloc(sizeof(Json) * 3),
    };
    person.values[3].values[0] = (Json) {.type = j_string,.string = "Anaelle"};
    person.values[3].values[1] = (Json) {.type = j_string,.string = "Noam"};
    person.values[3].values[2] = (Json) {.type = j_string,.string = "Tom"};

    person.keys[4] = "address";
    person.values[4] = (Json) {
            .type = j_object,
            .nb_elements = 2,
            .keys = malloc(sizeof(char *) * 2),
            .values = malloc(sizeof(Json) * 2),
    };
    person.values[4].keys[0] = "street";
    person.values[4].values[0] = (Json) {.type = j_string,.string = "1 rue du Pain"};
    person.values[4].keys[1] = "zipcode";
    person.values[4].values[1] = (Json) {.type = j_string,.string = "12345"};

    return person;
}

/**
 * [1,"two", {"number":3}]
 */
Json array() {
    Json array = {
            .type = j_array,
            .nb_elements = 3,
            .values = malloc(sizeof(Json) * 3),
    };
    array.values[0] = (Json) {.type = j_number,.number = 1};
    array.values[1] = (Json) {.type = j_string,.string = "two"};
    array.values[2] = (Json) {
            .type = j_object,
            .nb_elements = 1,
            .keys = malloc(sizeof(char *)),
            .values = malloc(sizeof(Json)),
    };
    array.values[2].keys[0] = "number";
    array.values[2].values[0] = (Json) {.type = j_number,.number = 3};

    return array;
}

void clean_json(Json json) {
    if(json.nb_elements == 0) return;
    if(json.type == j_array || json.type == j_object) {
        for (size_t i = 0; i < json.nb_elements; i++) {
            clean_json(json.values[i]);
        }
        if(json.type == j_object) free(json.keys);
        free(json.values);
    }
}

static char *test_parse_json_empty() {
    Json empty = (Json) {
        .type = j_empty,
        .number = 0,
        .string = NULL,
        .nb_elements = 0,
        .keys = NULL,
        .values = NULL,
    };
    Query q = query(empty, "/some/path");
    mu_assert_true("Query should be an error", q.tag == q_Left);
    mu_assert_true("Query should be an error", q.error.type == qe_NotFound);
    return EXIT_SUCCESS;
}

static char *test_query_person_name() {
    Json p = person();
    Query q = query(p, "/name");
    mu_assert_true("test_query_person_name should be right", q.tag == q_Right);
    mu_assert_true("test_query_person_name should be a string", q.value.type == j_string);
    mu_assert_strings_equals("test_query_person_name should be Paul",q.value.string, "Paul");
    clean_json(p);
    return EXIT_SUCCESS;
}

static char *test_path_not_start_with_slash() {
    Json p = person();
    Query q = query(p, "name");
    mu_assert_true("test_path_not_start_with_slash should be an error", q.tag == q_Left);
    mu_assert_true("test_path_not_start_with_slash should be an error", q.error.type == qe_InvalidPath);
    clean_json(p);
    return EXIT_SUCCESS;
}

static char *test_get_next_key() {
    PathKey key = next_key("/name");
    mu_assert_strings_equals("test_get_next_key should be name", key.start, "name");
    mu_assert_strings_equals("test_get_next_key should be name", key.end, "");

    PathKey key_with_end = next_key("/name/end");
    mu_assert_strings_equals("test_get_next_key should be name", key_with_end.start, "name/end");
    mu_assert_strings_equals("test_get_next_key should be name", key_with_end.end, "/end");
    return EXIT_SUCCESS;
}

static char *test_query_person_age() {
    Json p = person();
    Query q = query(p, "/age");
    mu_assert_true("test_query_person_age should be right", q.tag == q_Right);
    mu_assert_true("test_query_person_age should be a number", q.value.type == j_number);
    mu_assert_true("test_query_person_age should be 20", q.value.number == 20);
    clean_json(p);
    return EXIT_SUCCESS;
}

static char *test_query_not_found() {
    Json p = person();
    Query q = query(p, "/not_found");
    mu_assert_true("test_query_not_found should be an error", q.tag == q_Left);
    mu_assert_true("test_query_not_found should be an error", q.error.type == qe_NotFound);
    clean_json(p);
    return EXIT_SUCCESS;
}

static char *test_query_array() {
    Json a = array();
    Query q = query(a, "/0");
    mu_assert_true("test_query_array should be right", q.tag == q_Right);
    mu_assert_true("test_query_array should be a number", q.value.type == j_number);
    mu_assert_true("test_query_array should be 1", q.value.number == 1);
    clean_json(a);
    return EXIT_SUCCESS;
}

static char *test_nested_object() {
    Json p = person();
    Query q = query(p, "/address/zipcode");
    mu_assert_true("test_nested_object should be right", q.tag == q_Right);
    mu_assert_true("test_nested_object should be a string", q.value.type == j_string);
    mu_assert_strings_equals("test_nested_object should be 12345", q.value.string, "12345");
    clean_json(p);
    return EXIT_SUCCESS;
}