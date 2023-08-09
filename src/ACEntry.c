#include <stdlib.h>
#include "ACEntry.h"

Status create_ACEntry(ACEntry **data_out, const char *command,
                      const char *alias_override, const char *section_override,
                      bool include_flags) {
    return SUCCESS;
}

// Returns the first word from 'command', allocating memory.
// "git add -A" -> "git"
char *create_section_name(const char *command) {
    return NULL;
}

// Returns an acronym for 'command', allocating memory.
// An acronym is one word whose letters are the first letter of every word in
// 'command' before the first optlon/flag, in order.
// If 'include_flags', then options and the first letter of flags are included,
// exluding their arguments, in order.
// "ex subcmd -asd 3 --flag", false -> "es"
// "ex subcmd -asd 3 --flag", true -> "esasdf"
char *create_alias_name(const char *command, bool include_flags) {
    return NULL;
}

// Returns the DJB2 hash of 'str' mod capacity.
int hash_alias(char *str, int capacity) {
    int hash = 5381;
    while (str) {
        hash = (hash << 5) + hash + *str;
        str++;
    }
    return (hash & 0xFFFFFFFF) % capacity;
}

void free_ACEntry(ACEntry *ac) {
    return;
}
