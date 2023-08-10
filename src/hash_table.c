#include <stdlib.h>
#include "hash_table.h"

Status create_hash_table(HashTable **data_out, int capacity,
                          float load_factor) {
    return SUCCESS;
}

Status add_entry(Entry *data, HashTable *ht) {
    return SUCCESS;
}

Status remove_entry(Entry **data_out, char *alias, HashTable *ht) {
    return SUCCESS;
}

Status resize_backing_array(HashTable *ht) {
    return SUCCESS;
}

void free_hash_table(HashTable *ht) {
    return;
}
