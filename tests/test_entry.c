#include <stdlib.h>
#include <check.h>
#include "../src/hash_table/entry.h"
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
    char *command = "git branch --all";
    char *section = create_section_name(command);
    ck_assert_str_eq(section, "git");
    free(section);
}
END_TEST

START_TEST(test_create_section_name_sudo) {
    char *command = "sudo jupyter notebook .";
    char *section = create_section_name(command);
    ck_assert_str_eq(section, "jupyter");
    free(section);
}
END_TEST

START_TEST(test_hash_alias) {
    char *alias1 = "gs";
    char *alias2 = "gst";
    int capacity = 137;
    
    int hash1 = hash_alias(alias1, capacity);
    int hash2 = hash_alias(alias2, capacity);

    ck_assert_int_ge(hash1, 0);
    ck_assert_int_lt(hash1, capacity);
    ck_assert_int_ge(hash2, 0);
    ck_assert_int_lt(hash2, capacity);

    ck_assert_int_ne(hash1, hash2);
}
END_TEST

START_TEST(test_create_entry_default1) {
    Entry *entry;
    char *command = "git branch --all";
    Status s = create_entry(&entry, command, NULL, NULL, NULL, 0, false);

    ck_assert(s == SUCCESS);
    ck_assert_str_eq(entry->alias, "gb");
    ck_assert_str_eq(entry->command, "git branch --all");
    ck_assert_str_eq(entry->section, "git");
    ck_assert(!entry->is_removed);
    free_entry(entry);
}
END_TEST

START_TEST(test_create_entry_default2) {
    Entry *entry;
    char *command = "meson compile -C ~/projects/alias/builds";
    Status s = create_entry(&entry, command, NULL, NULL, NULL, 0, false);

    ck_assert(s == SUCCESS);
    ck_assert_str_eq(entry->alias, "mc");
    ck_assert_str_eq(entry->command, "meson compile -C ~/projects/alias/builds");
    ck_assert_str_eq(entry->section, "meson");
    ck_assert(!entry->is_removed);
    free_entry(entry);
}
END_TEST

START_TEST(test_create_entry_include_flags) {
    Entry *entry;
    char *command = "sudo pacman -Syu";
    Status s = create_entry(&entry, command, NULL, NULL, NULL, 0, true);

    ck_assert(s == SUCCESS);
    ck_assert_str_eq(entry->alias, "pSyu");
    ck_assert_str_eq(entry->command, "sudo pacman -Syu");
    ck_assert_str_eq(entry->section, "pacman");
    ck_assert(!entry->is_removed);
    free_entry(entry);
}
END_TEST

START_TEST(test_create_entry_alias_override) {
    Entry *entry;
    char *command = "ls -al";
    char *alias_override = "ll";
    Status s = create_entry(&entry, command, alias_override, NULL, NULL, 0, true);

    ck_assert(s == SUCCESS);
    ck_assert_str_eq(entry->alias, "ll");
    ck_assert_str_eq(entry->command, "ls -al");
    ck_assert_str_eq(entry->section, "ls");
    ck_assert(!entry->is_removed);
    free_entry(entry);
}
END_TEST

START_TEST(test_create_entry_section_override_include_flags) {
    Entry *entry;
    char *command = "npm uninstall -g";
    char *section_override = "package_managers";
    Status s = create_entry(&entry, command, NULL, section_override, NULL, 0, true);

    ck_assert(s == SUCCESS);
    ck_assert_str_eq(entry->alias, "nug");
    ck_assert_str_eq(entry->command, "npm uninstall -g");
    ck_assert_str_eq(entry->section, "package_managers");
    ck_assert(!entry->is_removed);
    free_entry(entry);
}
END_TEST

START_TEST(test_create_entry_all_options) {
    Entry *entry;
    char *command = "cd ..";
    char *alias_override = "...";
    char *section_override = "etc";
    Status s = create_entry(&entry, command, alias_override, section_override, NULL, 0, false);

    ck_assert(s == SUCCESS);
    ck_assert_str_eq(entry->alias, "...");
    ck_assert_str_eq(entry->command, "cd ..");
    ck_assert_str_eq(entry->section, "etc");
    ck_assert(!entry->is_removed);
    free_entry(entry);
}
END_TEST

Suite *entry_suite(void) {
    Suite *s = suite_create("Entry");

    TCase *tc_helpers = tcase_create("Entry helpers");
    tcase_add_test(tc_helpers, test_create_alias_name);
    tcase_add_test(tc_helpers, test_create_alias_name_include_flags);
    tcase_add_test(tc_helpers, test_create_section_name);
    tcase_add_test(tc_helpers, test_create_section_name_sudo);
    tcase_add_test(tc_helpers, test_hash_alias);
    suite_add_tcase(s, tc_helpers);

    TCase *tc_ = tcase_create("Create entry");
    tcase_add_test(tc_, test_create_entry_default1);
    tcase_add_test(tc_, test_create_entry_default2);
    tcase_add_test(tc_, test_create_entry_include_flags);
    tcase_add_test(tc_, test_create_entry_alias_override);
    tcase_add_test(tc_, test_create_entry_section_override_include_flags);
    tcase_add_test(tc_, test_create_entry_all_options);
    suite_add_tcase(s, tc_);

    return s;
}
