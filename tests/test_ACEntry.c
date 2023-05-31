#include <check.h>
#include <stdlib.h>
#include "../src/ACEntry.h"
#include "test.h"

START_TEST(test_create_alias_name) {
    char *command = "grep --color=auto";
    char *alias = create_alias_name(command, false);
    ck_assert_str_eq(alias, "g");
    free(alias);
}
END_TEST

START_TEST(test_create_alias_name_include_flags) {
    char *command = "rm -rf";
    char *alias = create_alias_name(command, true);
    ck_assert_str_eq(alias, "rrf");
    free(alias);
}
END_TEST

START_TEST(test_create_section_name) {
    char *command = "sudo jupyter notebook .";
    char *section = create_section_name(command);
    ck_assert_str_eq(section, "jupyter");
    free(section);
}
END_TEST

START_TEST(test_hash_alias) {
    char *alias1 = "gs";
    char *alias2 = "gst";
    int capacity = 7;
    
    int hash1 = hash_alias(alias1, capacity);
    int hash2 = hash_alias(alias2, capacity);

    ck_assert_int_ge(hash1, 0);
    ck_assert_int_lt(hash1, capacity);
    ck_assert_int_ge(hash2, 0);
    ck_assert_int_lt(hash2, capacity);

    ck_assert_int_ne(hash1, hash2);
}
END_TEST

START_TEST(test_create_ACEntry_default1) {
    ACEntry *entry;
    char *command = "git branch --all";
    Status s = create_ACEntry(&entry, command, NULL, NULL, false);

    ck_assert(s == SUCCESS);
    ck_assert_str_eq(entry->alias, "gb");
    ck_assert_str_eq(entry->command, "git branch --all");
    ck_assert_str_eq(entry->section, "git");
    ck_assert(!entry->is_removed);
    free_ACEntry(entry);
}
END_TEST

START_TEST(test_create_ACEntry_default2) {
    ACEntry *entry;
    char *command = "meson compile -C ~/projects/alias/builds";
    Status s = create_ACEntry(&entry, command, NULL, NULL, false);

    ck_assert(s == SUCCESS);
    ck_assert_str_eq(entry->alias, "mc");
    ck_assert_str_eq(entry->command, "meson compile -C ~/projects/alias/builds");
    ck_assert_str_eq(entry->section, "meson");
    ck_assert(!entry->is_removed);
    free_ACEntry(entry);
}
END_TEST

START_TEST(test_create_ACEntry_include_flags) {
    ACEntry *entry;
    char *command = "sudo pacman -Syu";
    Status s = create_ACEntry(&entry, command, NULL, NULL, true);

    ck_assert(s == SUCCESS);
    ck_assert_str_eq(entry->alias, "pSyu");
    ck_assert_str_eq(entry->command, "sudo pacman -Syu");
    ck_assert_str_eq(entry->section, "pacman");
    ck_assert(!entry->is_removed);
    free_ACEntry(entry);
}
END_TEST

START_TEST(test_create_ACEntry_alias_override) {
    ACEntry *entry;
    char *command = "ls -al";
    char *alias_override = "ll";
    Status s = create_ACEntry(&entry, command, alias_override, NULL, true);

    ck_assert(s == SUCCESS);
    ck_assert_str_eq(entry->alias, "ll");
    ck_assert_str_eq(entry->command, "ls -al");
    ck_assert_str_eq(entry->section, "ls");
    ck_assert(!entry->is_removed);
    free_ACEntry(entry);
}
END_TEST

START_TEST(test_create_ACEntry_section_override_include_flags) {
    ACEntry *entry;
    char *command = "npm uninstall -g";
    char *section_override = "package_managers";
    Status s = create_ACEntry(&entry, command, NULL, section_override, true);

    ck_assert(s == SUCCESS);
    ck_assert_str_eq(entry->alias, "nug");
    ck_assert_str_eq(entry->command, "npm uninstall -g");
    ck_assert_str_eq(entry->section, "package_managers");
    ck_assert(!entry->is_removed);
    free_ACEntry(entry);
}
END_TEST

START_TEST(test_create_ACEntry_all_options) {
    ACEntry *entry;
    char *command = "cd ..";
    char *alias_override = "...";
    char *section_override = "etc";
    Status s = create_ACEntry(&entry, command, alias_override, section_override, false);

    ck_assert(s == SUCCESS);
    ck_assert_str_eq(entry->alias, "...");
    ck_assert_str_eq(entry->command, "cd ..");
    ck_assert_str_eq(entry->section, "etc");
    ck_assert(!entry->is_removed);
    free_ACEntry(entry);
}
END_TEST

Suite *ACEntry_suite(void) {
    Suite *s = suite_create("ACEntry");

    TCase *tc_helpers = tcase_create("AC helpers");
    tcase_add_test(tc_helpers, test_create_alias);
    tcase_add_test(tc_helpers, test_create_alias_include_flags);
    tcase_add_test(tc_helpers, test_create_section_name);
    tcase_add_test(tc_helpers, test_hash_alias);
    suite_add_tcase(s, tc_helpers);

    TCase *tc_AC = tcase_create("Create AC");
    tcase_add_test(tc_AC, test_create_ACEntry_default1);
    tcase_add_test(tc_AC, test_create_ACEntry_default2);
    tcase_add_test(tc_AC, test_create_ACEntry_include_flags);
    tcase_add_test(tc_AC, test_create_ACEntry_alias_override);
    tcase_add_test(tc_AC, test_create_ACEntry_section_override_include_flags);
    tcase_add_test(tc_AC, test_create_ACEntry_all_options);
    suite_add_tcase(s, tc_AC);

    return s;
}
