#ifndef HASHTABLE_H
#define HASHTABLE_H
#include "entry.h"

typedef struct HashTable {
    Entry **backing_array;
    int capacity;
    int size;
    float load_factor;
} HashTable;

Status create_hash_table(HashTable **data_out, int capacity,
                          float load_factor);
Status add_entry(Entry *data, HashTable *ht);
Status remove_entry(Entry **data_out, char *alias, HashTable *ht);
Status resize_backing_array(HashTable *ht);
void free_hash_table(HashTable *ht);

#endif
