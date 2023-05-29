#include <check.h>
#include <stdlib.h>
#include "../src/ACEntry.h"
#include "test.h"

START_TEST(test_create_acronym) {
    char *command = "grep --color=auto";
    char *acronym = create_acronym(command, false);
    ck_assert_str_eq(acronym, "g");
    free(acronym);
}
END_TEST

START_TEST(test_create_acronym_include_flags) {
    char *command = "rm -rf";
    char *acronym = create_acronym(command, true);
    ck_assert_str_eq(acronym, "rrf");
    free(acronym);
}
END_TEST

START_TEST(test_create_set_name) {
    char *command = "sudo jupyter notebook .";
    char *set = create_set_name(command);
    ck_assert_str_eq(set, "jupyter");
    free(set);
}
END_TEST

START_TEST(test_hash_acronym) {
    char *acronym1 = "gs";
    char *acronym2 = "gst";
    int capacity = 7;
    
    int hash1 = hash_acronym(acronym1, capacity);
    int hash2 = hash_acronym(acronym2, capacity);

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
    ck_assert_str_eq(entry->acronym, "gb");
    ck_assert_str_eq(entry->command, "git branch --all");
    ck_assert_str_eq(entry->set, "git");
    ck_assert(!entry->is_removed);
    free_ACEntry(entry);
}
END_TEST

START_TEST(test_create_ACEntry_default2) {
    ACEntry *entry;
    char *command = "meson compile -C ~/projects/acronym/builds";
    Status s = create_ACEntry(&entry, command, NULL, NULL, false);

    ck_assert(s == SUCCESS);
    ck_assert_str_eq(entry->acronym, "mc");
    ck_assert_str_eq(entry->command, "meson compile -C ~/projects/acronym/builds");
    ck_assert_str_eq(entry->set, "meson");
    ck_assert(!entry->is_removed);
    free_ACEntry(entry);
}
END_TEST

START_TEST(test_create_ACEntry_include_flags) {
    ACEntry *entry;
    char *command = "sudo pacman -Syu";
    Status s = create_ACEntry(&entry, command, NULL, NULL, true);

    ck_assert(s == SUCCESS);
    ck_assert_str_eq(entry->acronym, "pSyu");
    ck_assert_str_eq(entry->command, "sudo pacman -Syu");
    ck_assert_str_eq(entry->set, "pacman");
    ck_assert(!entry->is_removed);
    free_ACEntry(entry);
}
END_TEST

START_TEST(test_create_ACEntry_acronym_override) {
    ACEntry *entry;
    char *command = "ls -al";
    char *acronym_override = "ll";
    Status s = create_ACEntry(&entry, command, acronym_override, NULL, true);

    ck_assert(s == SUCCESS);
    ck_assert_str_eq(entry->acronym, "ll");
    ck_assert_str_eq(entry->command, "ls -al");
    ck_assert_str_eq(entry->set, "ls");
    ck_assert(!entry->is_removed);
    free_ACEntry(entry);
}
END_TEST

START_TEST(test_create_ACEntry_set_override_include_flags) {
    ACEntry *entry;
    char *command = "npm uninstall -g";
    char *set_override = "package_managers";
    Status s = create_ACEntry(&entry, command, NULL, set_override, true);

    ck_assert(s == SUCCESS);
    ck_assert_str_eq(entry->acronym, "nug");
    ck_assert_str_eq(entry->command, "npm uninstall -g");
    ck_assert_str_eq(entry->set, "package_managers");
    ck_assert(!entry->is_removed);
    free_ACEntry(entry);
}
END_TEST

START_TEST(test_create_ACEntry_all_options) {
    ACEntry *entry;
    char *command = "cd ..";
    char *acronym_override = "...";
    char *set_override = "etc";
    Status s = create_ACEntry(&entry, command, acronym_override, set_override, false);

    ck_assert(s == SUCCESS);
    ck_assert_str_eq(entry->acronym, "...");
    ck_assert_str_eq(entry->command, "cd ..");
    ck_assert_str_eq(entry->set, "etc");
    ck_assert(!entry->is_removed);
    free_ACEntry(entry);
}
END_TEST

Suite *ACEntry_suite(void) {
    Suite *s = suite_create("ACEntry");

    TCase *tc_helpers = tcase_create("AC helpers");
    tcase_add_test(tc_helpers, test_create_acronym);
    tcase_add_test(tc_helpers, test_create_acronym_include_flags);
    tcase_add_test(tc_helpers, test_create_set_name);
    tcase_add_test(tc_helpers, test_hash_acronym);
    suite_add_tcase(s, tc_helpers);

    TCase *tc_AC = tcase_create("Create AC");
    tcase_add_test(tc_AC, test_create_ACEntry_default1);
    tcase_add_test(tc_AC, test_create_ACEntry_default2);
    tcase_add_test(tc_AC, test_create_ACEntry_include_flags);
    tcase_add_test(tc_AC, test_create_ACEntry_acronym_override);
    tcase_add_test(tc_AC, test_create_ACEntry_set_override_include_flags);
    tcase_add_test(tc_AC, test_create_ACEntry_all_options);
    suite_add_tcase(s, tc_AC);

    return s;
}
