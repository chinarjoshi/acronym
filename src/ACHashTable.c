#include <stdlib.h>
#include "ACHashTable.h"

Status create_ACHashTable(ACHashTable **data_out, int capacity,
                          float load_factor) {
    return SUCCESS;
}

Status add_AC(ACEntry *data, ACHashTable *ht) {
    return SUCCESS;
}

Status remove_AC(ACEntry **data_out, char *alias, ACHashTable *ht) {
    return SUCCESS;
}

Status resize_backing_array(ACHashTable *ht) {
    return SUCCESS;
}

void free_ACHashTable(ACHashTable *ht) {
    return;
}
