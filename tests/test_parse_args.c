#include <stdlib.h>
#include <check.h>
#include "../src/parse_args/parse_args.h"
#include "test.h"

START_TEST(test_parse_args_add_normal) {
    int argc = 3;
    char *argv[] = { "acronym", "create", "git add -A" };
    Cli *cli = parse_args(argc, argv);
    ck_assert_int_eq(cli->type, CREATE);
    struct Create a = cli->cmd.create;
    ck_assert_str_eq("git add -A", a.command);
    ck_assert_ptr_null(a.alias_override);
    ck_assert_ptr_null(a.section_override);
    ck_assert(!a.include_flags);
}
END_TEST

START_TEST(test_parse_args_add_section) {
    int argc = 5;
    char *argv[] = { "acronym", "create", "git add -A", "-s", "git stuff" };
    Cli *cli = parse_args(argc, argv);
    ck_assert_int_eq(cli->type, CREATE);
    struct Create a = cli->cmd.create;
    ck_assert_str_eq("git add -A", a.command);
    ck_assert_ptr_null(a.alias_override);
    ck_assert_str_eq("git stuff", a.section_override);
    ck_assert(!a.include_flags);
}
END_TEST

START_TEST(test_parse_args_add_section_and_alias_override) {
    int argc = 7;
    char *argv[] = { "acronym", "create", "git add -A", "-a", "gaa", "-ls", "git adding" };
    Cli *cli = parse_args(argc, argv);
    ck_assert_int_eq(cli->type, CREATE);
    struct Create a = cli->cmd.create;
    ck_assert_str_eq("git add -A", a.command);
    ck_assert_str_eq("gaa", a.alias_override);
    ck_assert_str_eq("git adding", a.section_override);
    ck_assert(!a.include_flags);
}
END_TEST

START_TEST(test_parse_args_add_alias_override_local) {
    int argc = 5;
    char *argv[] = { "acronym", "create", "git add -A", "-la", "ga" };
    Cli *cli = parse_args(argc, argv);
    ck_assert_int_eq(cli->type, CREATE);
    struct Create a = cli->cmd.create;
    ck_assert_str_eq("git add -A", a.command);
    ck_assert_str_eq("ga", a.alias_override);
    ck_assert_ptr_null(a.section_override);
    ck_assert(!a.include_flags);
}
END_TEST

START_TEST(test_parse_args_remove_normal) {
    int argc = 5;
    char *argv[] = { "acronym", "delete", "a", "b", "-sf" };
    Cli *cli = parse_args(argc, argv);
    ck_assert_int_eq(cli->type, DELETE);
    struct Delete r = cli->cmd.delete;
    ck_assert_str_eq(r.aliases->data, "b");
    ck_assert_str_eq(r.aliases->next->data, "a");
    ck_assert(r.force);
    ck_assert(r.section);
    ck_assert(!r.interactive);
}
END_TEST

START_TEST(test_parse_args_remove_all_flags) {
    int argc = 8;
    char *argv[] = { "acronym", "delete", "-s", "a", "--force", "b", "-l", "c" };
    Cli *cli = parse_args(argc, argv);
    ck_assert_int_eq(cli->type, DELETE);
    struct Delete r = cli->cmd.delete;
    ck_assert_str_eq(r.aliases->data, "c");
    ck_assert_str_eq(r.aliases->next->data, "b");
    ck_assert_str_eq(r.aliases->next->next->data, "a");
    ck_assert(r.force);
    ck_assert(r.section);
}

START_TEST(test_parse_args_show_normal) {
    int argc = 2;
    char *argv[] = { "acronym", "read" };
    Cli *cli = parse_args(argc, argv);
    ck_assert_int_eq(cli->type, READ);
}
END_TEST

START_TEST(test_parse_args_edit_normal) {
    int argc = 4;
    char *argv[] = { "acronym", "update", "-e", "vim" };
    Cli *cli = parse_args(argc, argv);
    ck_assert_str_eq(cli->cmd.update.editor, "vim");
}
END_TEST

Suite *parse_args_suite(void) {
    Suite *s = suite_create("Parse Args");

    TCase *tc_add = tcase_create("Create");
    tcase_add_test(tc_add, test_parse_args_add_normal);
    tcase_add_test(tc_add, test_parse_args_add_section);
    tcase_add_test(tc_add, test_parse_args_add_section_and_alias_override);
    tcase_add_test(tc_add, test_parse_args_add_alias_override_local);
    suite_add_tcase(s, tc_add);

    TCase *tc_remove = tcase_create("Delete");
    tcase_add_test(tc_remove, test_parse_args_remove_normal);
    tcase_add_test(tc_remove, test_parse_args_remove_all_flags);
    suite_add_tcase(s, tc_remove);

    TCase *tc_show = tcase_create("Read");
    tcase_add_test(tc_show, test_parse_args_show_normal);
    suite_add_tcase(s, tc_show);

    TCase *tc_edit = tcase_create("Update");
    tcase_add_test(tc_edit, test_parse_args_edit_normal);
    suite_add_tcase(s, tc_edit);

    return s;
}
