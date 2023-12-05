#include "file_io.h"
#include "../hash_table/entry.h"
#include "../hash_table/hash_table.h"
#include <pcre.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
const char *ALIAS_PATTERN = "^alias\\s+([^=]+)=['\"]?([^#\\n]+)(?:\\s+## ?([^\\n]+?))?\\n";
const char *FILE_DELIMITER = "# --- Aliases ---\n";
const int OVECTOR_LEN = 30;

// Using a compiled regex, match 'text' and put results in 'alias', 'command', and 'section'.
// Returns false if it doesn't match, true if it does.
bool match_line(pcre *re, pcre_extra *extras, int *ovector, char *line, 
                       char *alias, char *command, char *section) {
    int rc = pcre_exec(re, extras, line, strlen(line), 0, 0, ovector, OVECTOR_LEN);
    if (rc == PCRE_ERROR_NOMATCH)
        return false;

    // Extract alias
    int alias_length = ovector[3] - ovector[2];
    strncpy(alias, line + ovector[2], alias_length);
    alias[alias_length] = '\0';

    // Extract command
    int command_len = ovector[5] - ovector[4];
    if (line[ovector[5] - 1] == '"' || line[ovector[5] - 1] == '\'')
        command_len--;
    strncpy(command, line + ovector[4], command_len);
    command[command_len] = '\0';

    // Extract section
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
 * file handler and returns a new one. Returns NULL if not output_non_matches,
 * regex, file I/O, or memory allocation fails, and a valid handler otherwise.
 */
FILE *read_aliases(FILE *f, HashTable *ht, bool output_non_matches) {
    char line[512], alias[64], command[256], section[64];
    int ovector[OVECTOR_LEN];
    Entry *entry;
    // Compile and optimize the regex
    const char *error;
    int erroffset;
    pcre *re = pcre_compile(ALIAS_PATTERN, 0, &error, &erroffset, NULL);
    pcre_extra *extras = pcre_study(re, 0, &error);
    if (!re || !extras) {
        printf("Error (regex): failure compiling regex: \"%s\".\n", ALIAS_PATTERN);
        return NULL;
    }
    // Open temporary file to append non-matching lines
    FILE *tmp_f = NULL;
    if (output_non_matches) {
        tmp_f = fopen(TMP_MISMATCHES_PATH, "w+");
        if (!tmp_f) {
            printf("Error (file I/O): cannot open file: \"%s\".", TMP_MISMATCHES_PATH);
            free_re_resources(re, extras, f);
            return NULL;
        }
    }

    // Read every line of the file. If the pattern doesn't match, append it to
    // the temporary file. Otherwise, add it to the hash table. At the end,
    // replace the original file with the pruned one.
    bool found_non_matching_line = false;
    char comment[sizeof(line) * 4];
    while (fgets(line, sizeof(line), f)) {
        if (match_line(re, extras, ovector, line, alias, command, section)) {
            if (create_entry(&entry, command, alias, section, comment, false) == ERR_OUT_OF_MEMORY) {
                printf("Error (memory): out of memory while creating entry.\n");
                free_re_resources(re, extras, tmp_f);
                fclose(f);
                if (output_non_matches)
                    remove(TMP_MISMATCHES_PATH);
                return NULL;
            }
            add_entry(entry, ht);
        } else if (line[0] == '#') {
            // Save the value if its a comment
            strncat(comment, line, sizeof(comment) - strlen(comment) - 1);
            continue;
        } else if (output_non_matches && line[0] != '\n' && strcmp(line, FILE_DELIMITER) != 0) {
            // Add every line except new line and delimiter
            fputs(line, tmp_f);
            found_non_matching_line = true;
        }
        comment[0] = '\0';
    }

    if (output_non_matches && found_non_matching_line) {
        fputs("\n", tmp_f);
        fputs(FILE_DELIMITER, tmp_f);
    }
    free_re_resources(re, extras, f);
    return tmp_f;
}

// Given a hash table of aliases, write all elements to the given file stream.
// Do two passes over the hash table:
// 1. Write all entries without a comment first compactly
// 2. Write remaining entries sandwiched between whitespace
bool write_aliases(FILE *f, HashTable *ht) {
    if (ht->size == 0)
        return true;

    Entry *entry;
    for (int i = 0; i < ht->capacity; i++) {
        entry = ht->backing_array[i];
        if (entry && !entry->is_removed && !entry->comment) {
            fprintf(f, "alias %s='%s' ## %s\n", entry->alias, entry->command, entry->section);
        }
    }
    for (int i = 0; i < ht->capacity; i++) {
        entry = ht->backing_array[i];
        if (entry && !entry->is_removed && entry->comment) {
            fprintf(f, "\n%s\n", entry->comment);
            fprintf(f, "alias %s='%s' ## %s\n", entry->alias, entry->command, entry->section);
        }
    }
    return true;
}
