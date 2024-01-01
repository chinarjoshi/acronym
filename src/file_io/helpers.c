#include <unistd.h>
#include <string.h>
#include <pcre.h>
#include <sys/stat.h>
#include <linux/limits.h>
#include "file_io.h"
#include "../parse_args/parse_args.h"
#define ACRONYM_FILENAME_DEFAULT ".aliases.sh"
#define ACRONYM_LOCAL_FILENAME_DEFAULT ".env"

static char *ACRONYM_FILENAME;
static char *ACRONYM_LOCAL_FILENAME;
static char *ACRONYM_GLOBAL_DIR;
bool IS_IN_GIT_REPO;

// The execution scopes, required by all the subcommands.
char *ALIASES_PATH;
char GLOBAL_ALIASES_PATH[PATH_MAX];
char PROJ_ALIASES_PATH[PATH_MAX];
char LOCAL_ALIASES_PATH[PATH_MAX];

// Temporary files for writing and editing
char TMP_MISMATCHES_PATH[PATH_MAX];
char TMP_TOML_PATH[PATH_MAX];

const char *in_git_repo_cmd = "git rev-parse --is-inside-work-tree >/dev/null 2>&1";
// This command returns the directory of the project level aliases file. 
// if you are in a Git repository.
//   if there is a file matching %s, use its directory
//   else, use the Git repository root.
// else, use the current working directory.
const char *find_git_alias_dir_cmd = "\
    find $(git rev-parse --show-toplevel) \
        -maxdepth 2 \
        -type f \
        -name '%s' \
    | head -n1 \
    || git rev-parse --show-toplevel \
    | xargs realpath --relative-to=$(pwd) \
    | xargs dirname";


static void get_env_vars() {
    char *c = getenv("ACRONYM_FILENAME");
    ACRONYM_FILENAME = (c) ? c : ACRONYM_FILENAME_DEFAULT;

    c = getenv("ACRONYM_LOCAL_FILENAME");
    ACRONYM_LOCAL_FILENAME = (c) ? c : ACRONYM_LOCAL_FILENAME_DEFAULT;

    // Ensure main and local aliases file names are not the same
    if (!strcmp(ACRONYM_FILENAME, ACRONYM_LOCAL_FILENAME))
        fprintf(stderr, "Error (Environment): ACRONYM_FILENAME and ACRONYM_LOCAL_FILENAME cannot be equal");

    c = getenv("ACRONYM_GLOBAL_DIR");
    ACRONYM_GLOBAL_DIR = (c) ? c : getenv("HOME");

    // Ensure global directory exists
    struct stat st = {0};
    if (stat(ACRONYM_GLOBAL_DIR, &st) == -1)
        fprintf(stderr, "Error (File I/O): ACRONYM_GLOBAL_DIR is invalid path.");
}

void setup_path_buffers(Scope scope) {
    get_env_vars();

    ALIASES_PATH = (scope == LOCAL) ? ACRONYM_LOCAL_FILENAME : ACRONYM_FILENAME;
    IS_IN_GIT_REPO = !system(in_git_repo_cmd);

    // If not global scope, set dir to the directory in repo with the aliases file
    char *dir = ACRONYM_GLOBAL_DIR;
    if (scope != GLOBAL) {
        if (IS_IN_GIT_REPO) {
            char command_buf[strlen(find_git_alias_dir_cmd) + strlen(ACRONYM_FILENAME) + 1];
            char output_buf[PATH_MAX];
            sprintf(command_buf, find_git_alias_dir_cmd, ACRONYM_FILENAME);
            FILE *fp = popen(command_buf, "r");
            dir = fgets(output_buf, sizeof(output_buf), fp);
            pclose(fp);
        } else {
            getcwd(dir, PATH_MAX);
        }
    }

    // Write to buffers
    snprintf(GLOBAL_ALIASES_PATH, PATH_MAX, "%s/%s", ACRONYM_GLOBAL_DIR, ACRONYM_FILENAME);
    snprintf(PROJ_ALIASES_PATH, PATH_MAX, "%s/%s", dir, ACRONYM_FILENAME);
    snprintf(LOCAL_ALIASES_PATH, PATH_MAX, "%s/%s", dir, ACRONYM_LOCAL_FILENAME);

    snprintf(TMP_MISMATCHES_PATH, PATH_MAX, "%s/%s", ACRONYM_GLOBAL_DIR, ".acronym.tmp");
    snprintf(TMP_TOML_PATH, PATH_MAX, "%s/%s", ACRONYM_GLOBAL_DIR, ".acronym.toml");

    ALIASES_PATH = GLOBAL_ALIASES_PATH;
    if (scope == LOCAL) {
        ALIASES_PATH = LOCAL_ALIASES_PATH;
    } else {
        ALIASES_PATH = PROJ_ALIASES_PATH;
    }

    // If alias file doesn't exist already, then make it
    if (access(ALIASES_PATH, F_OK) == -1) {
        FILE *f = fopen(ALIASES_PATH, "w"); 
        fclose(f);
    }
}

void free_re_resources(pcre *re, pcre_extra *extras, FILE *f) {
    pcre_free(re);
    pcre_free_study(extras);
    fclose(f);
}
