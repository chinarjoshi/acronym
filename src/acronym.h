#ifndef ACRONYM_H
#define ACRONYM_H
#include <stdbool.h>

typedef struct ACEntry {
    char *acronym; // key
    char *command; // value
    char *set;     // value
    bool is_removed;
} ACEntry;

typedef struct ACHashTable {
    ACEntry **backing_array;
    int capacity;
    int size;
    float load_factor;
} ACHashTable;

typedef enum Status {
    SUCCESS,
    ERR_NOT_FOUND,
    ERR_DUPLICATE,
    ERR_OUT_OF_MEMORY,
    ERR_INVALID_ARGS,
} Status;

// AC helper functions
Status create_ACEntry(ACEntry **data_out, const char *command,
                      const char *acronym_override, const char *set_override,
                      bool include_flags);
char *create_set_name(const char *command);
char *create_acronym(const char *command, bool include_flags);
int hash_acronym(char *acronym, int capacity);
void free_ACEntry(ACEntry *ac);

// ACHashTable functions
Status create_ACHashTable(ACHashTable **data_out, int capacity,
                          float load_factor);
Status add_AC(ACEntry *data, ACHashTable *ht);
Status remove_AC(ACEntry **data_out, char *acronym, ACHashTable *ht);
void free_ACHashTable(ACHashTable *ht);

#endif
