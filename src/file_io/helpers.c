#include <stdio.h>
#include <pcre.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
char ACRONYM_DIRNAME[64];
char ALIAS_FNAME[78];
char TOML_FNAME[78];
char TMP_FNAME[78];
const char *AUTOENV_FNAME;

// Sets 'ALIAS_FNAME' to the value of environmental variable "ACRONYM_ALIAS_FILE",
// or if not found, the expansion of "~/.aliases"
// Sets 'AUTOENV_FNAME' to its env variable, or '.env' if not found
// (default: ".env"), otherwise returns the absolute path to ~/.aliases.
void setup_fname_buffers() {
    const char *acronym_dirname = getenv("ACRONYM_DIRECTORY");
    if (acronym_dirname && strlen(acronym_dirname) > 0) {
        snprintf(ACRONYM_DIRNAME, strlen(acronym_dirname), "%s", acronym_dirname);
    } else {
        snprintf(ACRONYM_DIRNAME, strlen(ACRONYM_DIRNAME), "%s/.acronym", getenv("HOME"));
    }

    snprintf(TMP_FNAME, sizeof(TMP_FNAME), "%s/.tmpfile", ACRONYM_DIRNAME);
    snprintf(TOML_FNAME, sizeof(TOML_FNAME), "%s/.tmpfile.toml", ACRONYM_DIRNAME);
    AUTOENV_FNAME = getenv("AUTOENV_ENV_FILENAME");
    if (!AUTOENV_FNAME)
        AUTOENV_FNAME = ".env";
}

// Make sure the acronym directory exists, and that the alias file exists in it
void ensure_alias_file_exists() {
    // Create acronym directory if it doesn't exist
    struct stat statbuf;
    if (stat(ACRONYM_DIRNAME, &statbuf) == -1 || !S_ISDIR(statbuf.st_mode)) {
        if (mkdir(ACRONYM_DIRNAME, 0755) == -1) {
            perror("mkdir");
            return;
        }
    }

    // Make sure the alias file exists
    if (access(ALIAS_FNAME, F_OK) == -1) {
        FILE *f = fopen(ALIAS_FNAME, "w"); 
        fclose(f);
    }
}

void free_re_resources(pcre *re, pcre_extra *extras, FILE *f) {
    pcre_free(re);
    pcre_free_study(extras);
    fclose(f);
}
