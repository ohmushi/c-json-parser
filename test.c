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

static char *test_push_key_value_pair_in_json();

static char *test_push_value_in_json();

static char *test_parse_array();

static char *test_parse_array_with_nested_object();

static char *test_parse_object_with_nested_array();

static char *test_nested_json();

static char *test_parse_root_array();

static char *test_get_next_null_value_in_string();

static char *test_get_next_value_in_string_when_null();

static char *test_get_next_boolean_value_in_string();

static char *test_get_next_value_in_string_when_boolean();

static char *test_reject_parse_key_value_if_double_dot_not_present();

static char * test_parse_empty_object();

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
    mu_run_test(test_get_next_array_value_in_string);
    mu_run_test(test_is_white_space);
    mu_run_test(test_expect_next_value);
    mu_run_test(test_push_key_value_pair_in_json);
    mu_run_test(test_push_value_in_json);
    mu_run_test(test_parse_array);
    mu_run_test(test_parse_array_with_nested_object);
    mu_run_test(test_parse_object_with_nested_array);
    mu_run_test(test_nested_json);
    mu_run_test(test_parse_root_array);
    mu_run_test(test_get_next_null_value_in_string);
    mu_run_test(test_get_next_value_in_string_when_null);
    mu_run_test(test_get_next_boolean_value_in_string);
    mu_run_test(test_get_next_value_in_string_when_boolean);
    mu_run_test(test_reject_parse_key_value_if_double_dot_not_present);
    mu_run_test(test_parse_empty_object);
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
    mu_assert("test_get_string_between_quotes", key.type == j_string);
    mu_assert_not_null("test_get_string_between_quotes", key.string);
    mu_assert_strings_equals("test_get_string_between_quotes", key.string, "a key");
    free(key.string);
    Parsed key_two = get_first_string_between_double_quote("\"Paul\", \"age\":20, \"city\":\"Paris\"}");
    mu_assert("test_get_string_between_quotes", key_two.type == j_string);
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
    Parsed parsed = parse_key_value_pair("{\"name\":\"Paul\", \"age\":20, \"city\":\"Paris\"}");
    mu_assert("test_parse_key_value, type", parsed.type == j_key_value_pair_p);
    mu_assert_strings_equals("test_parse_key_value, key", "name", parsed.key_value_pair.key);
    mu_assert("test_parse_key_value, type", parsed.key_value_pair.value.type == j_string_p);
    mu_assert_strings_equals("test_parse_key_value, string", parsed.key_value_pair.value.string, "Paul");
    mu_assert_strings_equals("test_parse_key_value, start", parsed.start,
                             "\"name\":\"Paul\", \"age\":20, \"city\":\"Paris\"}");
    mu_assert_strings_equals("test_parse_key_value, start", parsed.end, ", \"age\":20, \"city\":\"Paris\"}");

    free(parsed.key_value_pair.key);
    return EXIT_SUCCESS;
}

static char *test_get_type_of_next_value() {
    mu_assert("test_get_type_of_next_value, string", get_type_of_next_value(": \"string\" ") == j_string);
    mu_assert("test_get_type_of_next_value, number", get_type_of_next_value(" 20") == j_number);
    mu_assert("test_get_type_of_next_value, negative number", get_type_of_next_value(" -20") == j_number);
    mu_assert("test_get_type_of_next_value, node", get_type_of_next_value("  : { \"k\" : \"v\"}") == j_object);
    mu_assert("test_get_type_of_next_value, array", get_type_of_next_value("  : [1,2,3], ") == j_array);
    mu_assert("test_get_type_of_next_value, null", get_type_of_next_value(" null ") == j_null);
    mu_assert("test_get_type_of_next_value, error", get_type_of_next_value(" abc ") == 'x');

    return EXIT_SUCCESS;
}

static char *test_get_next_string_value_in_string() {
    NextValueInString string = get_next_value_in_string(":\"Paul\",");
    mu_assert("test_get_next_string_value_in_string, string", string.json.type == j_string);
    mu_assert_strings_equals("test_get_next_string_value_in_string, string", string.json.string, "Paul");
    clean_json(&string.json);

    return EXIT_SUCCESS;
}

static char *test_get_next_number_value_in_string() {
    NextValueInString number = get_next_value_in_string(": 20,");
    mu_assert("test_get_next_string_value_in_string, number", number.json.type == j_number);
    mu_assert_ints_equals("test_get_next_string_value_in_string, number", number.json.number, 20);
    clean_json(&number.json);

    return EXIT_SUCCESS;
}

static char *test_get_next_object_value_in_string() {
    NextValueInString obj = get_next_value_in_string(": {\"k\":\"v\"},");
    mu_assert("test_get_next_string_value_in_string, obj", obj.json.type == j_object);
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
    mu_assert("error, json_obj->type != j_object", json_obj->type == j_object);
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
    mu_assert("test_get_next_string_value_in_string, array", array.json.type == j_array);
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

    mu_assert_false("test_is_white_space: A", is_white_space(j_array));
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


static char *test_push_key_value_pair_in_json() {
    Json json = empty_json_object();
    push_key_value_pair_in_object("first key", json_string("first string"), &json);
    mu_assert_ints_equals("test_push_key_value_pair_in_json : first nb_elements", json.nb_elements, 1);
    mu_assert_strings_equals("test_push_key_value_pair_in_json : first key", json.keys[0], "first key");
    mu_assert_strings_equals("test_push_key_value_pair_in_json : first string", json.values[0].string, "first string");

    push_key_value_pair_in_object("second key", json_string("second string"), &json);
    mu_assert_ints_equals("test_push_key_value_pair_in_json : second nb_elements", json.nb_elements, 2);
    mu_assert_strings_equals("test_push_key_value_pair_in_json : second key", json.keys[1], "second key");
    mu_assert_strings_equals("test_push_key_value_pair_in_json : second string", json.values[1].string,
                             "second string");
    return EXIT_SUCCESS;
}

static char *test_push_value_in_json() {
    Json not_an_array = empty_json_object();
    push_value_in_array(json_string("not an array"), &not_an_array);
    mu_assert_ints_equals("test_push_value_in_json, not an array", not_an_array.nb_elements, 0);
    mu_assert_null("test_push_value_in_json : not an array keys", not_an_array.keys);
    mu_assert_null("test_push_value_in_json : not an array values", not_an_array.values);

    Json array = empty_json_array();
    push_value_in_array(json_string("first string"), &array);
    mu_assert_ints_equals("test_push_value_in_json : first nb_elements", array.nb_elements, 1);
    mu_assert_null("test_push_value_in_json : no keys", array.keys);
    mu_assert_chars_equals("test_push_value_in_json : type", array.values[0].type, j_string);
    mu_assert_strings_equals("test_push_value_in_json : first string", array.values[0].string, "first string");

    push_value_in_array(json_string("second string"), &array);
    mu_assert_ints_equals("test_push_value_in_json : second nb_elements", array.nb_elements, 2);
    mu_assert_null("test_push_value_in_json : no keys", array.keys);
    mu_assert_chars_equals("test_push_value_in_json : type", array.values[1].type, j_string);
    mu_assert_strings_equals("test_push_value_in_json : second string", array.values[1].string, "second string");

    return EXIT_SUCCESS;
}

static char *test_parse_array() {
    Parsed array_number = parse_json_array("[1,\"2\",3]");
    mu_assert_chars_equals("test_parse_array, type", array_number.type, j_array);
    mu_assert_ints_equals("test_parse_array, nb_elements", array_number.node.nb_elements, 3);
    mu_assert_ints_equals("test_parse_array, number", array_number.node.values[0].number, 1);
    mu_assert_strings_equals("test_parse_array, number", array_number.node.values[1].string, "2");
    mu_assert_ints_equals("test_parse_array, number", array_number.node.values[2].number, 3);
    clean_json(&array_number.node);

    return EXIT_SUCCESS;
}

static char *test_parse_array_with_nested_object() {
    Parsed array = parse_json_array("[1, {\"k\" : 2} ]");
    mu_assert_chars_equals("test_parse_array_with_nested_object, type", array.type, j_array);
    mu_assert_ints_equals("test_parse_array_with_nested_object, nb_elements", array.node.nb_elements, 2);
    mu_assert_ints_equals("test_parse_array_with_nested_object, number", array.node.values[0].number, 1);

    // object
    mu_assert_chars_equals("test_parse_array_with_nested_object, object", array.node.values[1].type, j_object);
    mu_assert_ints_equals("test_parse_array_with_nested_object, object nb_elements", array.node.values[1].nb_elements,
                          1);
    mu_assert_strings_equals("test_parse_array_with_nested_object, object key", array.node.values[1].keys[0], "k");
    mu_assert_ints_equals("test_parse_array_with_nested_object, object value", array.node.values[1].values[0].number,
                          2);

    return EXIT_SUCCESS;
}

static char *test_parse_object_with_nested_array() {
    Parsed obj = parse_json_object("{\"array\":[1,2]}");

    mu_assert_ints_equals("test_parse_object_with_nested_array, nb_elements", obj.node.nb_elements, 1);
    mu_assert_strings_equals("test_parse_object_with_nested_array, key", obj.node.keys[0], "array");

    // array
    mu_assert_chars_equals("test_parse_array_with_nested_object, type", obj.node.values[0].type, j_array);
    mu_assert_ints_equals("test_parse_array_with_nested_object, nb_elements", obj.node.values[0].nb_elements, 2);
    mu_assert_ints_equals("test_parse_array_with_nested_object, array[0]", obj.node.values[0].values[0].number, 1);
    mu_assert_ints_equals("test_parse_array_with_nested_object, array[0]", obj.node.values[0].values[1].number, 2);


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
    mu_assert("error, json_obj->type != j_object", json_obj->type == j_object);
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
    mu_assert("error, string[3].type != j_array", json_obj->values[3].type == j_array);
    mu_assert("error, string[3].nb_elements != 2", json_obj->values[3].nb_elements == 3);
    mu_assert("error, string[3].values[0] != Anaelle", strcmp(json_obj->values[3].values[0].string, "Anaelle") == 0);
    mu_assert("error, string[3].values[1] != Noam", strcmp(json_obj->values[3].values[1].string, "Noam") == 0);
    mu_assert("error, string[3].values[2] != Tom", strcmp(json_obj->values[3].values[2].string, "Tom") == 0);

    // nested
    mu_assert("error, key[4] != address", strcmp(json_obj->keys[4], "address") == 0);
    mu_assert("error, string[4].type != j_object", json_obj->values[4].type == j_object);
    mu_assert("error, string[4].nb_elements != 2", json_obj->values[4].nb_elements == 2);
    mu_assert("error, string[4].keys[0] != street", strcmp(json_obj->values[4].keys[0], "street") == 0);
    mu_assert("error, string[4].values[0] != 1 rue du Pain",
              strcmp(json_obj->values[4].values[0].string, "1 rue du Pain") == 0);
    mu_assert("error, string[4].keys[1] != zipcode", strcmp(json_obj->values[4].keys[1], "zipcode") == 0);
    mu_assert("error, string[4].values[1] != 12345", strcmp(json_obj->values[4].values[1].string, "12345") == 0);

    free_json(json_obj);
    return EXIT_SUCCESS;
}

static char *test_parse_root_array() {
    const char *str = "[1, 2,3]";
    Json *json = parse_json(str);

    mu_assert_not_null("test_parse_root_array", json);
    mu_assert_chars_equals("test_parse_root_array", json->type, j_array);
    mu_assert_ints_equals("test_parse_root_array", json->nb_elements, 3);
    free_json(json);

    return EXIT_SUCCESS;
}

static char *test_get_next_null_value_in_string() {
    NextValueInString null = get_next_null_value_in_string(": null,");
    mu_assert_chars_equals("test_get_next_null_value_in_string, type", null.json.type, j_null);
    mu_assert_chars_equals("test_get_next_null_value_in_string, start", *null.start, 'n');
    mu_assert_chars_equals("test_get_next_null_value_in_string, end", *null.end, 'l');

    NextValueInString notnull = get_next_null_value_in_string(": notnull,");
    mu_assert_chars_equals("test_get_next_null_value_in_string, type", notnull.json.type, j_empty);
    return EXIT_SUCCESS;
}

static char *test_get_next_value_in_string_when_null() {
    NextValueInString null = get_next_value_in_string(": null,");
    mu_assert_chars_equals("test_get_next_value_in_string_when_null, type", null.json.type, j_null);
    mu_assert_chars_equals("test_get_next_value_in_string_when_null, start", *null.start, 'n');
    mu_assert_chars_equals("test_get_next_value_in_string_when_null, end", *null.end, 'l');

    NextValueInString notnull = get_next_value_in_string(": notnull,");
    mu_assert_chars_equals("test_get_next_value_in_string_when_null, type", notnull.json.type, j_empty);
    return EXIT_SUCCESS;
}

static char *test_get_next_boolean_value_in_string() {
    NextValueInString j_true = get_next_boolean_value_in_string(": true,");
    mu_assert_ints_equals("test_get_next_boolean_value_in_string, true type", j_true.json.type, j_boolean);
    mu_assert_chars_equals("test_get_next_boolean_value_in_string, true start", *j_true.start, 't');
    mu_assert_chars_equals("test_get_next_boolean_value_in_string, true end", *j_true.end, 'e');

    NextValueInString j_false = get_next_boolean_value_in_string(": false,");
    mu_assert_ints_equals("test_get_next_boolean_value_in_string, false type", j_false.json.type, j_boolean);
    mu_assert_chars_equals("test_get_next_boolean_value_in_string, false start", *j_false.start, 'f');
    mu_assert_chars_equals("test_get_next_boolean_value_in_string, false end", *j_false.end, 'e');

    NextValueInString notboolean = get_next_boolean_value_in_string(": notboolean,");
    mu_assert_ints_equals("test_get_next_boolean_value_in_string, notboolean type", notboolean.json.type, j_empty);
    return EXIT_SUCCESS;
}


static char *test_get_next_value_in_string_when_boolean() {
    NextValueInString j_true = get_next_value_in_string(": true,");
    mu_assert_ints_equals("test_get_next_value_in_string_when_boolean, true type", j_true.json.type, j_boolean);
    mu_assert_chars_equals("test_get_next_value_in_string_when_boolean, true start", *j_true.start, 't');
    mu_assert_chars_equals("test_get_next_value_in_string_when_boolean, true end", *j_true.end, 'e');

    NextValueInString j_false = get_next_value_in_string(": false,");
    mu_assert_ints_equals("test_get_next_value_in_string_when_boolean, false type", j_false.json.type, j_boolean);
    mu_assert_chars_equals("test_get_next_value_in_string_when_boolean, false start", *j_false.start, 'f');
    mu_assert_chars_equals("test_get_next_value_in_string_when_boolean, false end", *j_false.end, 'e');

    NextValueInString notboolean = get_next_value_in_string(": notboolean,");
    mu_assert_ints_equals("test_get_next_value_in_string_when_boolean, notboolean type", notboolean.json.type, j_empty);
    return EXIT_SUCCESS;
}


static char *test_reject_parse_key_value_if_double_dot_not_present() {
    Parsed parsed = parse_key_value_pair("{\"key\" \"value\"}");
    mu_assert("test_reject_parse_key_value_if_double_dot_not_present, type", parsed.type == j_empty_p);
    return EXIT_SUCCESS;
}

static char * test_parse_empty_object() {
    Json *empty = parse_json("{}");
    mu_assert_not_null("test_parse_empty_object", empty);
    mu_assert_ints_equals("test_parse_empty_object, type", empty->type, j_object);
    mu_assert_ints_equals("test_parse_empty_object, nb elements", empty->nb_elements, 0);
    mu_assert_null("test_parse_empty_object, keys", empty->keys);
    mu_assert_null("test_parse_empty_object, values", empty->values);
    free_json(empty);

    Json *non_empty = parse_json("{\"k\": \"v\"}");
    mu_assert_not_null("test_parse_empty_object, not empty", non_empty);
    mu_assert_ints_equals("test_parse_empty_object, not empty type", non_empty->type, j_object);
    mu_assert_ints_equals("test_parse_empty_object, not empty nb elements", non_empty->nb_elements, 1);
    free_json(non_empty);
    return EXIT_SUCCESS;
}