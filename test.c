#include <string.h>
#include <stdlib.h>
#include <printf.h>

#include "library.h"
#include "minunit.h"

static char *test_parse_null();

static char *test_get_string_between_quotes();

static char *test_get_first_key();

static char *test_parse_key_value();

static char *test_get_type_of_next_value();

static char *test_get_next_string_value_in_string();

static char *test_get_next_number_value_in_string();

static char *test_get_next_object_value_in_string();

static char *test_get_next_array_value_in_string();

static char *test_parse_simple_json();

static char *test_is_white_space();

static char *test_expect_next_value();

static char * test_push_key_value_pair_in_json();

static char *test_nested_json();

int tests_run = 0;

static char *all_tests() {
    mu_run_test(test_parse_null);
    mu_run_test(test_get_string_between_quotes);
    mu_run_test(test_get_first_key);
    mu_run_test(test_parse_key_value);
    mu_run_test(test_get_type_of_next_value);
    mu_run_test(test_get_next_string_value_in_string);
    mu_run_test(test_get_next_number_value_in_string);
    mu_run_test(test_get_next_object_value_in_string);
    mu_run_test(test_parse_simple_json);
    //mu_run_test(test_get_next_array_value_in_string);
    // TODO test_get_next array, null, bool
    mu_run_test(test_is_white_space);
    mu_run_test(test_expect_next_value);
    mu_run_test(test_push_key_value_pair_in_json);
    //mu_run_test(test_nested_json);
    return EXIT_SUCCESS;
}

int main(int argc, char **argv) {
    char *result = all_tests();
    if (result != EXIT_SUCCESS) {
        printf("%s\n", result);
    } else {
        printf("ALL TESTS PASSED\n");
    }
    printf("Tests run: %d\n", tests_run);

    return result != EXIT_SUCCESS;
}

static char *test_parse_null() {
    Json *json_obj = parse_json(NULL);
    mu_assert("error, the json_obj should be null", json_obj == NULL);
    clean_json(json_obj);
    return EXIT_SUCCESS;
}

static char *test_get_string_between_quotes() {
    const char *str = " \"a key\" : \"then a string\"";
    Parsed key = get_first_string_between_double_quote(str);
    mu_assert("test_get_string_between_quotes", key.type == 's');
    mu_assert_not_null("test_get_string_between_quotes", key.string);
    mu_assert_strings_equals("test_get_string_between_quotes", key.string, "a key");
    free(key.string);
    Parsed key_two = get_first_string_between_double_quote("\"Paul\", \"age\":20, \"city\":\"Paris\"}");
    mu_assert("test_get_string_between_quotes", key_two.type == 's');
    mu_assert_not_null("test_get_string_between_quotes", key_two.string);
    mu_assert_strings_equals("test_get_string_between_quotes, string", key_two.string, "Paul");
    free(key_two.string);

    mu_assert_null("test_get_string_between_quotes, empty key", get_first_string_between_double_quote("\"\"").string);
    mu_assert_null("test_get_string_between_quotes, just one quote",
                   get_first_string_between_double_quote("juste\"one quote").string);
    mu_assert_null("test_get_string_between_quotes, no quote", get_first_string_between_double_quote("no key").string);

    return EXIT_SUCCESS;
}

static char *test_get_first_key() {

    Json *json_obj = parse_json("{\"name\":\"Paul\", \"age\":20, \"city\":\"Paris\" }");

    mu_assert_not_null("test_get_first_key, not null", json_obj);
    mu_assert_strings_equals("test_get_first_key", json_obj->keys[0], "name");

    free_json(json_obj);
    return EXIT_SUCCESS;
}

static char *test_parse_key_value() {
    KeyValuePairParsed parsed = parse_key_value_pair("{\"name\":\"Paul\", \"age\":20, \"city\":\"Paris\"}");
    mu_assert_strings_equals("test_parse_key_value, key", "name", parsed.key);
    mu_assert("test_parse_key_value, type", parsed.value.type == 's');
    mu_assert_strings_equals("test_parse_key_value, string", parsed.value.string, "Paul");
    mu_assert_strings_equals("test_parse_key_value, start", parsed.start,
                             "\"name\":\"Paul\", \"age\":20, \"city\":\"Paris\"}");
    mu_assert_strings_equals("test_parse_key_value, start", parsed.end, ", \"age\":20, \"city\":\"Paris\"}");

    free(parsed.key);
    return EXIT_SUCCESS;
}

static char *test_get_type_of_next_value() {
    mu_assert("test_get_type_of_next_value, string", get_type_of_next_value(": \"string\" ") == 's');
    mu_assert("test_get_type_of_next_value, number", get_type_of_next_value(" 20") == 'n');
    mu_assert("test_get_type_of_next_value, negative number", get_type_of_next_value(" -20") == 'n');
    mu_assert("test_get_type_of_next_value, object", get_type_of_next_value("  : { \"k\" : \"v\"}") == 'o');
    mu_assert("test_get_type_of_next_value, array", get_type_of_next_value("  : [1,2,3], ") == 'a');
    mu_assert("test_get_type_of_next_value, null", get_type_of_next_value(" null ") == '\0');
    mu_assert("test_get_type_of_next_value, error", get_type_of_next_value(" abc ") == 'x');

    return EXIT_SUCCESS;
}

static char *test_get_next_string_value_in_string() {
    NextValueInString string = get_next_value_in_string(":\"Paul\",");
    mu_assert("test_get_next_string_value_in_string, string", string.json.type == 's');
    mu_assert_strings_equals("test_get_next_string_value_in_string, string", string.json.string, "Paul");
    clean_json(&string.json);

    return EXIT_SUCCESS;
}

static char *test_get_next_number_value_in_string() {
    NextValueInString number = get_next_value_in_string(": 20,");
    mu_assert("test_get_next_string_value_in_string, number", number.json.type == 'n');
    mu_assert_ints_equals("test_get_next_string_value_in_string, number", number.json.number, 20);
    clean_json(&number.json);

    return EXIT_SUCCESS;
}

static char *test_get_next_object_value_in_string() {
    NextValueInString obj = get_next_value_in_string(": {\"k\":\"v\"},");
    mu_assert("test_get_next_string_value_in_string, obj", obj.json.type == 'o');
    mu_assert_ints_equals("test_get_next_string_value_in_string, obj", obj.json.nb_elements, 1);
    mu_assert_strings_equals("test_get_next_string_value_in_string, obj", obj.json.keys[0], "k");
    mu_assert_strings_equals("test_get_next_string_value_in_string, obj", obj.json.values[0].string, "v");
    clean_json(&obj.json);
    return EXIT_SUCCESS;
}


static char *test_parse_simple_json() {
    const char *json_str = "{\"name\":\"Paul\", \"age\":20, \"city\":\"Paris\"}";
    Json *json_obj = parse_json(json_str);

    mu_assert("error, the json_obj should be valid ", json_obj != NULL);
    mu_assert("error, json_obj->type != 'o'", json_obj->type == 'o');
    mu_assert("error, json_obj->nb_elements != 3", json_obj->nb_elements == 3);
    mu_assert("error, key[0] != name", strcmp(json_obj->keys[0], "name") == 0);
    mu_assert("error, string[0] != Paul", strcmp(json_obj->values[0].string, "Paul") == 0);
    mu_assert("error, key[1] != age", strcmp(json_obj->keys[1], "age") == 0);
    mu_assert("error, string[1] != 20", json_obj->values[1].number == 20);
    mu_assert("error, key[2] != city", strcmp(json_obj->keys[2], "city") == 0);
    mu_assert("error, string[2] != Paris", strcmp(json_obj->values[2].string, "Paris") == 0);

    free_json(json_obj);
    return EXIT_SUCCESS;
}

static char *test_get_next_array_value_in_string() {

    NextValueInString array = get_next_value_in_string(": [1, \"two\"],");
    mu_assert("test_get_next_string_value_in_string, array", array.json.type == 'a');
    mu_assert_null("test_get_next_string_value_in_string, array keys", array.json.keys);
    mu_assert_ints_equals("test_get_next_string_value_in_string, array", array.json.nb_elements, 2);
    mu_assert_ints_equals("test_get_next_string_value_in_string, array", array.json.values[0].number, 1);
    mu_assert_strings_equals("test_get_next_string_value_in_string, array", array.json.values[1].string, "two");
    clean_json(&array.json);

    return EXIT_SUCCESS;
}

static char *test_is_white_space() {
    mu_assert_true("test_is_white_space: space", is_white_space(' '));
    mu_assert_true("test_is_white_space: \\n", is_white_space('\n'));
    mu_assert_true("test_is_white_space: \\t", is_white_space('\t'));
    mu_assert_true("test_is_white_space: \\r", is_white_space('\r'));

    mu_assert_false("test_is_white_space: A", is_white_space('A'));
    return EXIT_SUCCESS;
}

static char *test_expect_next_value() {
    mu_assert_true("test_expect_next_value", expect_next_value(", \"next\":\"v\""));
    mu_assert_true("test_expect_next_value", expect_next_value("   , "));

    mu_assert_false("test_expect_next_value", expect_next_value("   "));
    mu_assert_false("test_expect_next_value", expect_next_value(""));
    mu_assert_false("test_expect_next_value", expect_next_value("}"));
    mu_assert_false("test_expect_next_value", expect_next_value(" ]"));
    mu_assert_false("test_expect_next_value", expect_next_value("\"next\":\"but no comma\""));
    mu_assert_false("test_expect_next_value", expect_next_value(" X  , "));
    return EXIT_SUCCESS;
}


static char * test_push_key_value_pair_in_json() {
    Json json = empty_json_object();
    push_key_value_pair_in_json("first key", json_string("first string"), &json);
    mu_assert_ints_equals("test_push_key_value_pair_in_json : first nb_elements", json.nb_elements, 1);
    mu_assert_strings_equals("test_push_key_value_pair_in_json : first key", json.keys[0], "first key");
    mu_assert_strings_equals("test_push_key_value_pair_in_json : first string", json.values[0].string, "first string");

    push_key_value_pair_in_json("second key", json_string("second string"), &json);
    mu_assert_ints_equals("test_push_key_value_pair_in_json : second nb_elements", json.nb_elements, 2);
    mu_assert_strings_equals("test_push_key_value_pair_in_json : second key", json.keys[1], "second key");
    mu_assert_strings_equals("test_push_key_value_pair_in_json : second string", json.values[1].string, "second string");
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
    mu_assert("error, string[0] != Paul", strcmp(json_obj->values[0].string, "Paul") == 0);
    mu_assert("error, key[1] != age", strcmp(json_obj->keys[1], "age") == 0);
    mu_assert("error, string[1] != 20", json_obj->values[1].number == 20);
    mu_assert("error, key[2] != city", strcmp(json_obj->keys[2], "city") == 0);
    mu_assert("error, string[2] != Paris", strcmp(json_obj->values[2].string, "Paris") == 0);

    // array
    mu_assert("error, key[3] != friends", strcmp(json_obj->keys[3], "friends") == 0);
    mu_assert("error, string[3].type != 'a'", json_obj->values[3].type == 'a');
    mu_assert("error, string[3].nb_elements != 2", json_obj->values[3].nb_elements == 3);
    mu_assert("error, string[3].values[0] != Anaelle", strcmp(json_obj->values[3].values[0].string, "Anaelle") == 0);
    mu_assert("error, string[3].values[1] != Noam", strcmp(json_obj->values[3].values[1].string, "Noam") == 0);
    mu_assert("error, string[3].values[2] != Tom", strcmp(json_obj->values[3].values[2].string, "Tom") == 0);

    // nested
    mu_assert("error, key[4] != address", strcmp(json_obj->keys[4], "address") == 0);
    mu_assert("error, string[4].type != 'o'", json_obj->values[4].type == 'o');
    mu_assert("error, string[4].nb_elements != 2", json_obj->values[4].nb_elements == 2);
    mu_assert("error, string[4].keys[0] != street", strcmp(json_obj->values[4].keys[0], "street") == 0);
    mu_assert("error, string[4].values[0] != 1 rue du Pain",
              strcmp(json_obj->values[4].values[0].string, "1 rue du Pain") == 0);
    mu_assert("error, string[4].keys[1] != zipcode", strcmp(json_obj->values[4].keys[1], "zipcode") == 0);
    mu_assert("error, string[4].values[1] != 12345", strcmp(json_obj->values[4].values[1].string, "12345") == 0);

    free_json(json_obj);
    return EXIT_SUCCESS;
}