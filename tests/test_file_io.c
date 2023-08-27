#include <check.h>

START_TEST(test_match_line) {
}
END_TEST

START_TEST(test_read_aliases) {
}
END_TEST

Suite *file_io_suite(void) {
    Suite *s = suite_create("File IO");

    TCase *tc_match = tcase_create("Match");
    tcase_add_test(tc_match, test_match_line);
    suite_add_tcase(s, tc_match);

    TCase *tc_add = tcase_create("Read");
    suite_add_tcase(s, tc_add);

    TCase *tc_remove = tcase_create("Write");
    suite_add_tcase(s, tc_remove);

    return s;
}
