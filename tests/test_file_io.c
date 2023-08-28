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

void teardown_match_line() {
    pcre_free(re);
    pcre_free_study(extras);
}

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
"alias build=\"meson compile -C ~/projects/acronym/builds\"\n"
"alias run='~/projects/acronym/builds/acronym' ## acronym\n"
"alias test='CK_FORK=yes ~/projects/acronym/builds/tests'\n"
"alias debug=\"gdb -q ~/projects/acronym/builds/tests\" ##bug\n"
"\n"
"CK_RUN_SUITE=\"Hash Table\"\n"
"CK_FORK=no\n"
"# --- Aliases ---\n"
"DEBUG_EXECUTABLE=~/projects/acronym/builds/tests\n", f);
    rewind(f);

    FILE *tmp = read_aliases(f, ht);
    
    Entry *e = ht->backing_array[hash_alias("build", capacity)];
    ck_assert_str_eq(e->alias, "build");
    ck_assert_str_eq(e->section, "");
    ck_assert_str_eq(e->command, "meson compile -C ~/projects/acronym/builds");

    e = ht->backing_array[hash_alias("run", capacity)];
    ck_assert_str_eq(e->alias, "run");
    ck_assert_str_eq(e->section, "acronym");
    ck_assert_str_eq(e->command, "~/projects/acronym/builds/acronym");

    e = ht->backing_array[hash_alias("test", capacity)];
    ck_assert_str_eq(e->alias, "test");
    ck_assert_str_eq(e->section, "");
    ck_assert_str_eq(e->command, "CK_FORK=yes ~/projects/acronym/builds/tests");

    e = ht->backing_array[hash_alias("debug", capacity)];
    ck_assert_str_eq(e->alias, "debug");
    ck_assert_str_eq(e->section, "bug");
    ck_assert_str_eq(e->command, "gdb -q ~/projects/acronym/builds/tests");

    char line[512];
    fgets(line, sizeof(line), tmp);
    ck_assert_str_eq(line, "CK_RUN_SUITE=\"Hash Table\"\n");
    fgets(line, sizeof(line), tmp);
    ck_assert_str_eq(line, "CK_FORK=no\n");
    fgets(line, sizeof(line), tmp);
    // Skip the --- ALIAS --- delimiter
    ck_assert_str_eq(line, "DEBUG_EXECUTABLE=~/projects/acronym/builds/tests\n");
    ck_assert(!fgets(line, sizeof(line), tmp));
}
END_TEST

START_TEST(test_write_aliases) {
    HashTable *ht;
    int capacity = 71;
    create_hash_table(&ht, capacity, .5);

    Entry *entries[4];
    create_entry(&entries[0], "git diff", 0, 0, 0);
    create_entry(&entries[1], "ls -al", 0, 0, 0);
    create_entry(&entries[2], "git push -u origin", 0, 0, 0);
    create_entry(&entries[3], "git status", 0, 0, 0);
    for (int i = 0; i < 4; i++)
        add_entry(entries[i], ht);

    FILE *f = fopen("acronym_test_tmpfile", "w");
    fputs("CK_FORK=no\n", f);

    write_aliases(f, ht);
    rewind(f);

    char line[512];
    fgets(line, sizeof(line), f);
    ck_assert_str_eq(line, "CK_FORK=no\n");
    fgets(line, sizeof(line), f);
    ck_assert_str_eq(line, "\n");
    fgets(line, sizeof(line), f);
    ck_assert_str_eq(line, FILE_DELIMITER);
    fgets(line, sizeof(line), f);
    ck_assert_str_eq(line, "alias gd=\"git diff\" ## git\n");
    fgets(line, sizeof(line), f);
    ck_assert_str_eq(line, "alias l=\"ls -al\" ## ls\n");
    fgets(line, sizeof(line), f);
    ck_assert_str_eq(line, "alias gp=\"git push -u origin\" ## git\n");
    fgets(line, sizeof(line), f);
    ck_assert_str_eq(line, "alias gs=\"git status\" ## git\n");
    ck_assert(!fgets(line, sizeof(line), f));
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
