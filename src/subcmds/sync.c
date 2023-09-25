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
void get_diff_stats(char *buffer, size_t buffer_size);

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
    command[0] = '\0';
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
        get_diff_stats(command, sizeof(command));
        if (strlen(command) == 0) {
            return true; // Nothing to do
        }
        if (!add_and_commit(command)) {
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

void get_diff_stats(char *buffer, size_t buffer_size) {
    char tmp[256];

    // Run git diff command and read output
    FILE *fp = popen("git diff --stat", "r");
    if (!fp) {
        perror("popen");
        exit(EXIT_FAILURE);
    }

    // If the file is empty, then there are no changes to commit.
    if (!fgets(tmp, sizeof(tmp), fp))
        return; 

    // Read the second line
    fgets(tmp, sizeof(tmp), fp);
    pclose(fp);

    // Remove "1 file changed," since it's always 1 file
    char *ptr = strchr(tmp, ',');
    if (ptr && *(ptr + 1)) {
        ptr += 2;
    } else {
        strcpy(buffer, "No changes");
        return;
    }

    // Get current date
    time_t t = time(NULL);
    struct tm *tm_info = localtime(&t);
    char date[16];
    strftime(date, sizeof(date), "%b %d:", tm_info);

    // Combine date and git stats
    snprintf(buffer, buffer_size, "%s %s", date, ptr);
}
