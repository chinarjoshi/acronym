#ifndef ACENTRY_H
#define ACENTRY_H
#include <stdbool.h>

typedef struct ACEntry {
    char *acronym; // key
    char *command; // value
    char *set;     // value
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
                      const char *acronym_override, const char *set_override,
                      bool include_flags);
char *create_set_name(const char *command);
char *create_acronym(const char *command, bool include_flags);
int hash_acronym(char *acronym, int capacity);
void free_ACEntry(ACEntry *ac);

#endif
