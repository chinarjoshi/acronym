#include <linux/limits.h>
#include <stdio.h>
#include <pcre.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "../parse_args/parse_args.h"
#include <sys/stat.h>
char *ACRONYM_ALIAS_FNAME;
char ALIAS_FILE[PATH_MAX];
char OLD_ALIAS_FILE[PATH_MAX];
char TMP_MISMATCHES_FILE[64];
char TMP_TOML_FILE[64];

// Assuming you're in a git repository, this formatted command 
// finds the relative path to the directory of the given file
// up to 2 level deep.
const char *find_alias_dir_cmd = "\
    find $(git rev-parse --show-toplevel) \
        -maxdepth 2 \
        -type f \
        -name '%s' \
    | head -n1 \
    | xargs realpath --relative-to=$(pwd) \
    | xargs dirname";

// Sets the following global char buffers:
// ACRONYM_ALIAS_FNAME: its env variable, or ".aliases.sh".
//      Let {} be a placeholder for this value.
// ALIAS_FILE: ~/{} if global scope,
//             */{} if project scope, where * is git repo root
//          or */{}.local if local project scope,
// OLD_ALIAS_FILE: */{}.old,
// TMP_MISMATCHES_FILE: ~/.acronym.tmp,
// TMP_TOML_FILE: ~/.acronym.toml.
void setup_path_buffers(Scope scope) {
    ACRONYM_ALIAS_FNAME = getenv("ACRONYM_ALIAS_FNAME");
    if (!ACRONYM_ALIAS_FNAME)
        ACRONYM_ALIAS_FNAME = ".aliases.sh";

    char *dir, *file = ACRONYM_ALIAS_FNAME, *home = getenv("HOME");

    // Set dir based on scope
    if (scope == GLOBAL) {
        dir = home;
    } else {
        char command_buf[strlen(find_alias_dir_cmd) + strlen(ACRONYM_ALIAS_FNAME)];
        char output_buf[PATH_MAX];
        sprintf(command_buf, find_alias_dir_cmd, ACRONYM_ALIAS_FNAME);
        FILE *fp = popen(command_buf, "r");
        dir = fgets(output_buf, sizeof(output_buf), fp);
        fclose(fp);
    }

    // Get the base name of the alias file
    int base_len = strlen(ACRONYM_ALIAS_FNAME);
    char local_buf[base_len + 6];
    if (base_len > 3 && !strncmp(ACRONYM_ALIAS_FNAME + base_len - 3, ".sh", 3))
        base_len -= 3;
    snprintf(local_buf, base_len + 6, "%.*s.local", base_len, ACRONYM_ALIAS_FNAME);

    // Change file name if local scope
    if (scope == LOCAL)
        file = local_buf;

    // Write to buffers
    snprintf(ALIAS_FILE, PATH_MAX, "%s/%s", dir, file);
    snprintf(OLD_ALIAS_FILE, PATH_MAX, "%s/%.*s.old", dir, base_len, ACRONYM_ALIAS_FNAME);
    sprintf(TMP_MISMATCHES_FILE, "%s/%s", home, ".acronym.tmp");
    sprintf(TMP_TOML_FILE, "%s/%s", home, ".acronym.toml");

    // If alias file doesn't exist already, then make it
    if (access(ALIAS_FILE, F_OK) == -1) {
        FILE *f = fopen(ALIAS_FILE, "w"); 
        fclose(f);
    }
}

void free_re_resources(pcre *re, pcre_extra *extras, FILE *f) {
    pcre_free(re);
    pcre_free_study(extras);
    fclose(f);
}
