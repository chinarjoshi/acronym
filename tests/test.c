#include <check.h>
#include <stdlib.h>
#include "test.h"

int main() {
    Suite *suite1 = entry_suite();
    Suite *suite2 = hash_table_suite();
    Suite *suite3 = parse_args_suite();

    SRunner *sr = srunner_create(suite1);
    srunner_add_suite(sr, suite2);
    srunner_add_suite(sr, suite3);

    srunner_run_all(sr, CK_NORMAL);
    int number_failed = srunner_ntests_failed(sr);
    srunner_free(sr);
    return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
