#include <stdlib.h>
#include <check.h>
#include "../src/subcmds/subcmds.h"
#include "test.h"

char line[256];
FILE *alias_f;
static void setup() {
    strcpy(ALIAS_FNAME, "/tmp/acronym_test_cmd_alias");
    strcpy(TMP_FNAME, "/tmp/acronym_test_cmd_tmpfile"); // MUST BE ON SAME DEVICE AS ABOVE
    AUTOENV_FNAME = "/tmp/acronym_test_cmd_autoenv";
    alias_f = fopen(ALIAS_FNAME, "w");
    fputs(
"# --- Aliases ---\n"
"alias test='CK_FORK=yes ~/projects/acronym/builds/tests' ## unit test\n"
"\n"
"CK_RUN_SUITE=\"Hash Table\"\n"
"CK_FORK=no\n"
"DEBUG_EXECUTABLE=~/projects/acronym/builds/tests\n", alias_f);
    fclose(alias_f);
}

static void teardown() {
    fclose(alias_f);
    remove(ALIAS_FNAME);
}

START_TEST(test_add_cmd_normal) {
    Cli cli = {
        .type = ADD,
        .verbosity = 1,
        .cmd.add = {
            .command = "grep --color=auto",
            .section_override = "utils",
            .include_flags = true,
        },
    };

    int result = add_cmd(&cli);
    ck_assert(!result);

    alias_f = fopen(ALIAS_FNAME, "r");
    bool found_new_line = false, found_old_line = false;
    while (fgets(line, sizeof(line), alias_f)) {
        if (strncmp("alias gc", line, 8)) {
            ck_assert_str_eq("alias gc=\"grep --color=auto\" ## utils\n", line);
            found_new_line = true;
        } else if (strncmp("alias test", line, 10)) {
            ck_assert_str_eq("alias test='CK_FORK=yes ~/projects/acronym/builds/tests' ## unit test\n", line);
            found_old_line = true;
        }
    }
    ck_assert(found_new_line && found_old_line);
}
END_TEST

START_TEST(test_add_cmd_duplicate) {
    Cli cli = {
        .type = ADD,
        .verbosity = 1,
        .cmd.add = {
            .command = "pytest ./*",
            .alias_override = "test",
            .section_override = "unit test",
        },
    };

    int result = add_cmd(&cli);
    ck_assert(result);

    alias_f = fopen(ALIAS_FNAME, "r");
    while (fgets(line, sizeof(line), alias_f)) {
        if (strncmp("alias gc", line, 8)) {
            ck_assert_str_eq("alias gc=\"grep --color=auto\" ## utils\n", line);
            return;
        }
    }
}
END_TEST

START_TEST(test_remove_cmd_normal) {
    AliasListNode a = { .data = "test", .next = NULL };
    Cli cli = {
        .type = REMOVE,
        .verbosity = 1,
        .cmd.remove.aliases = &a,
    };

    int result = remove_cmd(&cli);
    ck_assert(!result);

    alias_f = fopen(ALIAS_FNAME, "r");
    while (fgets(line, sizeof(line), alias_f)) {
        if (strncmp("alias test='CK_FORK=", line, 20)) {
            ck_abort();
        }
    }
}
END_TEST

Suite *subcmds_suite(void) {
    Suite *s = suite_create("Subcmds");

    TCase *tc_add = tcase_create("Add cmd");
    tcase_add_test(tc_add, test_add_cmd_normal);
    tcase_add_test(tc_add, test_add_cmd_duplicate);
    tcase_add_checked_fixture(tc_add, setup, teardown);
    suite_add_tcase(s, tc_add);

    TCase *tc_remove = tcase_create("Remove cmd");
    tcase_add_test(tc_remove, test_remove_cmd_normal);
    tcase_add_checked_fixture(tc_remove, setup, teardown);
    suite_add_tcase(s, tc_remove);

    // TCase *tc_show = tcase_create("Show cmd");
    // tcase_add_test(tc_show, test_create_alias_name_include_flags);
    // suite_add_tcase(s, tc_show);
    //
    // TCase *tc_tree = tcase_create("Tree cmd");
    // tcase_add_test(tc_tree, test_create_alias_name_include_flags);
    // suite_add_tcase(s, tc_tree);
    //
    return s;
}
