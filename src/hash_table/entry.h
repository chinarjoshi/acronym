#ifndef ENTRY_H
#define ENTRY_H
#include <stdbool.h>

// Aliased command entry in hash table
typedef struct Entry {
    char *alias;   // key
    char *command; // value
    char *section; // cached value
    char *comment; // description
    bool is_removed;
} Entry;

typedef enum Status {
    SUCCESS,
    ERR_INVALID_ARGS,
    ERR_OUT_OF_MEMORY,
    ERR_NOT_FOUND,    // Remove
    ERR_DUPLICATE,    // Add
    ERR_INVALID_PATH, // Tree, Show
} Status;

Status create_entry(Entry **data_out, const char *command,
                    const char *alias_override, const char *section_override,
                    bool include_flags);
char *create_alias_name(const char *command, bool include_flags);
char *create_section_name(const char *command);
int hash_alias(char *alias, int capacity);
void free_entry(Entry *entry);

#endif
