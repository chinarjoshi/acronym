#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include "subcmds.h"
#include <time.h>
#include "../file_io/file_io.h"

static inline bool has_remote() { return system("git remote show"); }
static inline bool init_git_repo() { return system("git init --quiet"); }
static inline bool pull_changes() { return system("git pull --quiet"); }
static inline bool push_changes() { return system("git push --quiet"); }
static inline bool is_valid_git_repo() { return system(
    "git rev-parse --is-inside-work-tree > /dev/null 2>&1"); }
static inline bool add_and_commit(const char *message) {
    char cmd[128];
    snprintf(cmd, sizeof(cmd), "git add aliases.sh && git commit -m '%s'", message);
    return system(cmd);
}

bool sync_cmd(Cli *cli) {
    struct Sync s = cli->cmd.sync;

    // Change directory to the acronym directory
    if (chdir(ACRONYM_DIRNAME) != 0) {
        fprintf(stderr, "Error: Failed to change directory to %s.\n", ACRONYM_DIRNAME);
        return false;
    }
    // Ensure that it is a valid git repository
    if (!is_valid_git_repo()) {
        if (!init_git_repo()) {
            fprintf(stderr, "Error: Failed to initialize Git repository in %s.\n", ACRONYM_DIRNAME);
            return false;
        }
    }

    char command[256];
    if (s.remote_URL) {
        snprintf(command, sizeof(command), "git remote set-url origin %s", s.remote_URL);
        if (system(command))
            return false;
    } else if (s.forward) {
        snprintf(command, sizeof(command), "git checkout HEAD@{%d}", s.forward);
        if (system(command))
            return false;
    } else if (s.backward) {
        snprintf(command, sizeof(command), "git checkout HEAD~%d", s.forward);
        if (system(command))
            return false;
    } else if (s.commit_hash) {
        snprintf(command, sizeof(command), "git checkout %s", s.commit_hash);
        if (system(command) != 0) {
            return false;  // Failed to checkout to the commit hash
        }
    }

    // No-args case
    else {
        if (has_remote()) {
            if (!pull_changes()) {
                fprintf(stderr, "Error: Git pull failed.\n");
                return false;
            }
        }

        // Add aliases.sh and commit
        time_t t = time(NULL);
        struct tm *tm_info = localtime(&t);
        char timestamp[64];
        strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", tm_info);
        if (!add_and_commit(timestamp)) {
            fprintf(stderr, "Error: Git add and commit failed.\n");
            return false;
        }

        if (has_remote()) {
            if (!push_changes()) {
                fprintf(stderr, "Error: Git push failed.\n");
                return false;
            }
        }
    }

    return true;
}
