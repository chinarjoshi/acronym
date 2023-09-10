#include "subcmds.h"
#include "../hash_table/entry.h"
#include "../hash_table/hash_table.h"
#include "../file_io.h"

bool add_cmd(Cli *cli, HashTable *ht) {
    // Initialize structures and variables
    struct Add a = cli->cmd.add;
    Entry *entry;

    const char *alias_fname = (a.local) ? AUTOENV_FNAME : ALIAS_FNAME;
    // Open the correct alias file ('.env' if 'a.local', else '~/.aliases')
    FILE *alias_f = fopen(alias_fname, "w+");
    if (!alias_f)
        return cleanup("Error: aliases file cannot be opened: %s\n", alias_fname, ht, 0, 0);

    // Read aliases into hash table and write non-matching lines to tmp
    FILE *tmp_f = read_aliases(alias_f, ht);
    if (!tmp_f)
        return cleanup(0, 0, ht, alias_f, 0);

    // Create entry from command line args
    create_entry(&entry, a.command, a.alias_override, a.section_override, a.include_flags);

    // Add new entry to hash table and check for duplicate
    if (add_entry(entry, ht) == ERR_DUPLICATE) {
        if (cli->verbosity)
            printf("Duplicate alias: %s=\"%s\"\n", entry->alias, entry->command);
        free_entry(entry);
        return cleanup(0, 0, ht, tmp_f, TMP_FNAME);
    }

    // Write new aliases back to file and check for write permission
    if (!write_aliases(tmp_f, ht))
        return cleanup("Error: unable to write to alias file", 0, ht, tmp_f, TMP_FNAME);

    free_hash_table(ht);
    fclose(tmp_f);
    rename(TMP_FNAME, alias_fname);
    // Cleanup cli
    return true;
}
