#include <stdlib.h>
#include <check.h>
#include "../src/subcmds/subcmds.h"
#include "../src/file_io/file_io.h"
#include "test.h"

char line[256];
FILE *alias_f;
static void setup() {
    ALIASES_PATH = "/home/c/.acronym_test_cmd_alias";
    IS_IN_GIT_REPO = false;
    strcpy(TMP_MISMATCHES_PATH, "/home/c/.acronym_test_cmd_tmpfile"); // MUST BE ON SAME DEVICE AS ABOVE
    strcpy(GLOBAL_ALIASES_PATH, ALIASES_PATH);
    alias_f = fopen(ALIASES_PATH, "w");
    fputs(
"# --- Aliases ---\n"
"alias test='CK_FORK=yes ~/projects/acronym/builds/tests' ## unit test\n"
"alias gd='git diff' ## git\n"
"\n"
"CK_RUN_SUITE=\"Hash Table\"\n"
"CK_FORK=no\n"
"DEBUG_EXECUTABLE=~/projects/acronym/builds/tests\n", alias_f);
    fclose(alias_f);
}

static void teardown() {
    fclose(alias_f);
    remove(ALIASES_PATH);
}

START_TEST(test_add_cmd_normal) {
    Cli cli = {
        .type = CREATE,
        .verbosity = 1,
        .cmd.create = {
            .command = "grep --color=auto",
            .section_override = "utils",
            .include_flags = true,
        },
    };

    int result = create_cmd(&cli);
    ck_assert(result);

    alias_f = fopen(ALIASES_PATH, "r");
    bool found_new_line = false, found_old_line = false;
    while (fgets(line, sizeof(line), alias_f)) {
        if (!strncmp("alias gc", line, 8)) {
            ck_assert_str_eq("alias gc='grep --color=auto' ## utils\n", line);
            found_new_line = true;
        } else if (!strncmp("alias test", line, 10)) {
            ck_assert_str_eq("alias test='CK_FORK=yes ~/projects/acronym/builds/tests' ## unit test\n", line);
            found_old_line = true;
        }
    }
    ck_assert(found_new_line && found_old_line);
}
END_TEST

START_TEST(test_add_cmd_duplicate) {
    Cli cli = {
        .type = CREATE,
        .verbosity = 1,
        .cmd.create = {
            .command = "pytest ./*",
            .alias_override = "test",
            .section_override = "unit test",
        },
    };

    int result = create_cmd(&cli);
    ck_assert(!result);

    alias_f = fopen(ALIASES_PATH, "r");
    while (fgets(line, sizeof(line), alias_f)) {
        if (!strncmp("alias test", line, 10)) {
            ck_assert_str_eq("alias test='CK_FORK=yes ~/projects/acronym/builds/tests' ## unit test\n", line);
            return;
        }
    }
}
END_TEST

START_TEST(test_remove_cmd_normal) {
    AliasListNode a = { .data = "test", .next = NULL };
    Cli cli = {
        .type = DELETE,
        .verbosity = 1,
        .cmd.delete.aliases = &a,
    };

    int result = delete_cmd(&cli);
    ck_assert(result);

    alias_f = fopen(ALIASES_PATH, "r");
    bool found_original_line = false;
    bool found_env_variable = false;
    while (fgets(line, sizeof(line), alias_f)) {
        if (!strncmp("alias test='CK_FORK=", line, 20)) {
            ck_abort();
        } else if (!strncmp("alias gd", line, 8)) {
            ck_assert_str_eq("alias gd='git diff' ## git\n", line);
            found_original_line = true;
        } else if (!strncmp("CK_RUN_SUITE", line, 12)) {
            ck_assert_str_eq("CK_RUN_SUITE=\"Hash Table\"\n", line);
            found_env_variable = true;
        }
    }
    ck_assert(found_original_line && found_env_variable);
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

    return s;
}
