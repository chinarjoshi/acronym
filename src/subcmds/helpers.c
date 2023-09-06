#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Returns whether 'child_path' is a subdirectory of 'parent_path'. If 'reverse', return
// the inverse.
bool is_child_path(const char *child_path, const char *parent_path, bool reverse) {
    if (strlen(child_path) < strlen(parent_path))
        return false;

    int len = strlen((reverse) ? parent_path : child_path);
    return strncmp(child_path, parent_path, len + 1) == 0;
}

// Returns an array of .env paths that are children or parents of 'start', according to 
// 'return_parents', in the form of 'num_paths' char pointers that must be freed.
char **get_env_paths(const char *start, bool return_parents, int *num_paths) {
    const char *auth_fname = getenv("AUTOENV_AUTH_FILE");
    FILE *auth_f = fopen(auth_fname, "r");
    if (!auth_f) {
        fprintf(stderr, "Error opening autoenv auth file: %s\n", auth_fname);
        return NULL;
    }

    char line[256];
    int max_paths = 15, path_count = 0;
    char** paths = malloc(max_paths * sizeof(char *));

    while (fgets(line, sizeof(line), auth_f)) {
        char* path = strtok(line, ":");
        if (path && is_child_path(path, start, return_parents)) {
            path_count++;
            if (path_count > max_paths) {
                max_paths *= 2;
                paths = realloc(paths, max_paths * sizeof(char *));
            }
            paths[path_count - 1] = strdup(path);
        }
    }

    fclose(auth_f);

    *num_paths = path_count;
    return paths;
}
