#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include "subcmds.h"
#include "../hash_table/hash_table.h"
char ALIAS_FNAME[64];
char TOML_FNAME[64];
char TMP_FNAME[64];
const char *AUTOENV_FNAME;
static void insert_path(const char *path, char **path_arr, int num_paths);

bool is_valid_dir(const char *dir) {
    struct stat statbuf;
    return !stat(dir, &statbuf) && S_ISDIR(statbuf.st_mode);
}

// Returns whether 'child_path' is a subdirectory of 'parent_path'. If 'reverse', return
// the inverse.
bool is_child_path(const char *child_path, const char *parent_path, bool reverse) {
    if (strlen(child_path) > strlen(parent_path))
        return false;

    int len = strlen(reverse ? parent_path : child_path);
    // Compare length - 6 characters of the paths (-5 because of /.env at the end)
    return strncmp(child_path, parent_path, len - 5) == 0;
}

// Returns an array of .env paths that are children or parents of 'start', according to 
// 'return_parents', in the form of 'num_paths' char pointers that must be freed.
// The array is ordered from shortest to longest path.
char **get_env_paths(const char *start, int *num_paths, bool return_parents) {
    const char *auth_fname = getenv("AUTOENV_AUTH_FILE");
    FILE *auth_f = fopen(auth_fname, "r");
    if (!auth_f) {
        fprintf(stderr, "Error (file I/O): cannot open autoenv auth file: \"%s\".\n", auth_fname);
        return NULL;
    }

    char line[PATH_MAX];
    int max_paths = 32, path_count = 0;
    char** paths = malloc(max_paths * sizeof(char *));

    while (fgets(line, sizeof(line), auth_f)) {
        char* path = strtok(line, ":");
        if (path && is_child_path(path, start, return_parents)) {
            if (path_count + 1 > max_paths) {
                max_paths *= 2;
                paths = realloc(paths, max_paths * sizeof(char *));
            }
            insert_path(path, paths, path_count);
            path_count++;
        }
    }

    fclose(auth_f);

    *num_paths = path_count;
    return paths;
}

// Insert 'path' into 'path_arr', which is length 'num_paths' and ascending in length.
static void insert_path(const char *path, char **path_arr, int num_paths) {
    int path_len = strlen(path);
    int i;

    // Find the position to insert the new path
    for (i = num_paths - 1; i >= 0; --i) {
        if (strlen(path_arr[i]) <= path_len) {
            break;
        }
        // Shift elements to make room for the new path
        path_arr[i + 1] = path_arr[i];
    }
    
    // Insert the new path
    path_arr[i + 1] = strdup(path);
}

int cleanup(const char *message, const char *message_arg, 
                          HashTable *ht, FILE *f, const char *fname_to_remove) {
    if (message) {
        if (message_arg)
            fprintf(stderr, message, message_arg); 
        else
            fprintf(stderr, "%s", message); 
    }
    free_hash_table(ht);
    fclose(f);
    remove(fname_to_remove);
    return 0;
}

// Sets 'ALIAS_FNAME' to expanded version of "~/.aliases"
// Sets 'AUTOENV_FNAME' to its env variable, or '.env' if not found
// (default: ".env"), otherwise returns the absolute path to ~/.aliases.
void set_alias_and_autoenv_fnames() {
    snprintf(ALIAS_FNAME, sizeof(ALIAS_FNAME), "%s/.aliases", getenv("HOME"));
    snprintf(TMP_FNAME, sizeof(TMP_FNAME), "%s/.acronym_tmpfile", getenv("HOME"));
    snprintf(TOML_FNAME, sizeof(TOML_FNAME), "%s/.acronym_tmpfile.toml", getenv("HOME"));

    AUTOENV_FNAME = getenv("AUTOENV_ENV_FILENAME");
    if (!AUTOENV_FNAME)
        AUTOENV_FNAME = ".env";
}
