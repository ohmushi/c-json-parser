#ifndef C_JSON_PARSER_MINUNIT_H
#define C_JSON_PARSER_MINUNIT_H

// https://jera.com/techinfo/jtns/jtn002

#define mu_assert(message, test) do { if (!(test)) return message; } while (0)
#define mu_assert_null(test_name, ptr) mu_assert("At ["test_name"] value should be NULL", ptr == NULL)
#define mu_assert_not_null(test_name, ptr) mu_assert("At ["test_name"] value should NOT be NULL", ptr != NULL)
#define mu_assert_strings_equals(test_name, str1, str2) mu_assert("At ["test_name"] strings are not equals.", strcmp(str1,str2) == 0)
#define mu_assert_ints_equals(test_name, n1, n2) mu_assert("At ["test_name"] ints are not equals.", n1 == n2)
#define mu_run_test(test) do { char *message = test(); tests_run++; \
                                if (message) return message; } while (0)
extern int tests_run;

#endif //C_JSON_PARSER_MINUNIT_H
