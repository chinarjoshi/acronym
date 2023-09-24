#include <stdlib.h>
#include <check.h>
#include "../src/subcmds/subcmds.h"
#include "../src/file_io/file_io.h"
#include "test.h"

char line[256];
FILE *alias_f;
static void setup() {
    strcpy(ALIAS_FNAME, "/home/c/.acronym_test_cmd_alias");
    strcpy(TMP_FNAME, "/home/c/.acronym_test_cmd_tmpfile"); // MUST BE ON SAME DEVICE AS ABOVE
    AUTOENV_FNAME = "/home/c/.acronym_test_cmd_autoenv";
    alias_f = fopen(ALIAS_FNAME, "w");
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
    ck_assert(result);

    alias_f = fopen(ALIAS_FNAME, "r");
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
        .type = ADD,
        .verbosity = 1,
        .cmd.add = {
            .command = "pytest ./*",
            .alias_override = "test",
            .section_override = "unit test",
        },
    };

    int result = add_cmd(&cli);
    ck_assert(!result);

    alias_f = fopen(ALIAS_FNAME, "r");
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
        .type = REMOVE,
        .verbosity = 1,
        .cmd.remove.aliases = &a,
    };

    int result = remove_cmd(&cli);
    ck_assert(result);

    alias_f = fopen(ALIAS_FNAME, "r");
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

START_TEST(test_show_cmd_normal) {
    const char *path = "/home/c/.aliases";
    strncpy(ALIAS_FNAME, path, strlen(path));
    Cli cli = {
        .type = SHOW,
        .verbosity = 1,
    };

    ck_assert(show_cmd(&cli));
}
END_TEST

START_TEST(test_show_cmd_local) {
    const char *path = "/home/c/.aliases";
    strncpy(ALIAS_FNAME, path, strlen(path));
    Cli cli = {
        .type = SHOW,
        .verbosity = 1,
        .cmd.show.local = true
    };

    ck_assert(show_cmd(&cli));
}
END_TEST

START_TEST(test_show_cmd_aliases) {
    const char *path = "/home/c/.aliases";
    strncpy(ALIAS_FNAME, path, strlen(path));
    AliasListNode b = { .data = "gp", .next = NULL };
    AliasListNode a = { .data = "a", .next = &b };
    Cli cli = {
        .type = SHOW,
        .verbosity = 1,
        .cmd.show.prefixes = &a
    };

    ck_assert(show_cmd(&cli));
}
END_TEST

START_TEST(test_show_cmd_commit_hash) {
    const char *path = "/home/c/.aliases";
    strncpy(ALIAS_FNAME, path, strlen(path));
    Cli cli = {
        .type = SHOW,
        .verbosity = 1,
        .cmd.show.commit_hash = "724976b"
    };

    ck_assert(show_cmd(&cli));
}
END_TEST

START_TEST(test_sync_cmd_normal) {
    const char *path = "/home/c/.aliases";
    strncpy(ALIAS_FNAME, path, strlen(path));
    Cli cli = {
        .type = SYNC,
        .verbosity = 1,
    };

    ck_assert(sync_cmd(&cli));
}
END_TEST

START_TEST(test_reccomend_cmd_normal) {
    const char *path = "/home/c/.aliases";
    strncpy(ALIAS_FNAME, path, strlen(path));
    Cli cli = {
        .type = RECCOMEND,
        .verbosity = 1,
    };

    ck_assert(reccomend_cmd(&cli));
}
END_TEST

START_TEST(test_compare_paths) {
    char *env_file = "/home/c/projects/.env";
    char *path1 = "/home/c/projects";
    char *outside_path = "/home/c/asdf";
    char *child_path = "/home/c/projects/acronym";
    char *parent_path = "/home/c";

    ck_assert(compare_paths(env_file, path1) == PATH_EQ);
    ck_assert(compare_paths(env_file, outside_path) == PATH_UNRELATED);
    ck_assert(compare_paths(env_file, child_path) == PATH_CHILD);
    ck_assert(compare_paths(env_file, parent_path) == PATH_PARENT);
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

    TCase *tc_show = tcase_create("Show cmd");
    tcase_add_test(tc_show, test_show_cmd_normal);
    tcase_add_test(tc_show, test_show_cmd_aliases);
    tcase_add_test(tc_show, test_show_cmd_commit_hash);
    tcase_add_test(tc_show, test_show_cmd_local);
    suite_add_tcase(s, tc_show);

    TCase *tc_sync = tcase_create("Sync cmd");
    tcase_add_test(tc_sync, test_sync_cmd_normal);
    suite_add_tcase(s, tc_sync);

    TCase *tc_reccomend = tcase_create("Reccomend cmd");
    tcase_add_test(tc_reccomend, test_reccomend_cmd_normal);
    suite_add_tcase(s, tc_reccomend);

    TCase *tc_helpers = tcase_create("Subcmd helpers");
    tcase_add_test(tc_helpers, test_compare_paths);
    suite_add_tcase(s, tc_helpers);
    return s;
}
