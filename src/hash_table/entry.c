#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "entry.h"

// Allocates an Entry and saves it in *data_out, returning the status.
// If 'alias_override' is a valid string, then the 'alias' field will contain it
// Same with 'section_override'
// If 'include_flags', then options and the first letter of flags are included,
// exluding their arguments, in order.
Status create_entry(Entry **data_out, const char *command,
                      const char *alias_override, const char *section_override,
                      bool include_flags) {
    Entry *entry = malloc(sizeof(Entry));
    if (!entry)
        return ERR_OUT_OF_MEMORY;

    // Fill the 'command' field.
    entry->command = malloc(strlen(command) + 1);
    if (!entry->command)
        return ERR_OUT_OF_MEMORY;
    strcpy(entry->command, command);

    // If 'alias_override' is given, then use it for 'alias' field. Otherwise, generate it.
    if (!alias_override)
        alias_override = create_alias_name(command, include_flags);
    entry->alias = malloc(strlen(alias_override) + 1);
    strcpy(entry->alias, alias_override);

    // Same thing with the section name.
    if (!section_override || section_override[0] == '\0')
        section_override = create_section_name(command);
    entry->section = malloc(strlen(section_override) + 1);
    strcpy(entry->section, section_override);
    entry->is_removed = false;

    *data_out = entry;
    return SUCCESS;
}

// Returns the first word from 'command' that is not "sudo", allocating memory.
// "git add -A" -> "git"
// "sudo rm -rf ." -> "rm"
char *create_section_name(const char *command) {
    // If the first word is sudo, then skip first word
    if (strlen(command) > 4 && strncmp(command, "sudo", 4) == 0)
        command += 5;

    // Find the number of chars in the first word.
    int section_length = 0;
    for (const char *ptr = command; *ptr && *ptr != ' '; ptr++) {
        section_length++;
    }

    // Allocate enough memory and copy characters
    char *section = malloc(section_length + 1);
    if (!section)
        return NULL;
    strncpy(section, command, section_length);
    section[section_length] = '\0';

    return section;
}

// Returns an acronym for 'command', allocating memory.
// An acronym is one word whose letters are the first letter of every word in
// 'command' before the first optlon/flag, in order.
// If 'include_flags', then options and the first letter of flags are included,
// exluding their arguments, in order.
// "ex subcmd -asd 3 --flag", false -> "es"
// "ex  subcmd  -asd 3 --flag", true -> "esasdf"
char *create_alias_name(const char *command, bool include_flags) {
    // If the first word is sudo, then skip first word
    if (strlen(command) > 4 && strncmp(command, "sudo", 4) == 0)
        command += 5;

    char *alias = malloc(strlen(command) + 1);
    if (!alias)
        return NULL;

    const char *command_ptr = command;
    char *alias_ptr = alias;
    *alias_ptr++ = *command_ptr;
    // If the current letter is space, then if next letter is a character, add it
    while (*command_ptr && *command_ptr != '-') {
        if (*command_ptr == ' ' && isalpha(command_ptr[1])) {
            *alias_ptr++ = command_ptr[1];
        }
        command_ptr++;
    }
    // If string has not terminated and include_flags, 
    // then continue to count every flag and first letter of options
    if (include_flags) {
        bool in_option = false;
        while (*command_ptr) {
            // If inside a compound option, make sure its still true
            if (in_option) {
                if (isalpha(*command_ptr)) {
                    *alias_ptr++ = *command_ptr;
                } else {
                    in_option = false;
                }
            } else if (command_ptr[0] == '-') {
                // If current letter and next letter are both dashes, then add the letter after both
                if (command_ptr[1] == '-' && isalpha(command_ptr[2])) {
                    *alias_ptr++ = command_ptr[2];
                    command_ptr += 3;
                    continue;
                } else if (isalpha(command_ptr[1])) {
                    in_option = true;
                }
            }
            command_ptr++;
        } 
    }
    *alias_ptr = '\0';
    return alias;
}

// Returns the DJB2 hash of 'str' mod capacity.
int hash_alias(char *str, int capacity) {
    int hash = 5381;
    while (*str) {
        hash = (hash << 5) + hash + *str;
        str++;
    }
    return (hash & 0xFFFFFFFF) % capacity;
}

void free_entry(Entry *entry) {
    free(entry->alias);
    free(entry->section);
    free(entry->command);
    free(entry);
}
