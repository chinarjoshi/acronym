#include <stdlib.h>
#include "ACEntry.h"

Status create_ACEntry(ACEntry **data_out, const char *command,
                      const char *alias_override, const char *section_override,
                      bool include_flags) {
    return SUCCESS;
}

char *create_section_name(const char *command) {
    return NULL;
}

char *create_alias_name(const char *command, bool include_flags) {
    return NULL;
}

int hash_alias(char *alias, int capacity) {
    return 0;
}

void free_ACEntry(ACEntry *ac) {
    return;
}
