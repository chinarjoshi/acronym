#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include "subcmds.h"
#include "../hash_table/hash_table.h"
static void insert_path(const char *path, char **path_arr, int num_paths);
char ALIAS_FNAME[64];
char TOML_FNAME[64];
char TMP_FNAME[64];
const char *AUTOENV_FNAME;

bool (*sub_cmds[])(Cli *) = {
    [ADD] = add_cmd,
    [REMOVE] = remove_cmd,
    [EDIT] = edit_cmd,
    [SHOW] = show_cmd,
};

bool is_valid_dir(const char *dir) {
    struct stat statbuf;
    return !stat(dir, &statbuf) && S_ISDIR(statbuf.st_mode);
}

enum PathCmp compare_paths(const char *env_fname, const char *directory) {
    int env_fname_len = strlen(env_fname) - 5;
    int directory_len = strlen(directory);

    int cmp_env_fname_len = !strncmp(env_fname, directory, env_fname_len);
    int cmp_directory_len = !strncmp(env_fname, directory, directory_len);

    if (env_fname_len == directory_len && cmp_env_fname_len) {
        return PATH_EQ;
    } else if (env_fname_len < directory_len && cmp_env_fname_len) {
        return PATH_CHILD;
    } else if (env_fname_len > directory_len && cmp_directory_len) {
        return PATH_PARENT;
    }

    return PATH_UNRELATED;
}

// Returns an array of .env paths that are children or parents of 'start', according to 
// 'return_parents', in the form of 'num_paths' char pointers that must be freed.
// The array is ordered from shortest to longest path.
char **get_env_paths(const char *start, int *num_paths) {
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
        enum PathCmp pcmp = compare_paths(path, start);
        if (path && (pcmp == PATH_EQ || pcmp == PATH_CHILD)) {
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

// Sets 'ALIAS_FNAME' to the value of environmental variable "ACRONYM_ALIAS_FILE",
// or if not found, the expansion of "~/.aliases"
// Sets 'AUTOENV_FNAME' to its env variable, or '.env' if not found
// (default: ".env"), otherwise returns the absolute path to ~/.aliases.
void setup_fname_buffers() {
    const char *alias_fname = getenv("ACRONYM_DIRECTORY");
    if (alias_fname && strlen(alias_fname) > 0) {
        snprintf(ALIAS_FNAME, sizeof(ALIAS_FNAME), "%s", alias_fname);
    } else {
        snprintf(ALIAS_FNAME, sizeof(ALIAS_FNAME), "%s/.aliases", getenv("HOME"));
    }
    snprintf(TMP_FNAME, sizeof(TMP_FNAME), "%s/.acronym_tmpfile", getenv("HOME"));
    snprintf(TOML_FNAME, sizeof(TOML_FNAME), "%s/.acronym_tmpfile.toml", getenv("HOME"));

    AUTOENV_FNAME = getenv("AUTOENV_ENV_FILENAME");
    if (!AUTOENV_FNAME)
        AUTOENV_FNAME = ".env";
}
