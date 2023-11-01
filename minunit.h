#ifndef C_JSON_PARSER_MINUNIT_H
#define C_JSON_PARSER_MINUNIT_H

// https://jera.com/techinfo/jtns/jtn002

#define mu_assert(message, test) do { if (!(test)) return message; } while (0)
#define mu_assert_null(test_name, ptr) mu_assert("At ["test_name"] string should be NULL", ptr == NULL)
#define mu_assert_not_null(test_name, ptr) mu_assert("At ["test_name"] pointer should NOT be NULL", ptr != NULL)
#define mu_assert_strings_equals(test_name, str1, str2) mu_assert("At ["test_name"] strings are not equals.", strcmp(str1,str2) == 0)
#define mu_assert_ints_equals(test_name, n1, n2) mu_assert("At ["test_name"] ints are not equals.", n1 == n2)
#define mu_assert_chars_equals(test_name, c1, c2) mu_assert("["test_name"] chars are not equals.", c1 == c2)
#define mu_assert_true(test_name, bool) mu_assert("["test_name"] is not TRUE.", bool == true)
#define mu_assert_false(test_name, bool) mu_assert("["test_name"] is not FALSE.", bool == false)
#define mu_run_test(test) do { char *message = test(); tests_run++; \
                                if (message) return message; } while (0)
extern int tests_run;

#endif //C_JSON_PARSER_MINUNIT_H
