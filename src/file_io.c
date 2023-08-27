#include "file_io.h"
#include "entry.h"
#include "hash_table.h"
#include <pcre.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
const char *TMP_FNAME = "acronym_tmpfile";
const char *ALIAS_PATTERN = "^alias ([^=]+)=([^#\n]+)(?: ## ?([^\n]+))?$";
const char *FILE_DELIMITER = "# --- Aliases ---\n";

// Using a compiled regex, match 'text' and put results in 'alias', 'command', and 'section'.
// Returns false if it doesn't match, true if it does.
bool match_line(pcre *re, pcre_extra *extras, int *ovector, char *line, 
                       char *alias, char *command, char *section) {
    int rc = pcre_exec(re, extras, line, strlen(line), 0, 0, ovector, 8);
    if (rc == PCRE_ERROR_NOMATCH)
        return false;

    strncpy(alias, line + ovector[2], ovector[3] - ovector[2]);
    strncpy(command, line + ovector[4], ovector[5] - ovector[4]);

    if (ovector[6] != -1) {
        strncpy(section, line + ovector[6], ovector[7] - ovector[6]);
    } else {
        section[0] = '\0';
    }
        
    return true;
}

/* Reads a file handler with lines of the form "alias X=Y" (optionally "## Z"
 * at the end) into a hash table, where X is the key, and Y and Z are the values.
 * Prunes those lines from file, leaving all others alone. Closes the given 
 * file handler and returns a new one. Returns NULL if regex, file I/O, or 
 * memory allocation fails, and a valid handler otherwise.
 */
FILE *read_aliases(FILE *f, HashTable *ht) {
    char line[512], alias[64], command[256], section[64];
    int ovector[8];
    Entry *entry;
    // Compile and optimize the regex
    pcre *re = pcre_compile(ALIAS_PATTERN, 0, 0, 0, 0);
    pcre_extra *extras = pcre_study(re, 0, 0);
    if (!re || !extras) {
        printf("Error compiling regex: %s", ALIAS_PATTERN);
        return NULL;
    }
    // Open temporary file to append non-matching lines
    FILE *tmp = fopen(TMP_FNAME, "w");
    if (!tmp) {
        printf("Error opening file: %s", TMP_FNAME);
        return NULL;
    }

    // Read every line of the file. If the pattern doesn't match, append it to
    // the temporary file. Otherwise, add it to the hash table. At the end,
    // replace the original file with the pruned one.
    while (fgets(line, sizeof(line), f)) {
        if (match_line(re, extras, ovector, line, alias, command, section)) {
            if (create_entry(&entry, command, alias, section, false) == ERR_OUT_OF_MEMORY) {
                printf("Out of memory while creating entry.");
                fclose(tmp);
                return NULL;
            }
            add_entry(entry, ht);
        } else if (strcmp(line, FILE_DELIMITER)) {
            // Add every line except the above
            fputs(line, tmp);
        }
    }

    fclose(f);
    pcre_free(re);
    pcre_free_study(extras);
    rewind(tmp);
    return tmp;
}

// Given a hash table of aliases, write all elements to the given file stream.
bool write_aliases(FILE *f, HashTable *ht) {
    Entry *entry;
    fputs("\n", f);
    fputs(FILE_DELIMITER, f);
    for (int i = 0; i < ht->capacity; i++) {
        entry = ht->backing_array[i];
        if (entry && !entry->is_removed) {
            fprintf(f, "alias %s=%s ## %s\n", entry->alias, entry->command, entry->section);
        }
    }
    return true;
}
