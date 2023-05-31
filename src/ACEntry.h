#ifndef ACENTRY_H
#define ACENTRY_H
#include <stdbool.h>

// Aliased command entry in hash table
typedef struct ACEntry {
    char *alias;   // key
    char *command; // value
    char *section; // cached value
    bool is_removed;
} ACEntry;

typedef enum Status {
    SUCCESS,
    ERR_NOT_FOUND,
    ERR_DUPLICATE,
    ERR_OUT_OF_MEMORY,
    ERR_INVALID_ARGS,
} Status;

Status create_ACEntry(ACEntry **data_out, const char *command,
                      const char *alias_override, const char *section_override,
                      bool include_flags);
char *create_alias_name(const char *command, bool include_flags);
char *create_section_name(const char *command);
int hash_alias(char *alias, int capacity);
void free_ACEntry(ACEntry *ac);

#endif
