#include <string.h>
#include <stdlib.h>
#include <printf.h>

#include "query.h"
#include "minunit.h"

static char *test_parse_null();

int tests_run = 0;

static char *all_tests() {
    mu_run_test(test_parse_null);
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

static char *test_parse_null() {
    return EXIT_SUCCESS;
}
