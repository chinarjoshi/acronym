#ifndef ACHASHTABLE_H
#define ACHASHTABLE_H
#include "ACEntry.h"

typedef struct ACHashTable {
    ACEntry **backing_array;
    int capacity;
    int size;
    float load_factor;
} ACHashTable;

Status create_ACHashTable(ACHashTable **data_out, int capacity,
                          float load_factor);
Status resize_backing_array(ACHashTable *ht);
Status add_AC(ACEntry *data, ACHashTable *ht);
Status get_AC(ACEntry **data_out, char *alias, ACHashTable *ht);
Status remove_AC(ACEntry **data_out, char *alias, ACHashTable *ht);
void free_ACHashTable(ACHashTable *ht);

#endif
