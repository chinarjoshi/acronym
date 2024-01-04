#include "subcmds.h"
#include "../hash_table/entry.h"
#include "../hash_table/hash_table.h"
#include "../file_io/file_io.h"

bool create_cmd(Cli *cli) {
    struct Create c = cli->cmd.create;
    HashTable *ht;
    create_hash_table(&ht, INITIAL_CAPACITY, LOAD_FACTOR);
    Entry *entry;

    // Open the aliases file
    FILE *alias_f = fopen(ALIASES_PATH, "r");
    if (!alias_f)
        return cleanup("Error (file I/O): aliases file cannot be opened: %s.\n", ALIASES_PATH, ht, 0, 0);

    // Read aliases into hash table and write non-matching lines to tmp
    FILE *tmp_f = read_aliases(alias_f, ht, true, 0);
    if (!tmp_f)
        return cleanup(0, 0, ht, alias_f, 0);

    // Create entry from command line args
    create_entry(&entry, c.command, c.alias_override, c.section_override, c.comment, 0, c.include_flags);

    // Add new entry to hash table and check for duplicate
    if (add_entry(entry, ht) == ERR_DUPLICATE) {
        if (cli->verbosity)
            printf("Duplicate: \033[31m%s\033[0m \033[34m= \033[31m\"%s\"\033[0m (\033[33m%s\033[0m) in \033[36m%s\033[0m\n",
                  entry->alias, entry->command, entry->section, replace_home_with_tilde(ALIASES_PATH, PATH_BUFFER));
        return cleanup(0, 0, ht, tmp_f, TMP_MISMATCHES_PATH);
    }

    // Write new aliases back to file and check for write permission
    if (!write_aliases(tmp_f, ht))
        return cleanup("Error (file I/O): unable to write to temporary alias file: \"%s\".", 
                       TMP_MISMATCHES_PATH, ht, tmp_f, TMP_MISMATCHES_PATH);

    if (rename(TMP_MISMATCHES_PATH, ALIASES_PATH))
        return cleanup("Error (file I/O): cannot override aliases.\n", 0, ht, tmp_f, TMP_MISMATCHES_PATH);

    printf("Added: %s \033[34m= \033[32m\"%s\"\033[0m (\033[33m%s\033[0m) to \033[36m%s\033[0m\n", 
           entry->alias, entry->command, entry->section, replace_home_with_tilde(ALIASES_PATH, PATH_BUFFER));
    free_hash_table(ht);
    fclose(tmp_f);
    return true;
}
