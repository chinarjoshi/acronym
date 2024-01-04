#include <unistd.h>
#include <string.h>
#include <pcre.h>
#include <sys/stat.h>
#include <linux/limits.h>
#include "file_io.h"
#include "../parse_args/parse_args.h"
#define ACRONYM_FILENAME_DEFAULT ".aliases.sh"
#define ACRONYM_LOCAL_FILENAME_DEFAULT ".env"

char *ACRONYM_FILENAME;
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
char PATH_BUFFER[PATH_MAX];

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
        -exec dirname {} \\; \
        -quit \
    | grep . \
    || git rev-parse --show-toplevel";


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

    if (IS_IN_GIT_REPO) {
        // If in git repo, set dir to the directory with the aliases file
        char command_buf[strlen(find_git_alias_dir_cmd) + strlen(ACRONYM_FILENAME) + 1];
        char directory[PATH_MAX];
        sprintf(command_buf, find_git_alias_dir_cmd, ACRONYM_FILENAME);

        FILE *fp = popen(command_buf, "r");
        fgets(directory, sizeof(directory), fp);
        pclose(fp);

        directory[strlen(directory) - 1] = '\0'; // After find command there is a trailing \n we need to strip

        // Write to git-specific buffers
        snprintf(PROJ_ALIASES_PATH, PATH_MAX, "%s/%s", directory, ACRONYM_FILENAME);
        snprintf(LOCAL_ALIASES_PATH, PATH_MAX, "%s/%s", directory, ACRONYM_LOCAL_FILENAME);
    }

    // Write to mandatory buffers
    snprintf(GLOBAL_ALIASES_PATH, PATH_MAX, "%s/%s", ACRONYM_GLOBAL_DIR, ACRONYM_FILENAME);
    snprintf(TMP_MISMATCHES_PATH, PATH_MAX, "%s/%s", ACRONYM_GLOBAL_DIR, ".acronym.tmp");
    snprintf(TMP_TOML_PATH, PATH_MAX, "%s/%s", ACRONYM_GLOBAL_DIR, ".acronym.toml");

    if (scope == GLOBAL)
        ALIASES_PATH = GLOBAL_ALIASES_PATH;
    else if (scope == LOCAL)
        ALIASES_PATH = LOCAL_ALIASES_PATH;
    else
        ALIASES_PATH = PROJ_ALIASES_PATH;

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
