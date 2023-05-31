#include <stdlib.h>
#include <check.h>
#include "../src/parse_args.h"
#include "test.h"

START_TEST(test_parse_args) {
    ck_assert(0);
}
END_TEST

Suite *parse_args(void) {
    Suite *s = suite_create("parse_args");

    TCase *tc_add = tcase_create("Add");
    tcase_add_test(tc_add, test_parse_args);
    suite_add_tcase(s, tc_add);

    TCase *tc_remove = tcase_create("Remove");
    suite_add_tcase(s, tc_remove);

    TCase *tc_edit = tcase_create("Edit");
    suite_add_tcase(s, tc_edit);

    return s;
}
