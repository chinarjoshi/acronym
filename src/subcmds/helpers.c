#include <stdbool.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "subcmds.h"
#include "../hash_table/hash_table.h"
static void insert_path(const char *path, char **path_arr, int num_paths);
bool (*sub_cmds[])(Cli *) = {
    [ADD] = add_cmd,
    [REMOVE] = remove_cmd,
    [EDIT] = edit_cmd,
    [SHOW] = show_cmd,
    [UNDO] = undo_cmd,
};

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

void free_env_paths(char **paths, int num_paths) {
    for (int i = 0; i < num_paths; ++i) {
        free(paths[i]);
    }
    free(paths);
}

// Bold the longest matching substring in 'data' from list 'l'
// Returns true if a substring was found, else false
static bool bold_longest_substring(char **data_ptr, AliasListNode *l) {
    char *data = *data_ptr;
    int longest_length = 0;
    char *longest_pos = NULL;
    char *longest_substring = NULL;
    
    for (struct AliasListNode *node = l; node; node = node->next) {
        char *pos = strstr(data, node->data);
        if (pos) {
            int length = strlen(node->data);
            if (length > longest_length) {
                longest_length = length;
                longest_pos = pos;
                longest_substring = node->data;
            }
        }
    }
    
    if (longest_substring) {
        size_t new_size = strlen(data) + 4 + 5 + 1; // ANSI codes + '\0'
        char *new_str = malloc(new_size);

        snprintf(new_str, new_size, "%.*s\033[1m%s\033[22m%s",
                 (int)(longest_pos - data), data,
                 longest_substring, longest_pos + longest_length);

        free(data);
        *data_ptr = new_str;
        return true;
    }
    
    return false;
}

// Removes entries of 'ht' of which no string in 'l' is a substring
// of the command field, or alias field if 'alias', or the section field if 'section'.
// Bolds matching text.
void filter_hash_table(HashTable *ht, AliasListNode *l, bool alias, bool section) {
    for (int i = 0; i < ht->capacity; i++) {
        Entry *entry = ht->backing_array[i];
        if (!entry || entry->is_removed) {
            continue;
        }

        char **data_ptr = &entry->command;
        if (alias) data_ptr = &entry->alias;
        else if (section) data_ptr = &entry->section;

        if (!bold_longest_substring(data_ptr, l)) {
            entry->is_removed = true;
            ht->size--;
        }
    }
}
