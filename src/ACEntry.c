#include <stdlib.h>
#include "ACEntry.h"

Status create_ACEntry(ACEntry **data_out, const char *command,
                      const char *acronym_override, const char *set_override,
                      bool include_flags) {
    return SUCCESS;
}

char *create_set_name(const char *command) {
    return NULL;
}

char *create_acronym(const char *command, bool include_flags) {
    return NULL;
}

int hash_acronym(char *acronym, int capacity) {
    return 0;
}

void free_ACEntry(ACEntry *ac) {
    return;
}
