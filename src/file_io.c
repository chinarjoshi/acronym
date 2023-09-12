#include "file_io.h"
#include "hash_table/entry.h"
#include "hash_table/hash_table.h"
#include <pcre.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
const char *ALIAS_PATTERN = "^alias\\s+([^=]+)=['\"]?([^#\\n]+)(?:\\s+## ?([^\\n]+?))?\\n";
const char *FILE_DELIMITER = "# --- Aliases ---\n";
const int OVECTOR_LEN = 30;
static void free_re_resources(pcre *re, pcre_extra *extras, FILE *f);

// Using a compiled regex, match 'text' and put results in 'alias', 'command', and 'section'.
// Returns false if it doesn't match, true if it does.
bool match_line(pcre *re, pcre_extra *extras, int *ovector, char *line, 
                       char *alias, char *command, char *section) {
    int rc = pcre_exec(re, extras, line, strlen(line), 0, 0, ovector, OVECTOR_LEN);
    if (rc == PCRE_ERROR_NOMATCH)
        return false;

    int alias_length = ovector[3] - ovector[2];
    strncpy(alias, line + ovector[2], alias_length);
    alias[alias_length] = '\0';

    int command_len = ovector[5] - ovector[4];
    strncpy(command, line + ovector[4], command_len);
    while (command[command_len - 1] == '\'' || command[command_len - 1] == '"' 
            || command[command_len - 1] == ' ') {
        command_len--;
    }
    command[command_len] = '\0';

    if (ovector[6] != -1) {
        int section_len = ovector[7] - ovector[6];
        strncpy(section, line + ovector[6], section_len);
        while (section[section_len - 1] == ' ') {
            section_len--;
        }
        section[section_len] = '\0';
    } else {
        section[0] = '\0';
    }
        
    return true;
}

/* Reads a file handler with lines of the form "alias X=Y" (optionally "## Z"
 * at the end) into a hash table, where X is the key, and Y and Z are the values.
 * Prunes those lines from file, leaving all others alone. *Closes the given 
 * file handler and returns a new one. Returns NULL if regex, file I/O, or 
 * memory allocation fails, and a valid handler otherwise.
 */
FILE *read_aliases(FILE *f, HashTable *ht) {
    char line[512], alias[64], command[256], section[64];
    int ovector[OVECTOR_LEN];
    Entry *entry;
    // Compile and optimize the regex
    const char *error;
    int erroffset;
    pcre *re = pcre_compile(ALIAS_PATTERN, 0, &error, &erroffset, NULL);
    pcre_extra *extras = pcre_study(re, 0, &error);
    if (!re || !extras) {
        printf("Error: compiling regex: %s", ALIAS_PATTERN);
        return NULL;
    }
    // Open temporary file to append non-matching lines
    FILE *tmp = fopen(TMP_FNAME, "w+");
    if (!tmp) {
        printf("Error: opening file: %s", TMP_FNAME);
        free_re_resources(re, extras, f);
        return NULL;
    }

    // Read every line of the file. If the pattern doesn't match, append it to
    // the temporary file. Otherwise, add it to the hash table. At the end,
    // replace the original file with the pruned one.
    while (fgets(line, sizeof(line), f)) {
        if (match_line(re, extras, ovector, line, alias, command, section)) {
            if (create_entry(&entry, command, alias, section, false) == ERR_OUT_OF_MEMORY) {
                printf("Error: out of memory while creating entry.");
                free_re_resources(re, extras, tmp);
                fclose(f);
                remove(TMP_FNAME);
                return NULL;
            }
            add_entry(entry, ht);
        } else if (line[0] != '\n' && strcmp(line, FILE_DELIMITER) != 0) {
            // Add every line except the above
            fputs(line, tmp);
        }
    }

    fputs("\n", tmp);
    free_re_resources(re, extras, f);
    return tmp;
}

// Given a hash table of aliases, write all elements to the given file stream.
bool write_aliases(FILE *f, HashTable *ht) {
    if (ht->size == 0)
        return true;

    Entry *entry;
    if (!fputs(FILE_DELIMITER, f)) {
        printf("Error: unable to write to file");
        return false;
    }
    for (int i = 0; i < ht->capacity; i++) {
        entry = ht->backing_array[i];
        if (entry && !entry->is_removed) {
            fprintf(f, "alias %s='%s' ## %s\n", entry->alias, entry->command, entry->section);
        }
    }
    return true;
}

static void free_re_resources(pcre *re, pcre_extra *extras, FILE *f) {
    pcre_free(re);
    pcre_free_study(extras);
    fclose(f);
}
