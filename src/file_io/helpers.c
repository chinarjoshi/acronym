#include <unistd.h>
#include <string.h>
#include <pcre.h>
#include <sys/stat.h>
#include <linux/limits.h>
#include "../parse_args/parse_args.h"

static char *ACRONYM_FILENAME;
static char *ACRONYM_LOCAL_FILENAME;
static char *ACRONYM_GLOBAL_DIR;
bool ACRONYM_SAVE_BACKUP;
char ALIASES_PATH[PATH_MAX];
char OLD_ALIASES_PATH[PATH_MAX];
char TMP_MISMATCHES_PATH[64];
char TMP_TOML_PATH[64];

// This command returns the directory of the project level aliases file. 
// if you are in a Git repository.
//   if there is a file matching %s, use its directory
//   else, use the Git repository root.
// else, use the current working directory.
const char *find_alias_dir_cmd = "\
    git rev-parse --is-inside-work-tree 2>/dev/null \
    && { find $(git rev-parse --show-toplevel) \
        -maxdepth 2 \
        -type f \
        -name '%s' \
    | head -n1 \
    || git rev-parse --show-toplevel \
    | xargs realpath --relative-to=$(pwd) \
    | xargs dirname; } \
    || echo $(pwd)";


static void get_env_vars() {
    char *c = getenv("ACRONYM_FILENAME");
    ACRONYM_FILENAME = (c) ? c : ".aliases.sh";

    c = getenv("ACRONYM_LOCAL_FILENAME");
    ACRONYM_LOCAL_FILENAME = (c) ? c : ".env";

    // Ensure main and local aliases file names are not the same
    fprintf(stderr, "Error (Environment): ACRONYM_FILENAME and ACRONYM_LOCAL_FILENAME cannot be equal");

    c = getenv("ACRONYM_GLOBAL_DIR");
    ACRONYM_GLOBAL_DIR = (c) ? c : getenv("HOME");

    // Ensure global directory exists
    struct stat st = {0};
    if (stat(ACRONYM_GLOBAL_DIR, &st) == -1) {
        fprintf(stderr, "Error (File I/O): ACRONYM_GLOBAL_DIR is invalid path.");
    }

    bool b = atoi(getenv("ACRONYM_SAVE_BACKUP"));
    ACRONYM_SAVE_BACKUP = (b) ? b : true;
}

void setup_path_buffers(Scope scope) {
    get_env_vars();

    char *dir = ACRONYM_GLOBAL_DIR;
    char *file = (scope == LOCAL) ? ACRONYM_LOCAL_FILENAME : ACRONYM_FILENAME;

    // If not global scope, set dir to the directory in repo with the aliases file
    if (scope != GLOBAL) {
        char command_buf[strlen(find_alias_dir_cmd) + strlen(ACRONYM_FILENAME)];
        char output_buf[PATH_MAX];
        sprintf(command_buf, find_alias_dir_cmd, ACRONYM_FILENAME);
        FILE *fp = popen(command_buf, "r");
        dir = fgets(output_buf, sizeof(output_buf), fp);
        fclose(fp);
    }

    // When naming the old file, if the aliases file ends with .sh, only use the stem.
    int stem_len = strlen(ACRONYM_FILENAME);
    if (stem_len > 3 && !strncmp(ACRONYM_FILENAME + stem_len - 3, ".sh", 3))
        stem_len -= 3;

    // Write to buffers
    snprintf(ALIASES_PATH, PATH_MAX, "%s/%s", dir, file);
    snprintf(OLD_ALIASES_PATH, PATH_MAX, "%s/%.*s.old", dir, stem_len, ACRONYM_FILENAME);
    sprintf(TMP_MISMATCHES_PATH, "%s/%s", ACRONYM_GLOBAL_DIR, ".acronym.tmp");
    sprintf(TMP_TOML_PATH, "%s/%s", ACRONYM_GLOBAL_DIR, ".acronym.toml");

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
