#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include "subcmds.h"
#include <time.h>
#include "../file_io/file_io.h"

static char _cmd_buf[256];
static inline bool git_cmd(const char *cmd, const char *optarg) {
    snprintf(_cmd_buf, sizeof(_cmd_buf), "git -qC %s %s %s", ACRONYM_DIRNAME, cmd, optarg);
    return system(_cmd_buf);
}

bool sync_cmd(Cli *cli) {
    struct Sync s = cli->cmd.sync;

    // Change directory to the acronym directory
    if (chdir(ACRONYM_DIRNAME) != 0) {
        fprintf(stderr, "Error: Failed to change directory to %s.\n", ACRONYM_DIRNAME);
        return false;
    }
    // Ensure that it is a valid git repository
    if (!git_cmd("rev-parse --is-inside-work-tree", 0)) {
        if (!git_cmd("init", 0)) {
            fprintf(stderr, "Error: Failed to initialize Git repository in %s.\n", ACRONYM_DIRNAME);
            return false;
        }
    }

    if (s.remote_URL) {
        if (git_cmd("remote set-url origin", s.remote_URL))
            return false;
    } else if (s.forward) {
        if (git_cmd("checkout HEAD@", s.forward))
            return false;
    } else if (s.backward) {
        if (git_cmd("checkout HEAD~", s.backward))
            return false;
    } else if (s.commit_hash) {
        if (git_cmd("checkout", s.commit_hash))
            return false;
    }

    // No-args case
    else {
        if (git_cmd("remote show", 0)) {
            if (!git_cmd("pull", 0)) {
                fprintf(stderr, "Error: Git pull failed.\n");
                return false;
            }
        }

        // Add aliases.sh and commit
        char diff_buf[256];
        get_diff_stats(diff_buf, sizeof(diff_buf));
        if (strlen(diff_buf) == 0) {
            return true; // Nothing to do
        }
        if (!git_cmd("add aliases.sh && git commit -m", diff_buf)) {
            fprintf(stderr, "Error: Git add and commit failed.\n");
            return false;
        }

        if (git_cmd("remote show", 0)) {
            if (!git_cmd("push", 0)) {
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
    snprintf(buffer, buffer_size, "%s: %s", date, ptr);
}
