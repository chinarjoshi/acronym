#include <stdlib.h>
#include <check.h>
#include "../src/parse_args/parse_args.h"
#include "test.h"

START_TEST(test_parse_args_add_normal) {
    int argc = 3;
    char *argv[] = { "acronym", "add", "git add -A" };
    Cli *cli = parse_args(argc, argv);
    ck_assert_int_eq(cli->type, ADD);
    struct Add a = cli->cmd.add;
    ck_assert_str_eq("git add -A", a.command);
    ck_assert_ptr_null(a.alias_override);
    ck_assert_ptr_null(a.section_override);
    ck_assert(!a.include_flags);
    ck_assert(!a.local);
}
END_TEST

START_TEST(test_parse_args_add_alias_override_local) {
    int argc = 5;
    char *argv[] = { "acronym", "add", "git add -A", "-la", "ga" };
    Cli *cli = parse_args(argc, argv);
    ck_assert_int_eq(cli->type, ADD);
    struct Add a = cli->cmd.add;
    ck_assert_str_eq("git add -A", a.command);
    ck_assert_str_eq("ga", a.alias_override);
    ck_assert_ptr_null(a.section_override);
    ck_assert(!a.include_flags);
    ck_assert(a.local);
}
END_TEST

START_TEST(test_parse_args_remove_normal) {
    int argc = 5;
    char *argv[] = { "acronym", "remove", "a", "b", "-rf" };
    Cli *cli = parse_args(argc, argv);
    ck_assert_int_eq(cli->type, REMOVE);
    struct Remove r = cli->cmd.remove;
    ck_assert_str_eq(r.aliases->data, "b");
    ck_assert_str_eq(r.aliases->next->data, "a");
    ck_assert(r.force);
    ck_assert(r.recursive);
    ck_assert(!r.local);
    ck_assert(!r.interactive);
}
END_TEST

START_TEST(test_parse_args_remove_all_flags) {
    int argc = 8;
    char *argv[] = { "acronym", "remove", "-r", "a", "--force", "b", "-l", "c" };
    Cli *cli = parse_args(argc, argv);
    ck_assert_int_eq(cli->type, REMOVE);
    struct Remove r = cli->cmd.remove;
    ck_assert_str_eq(r.aliases->data, "c");
    ck_assert_str_eq(r.aliases->next->data, "b");
    ck_assert_str_eq(r.aliases->next->next->data, "a");
    ck_assert(r.force);
    ck_assert(r.recursive);
    ck_assert(r.local);
}

START_TEST(test_parse_args_tree_normal) {
    int argc = 6;
    char *argv[] = { "acronym", "tree", "run", "test", "-ad", "~/dotfiles" };
    Cli *cli = parse_args(argc, argv);
    ck_assert_int_eq(cli->type, TREE);
    struct Tree t = cli->cmd.tree;
    ck_assert_str_eq(t.aliases->data, "test");
    ck_assert_str_eq(t.aliases->next->data, "run");
    ck_assert_str_eq(t.directory, "~/dotfiles");
    ck_assert(t.all);
}
END_TEST

START_TEST(test_parse_args_show_normal) {
    int argc = 4;
    char *argv[] = { "acronym", "show", "-d", "~/projects/acronym" };
    Cli *cli = parse_args(argc, argv);
    ck_assert_int_eq(cli->type, SHOW);
    ck_assert_str_eq(cli->cmd.show.directory, "~/projects/acronym");
    ck_assert(!cli->cmd.show.all);
}
END_TEST

START_TEST(test_parse_args_edit_normal) {
    int argc = 4;
    char *argv[] = { "acronym", "edit", "-e", "emacs" };
    Cli *cli = parse_args(argc, argv);
    ck_assert_str_eq(cli->cmd.edit.editor, "emacs");
    ck_assert(!cli->cmd.edit.local);
}
END_TEST

Suite *parse_args_suite(void) {
    Suite *s = suite_create("Parse Args");

    TCase *tc_add = tcase_create("Add");
    tcase_add_test(tc_add, test_parse_args_add_normal);
    tcase_add_test(tc_add, test_parse_args_add_alias_override_local);
    suite_add_tcase(s, tc_add);

    TCase *tc_remove = tcase_create("Remove");
    tcase_add_test(tc_remove, test_parse_args_remove_normal);
    tcase_add_test(tc_remove, test_parse_args_remove_all_flags);
    suite_add_tcase(s, tc_remove);

    TCase *tc_tree = tcase_create("Tree");
    tcase_add_test(tc_tree, test_parse_args_tree_normal);
    suite_add_tcase(s, tc_tree);

    TCase *tc_show = tcase_create("Show");
    tcase_add_test(tc_show, test_parse_args_show_normal);
    suite_add_tcase(s, tc_show);

    TCase *tc_edit = tcase_create("Edit");
    tcase_add_test(tc_edit, test_parse_args_edit_normal);
    suite_add_tcase(s, tc_edit);

    return s;
}
