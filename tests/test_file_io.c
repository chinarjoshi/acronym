#include <check.h>
#include <pcre.h>
#include "../src/file_io.h"

pcre *re;
pcre_extra *extras;
char alias[64], command[256], section[64];
int ovector[8];
void setup_match_line() {
    re = pcre_compile(ALIAS_PATTERN, 0, 0, 0, 0);
    extras = pcre_study(re, 0, 0);
    memset(alias, 0, sizeof(alias));
    memset(command, 0, sizeof(command));
    memset(section, 0, sizeof(section));
    memset(ovector, 0, sizeof(ovector));
}

void teardown_match_line() {}

START_TEST(test_match_line_basic) {
    char *line = "alias jup=jupyter";
    bool result = match_line(re, extras, ovector, line, alias, command, section);
    ck_assert(result);
    ck_assert_str_eq(alias, "jup");
    ck_assert_str_eq(section, "");
    ck_assert_str_eq(command, "jupyter");
}
END_TEST

START_TEST(test_match_line_mismatch) {
    char *line = "alias jup = jupyter # code";
    bool result = match_line(re, extras, ovector, line, alias, command, section);
    ck_assert(!result);
}
END_TEST

START_TEST(test_match_line_single_quotes) {
    char *line = "alias ll='ls -alf'";
    bool result = match_line(re, extras, ovector, line, alias, command, section);
    ck_assert(result);
    ck_assert_str_eq(alias, "ll");
    ck_assert_str_eq(section, "");
    ck_assert_str_eq(command, "ls -alf");
}
END_TEST

START_TEST(test_match_line_double_quotes) {
    char *line = "alias ga=\"git add -A\"";
    bool result = match_line(re, extras, ovector, line, alias, command, section);
    ck_assert(result);
    ck_assert_str_eq(alias, "ga");
    ck_assert_str_eq(section, "");
    ck_assert_str_eq(command, "git add -A");
}
END_TEST

START_TEST(test_match_line_with_section) {
    char *line = "alias l=ls ## etc ";
    bool result = match_line(re, extras, ovector, line, alias, command, section);
    ck_assert(result);
    ck_assert_str_eq(alias, "l");
    ck_assert_str_eq(section, "etc");
    ck_assert_str_eq(command, "ls");
}
END_TEST

START_TEST(test_match_line_single_quotes_section) {
    char *line = "alias gd='git diff HEAD~' ##git";
    bool result = match_line(re, extras, ovector, line, alias, command, section);
    ck_assert(result);
    ck_assert_str_eq(alias, "gd");
    ck_assert_str_eq(section, "git");
    ck_assert_str_eq(command, "git diff HEAD~");
}
END_TEST

START_TEST(test_read_aliases) {
    HashTable *ht;
    int capacity = 71;
    create_hash_table(&ht, capacity, .5);

    FILE *f = fopen("acronym_test_tmpfile", "w");
    fputs(
"alias build=\"meson compile -C ~/projects/acronym/builds\""
"alias run='~/projects/acronym/builds/acronym' ## acronym"
"alias test='CK_FORK=yes ~/projects/acronym/builds/tests'"
"alias debug=\"gdb -q ~/projects/acronym/builds/tests\" ##bug"
"\n"
"CK_RUN_SUITE=\"Hash Table\""
"CK_FORK=no"
"DEBUG_EXECUTABLE=~/projects/acronym/builds/tests", f);
    rewind(f);
}
END_TEST

START_TEST(test_write_aliases) {
}
END_TEST

Suite *file_io_suite(void) {
    Suite *s = suite_create("File IO");

    TCase *tc_match = tcase_create("Match");
    tcase_add_test(tc_match, test_match_line_basic);
    tcase_add_test(tc_match, test_match_line_mismatch);
    tcase_add_test(tc_match, test_match_line_single_quotes);
    tcase_add_test(tc_match, test_match_line_double_quotes);
    tcase_add_test(tc_match, test_match_line_with_section);
    tcase_add_test(tc_match, test_match_line_single_quotes_section);
    tcase_add_checked_fixture(tc_match, setup_match_line, teardown_match_line);
    suite_add_tcase(s, tc_match);

    TCase *tc_read = tcase_create("Read");
    tcase_add_test(tc_read, test_read_aliases);
    suite_add_tcase(s, tc_read);

    TCase *tc_write = tcase_create("Write");
    tcase_add_test(tc_write, test_write_aliases);
    suite_add_tcase(s, tc_write);

    return s;
}
