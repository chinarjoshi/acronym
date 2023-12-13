#include <stdbool.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "subcmds.h"
#include "../hash_table/hash_table.h"
#include "../file_io/file_io.h"
bool (*sub_cmds[])(Cli *) = {
    [ADD] = add_cmd,
    [REMOVE] = remove_cmd,
    [EDIT] = edit_cmd,
    [SHOW] = show_cmd,
    [UNDO] = undo_cmd,
};

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

bool include_aliases_file(char *path, HashTable *ht) {
    // Include local aliases, highlighted differently to differentiate them
    FILE *aliases_f = fopen(path, "r");
    if (!aliases_f) {
        perror("Error (file I/O): unable to open alias file: \"%s\".\n");
        return false;
    }
    read_aliases(aliases_f, ht, false);
    return true;
}
