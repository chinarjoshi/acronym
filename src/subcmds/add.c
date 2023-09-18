#include <sys/stat.h>
#include <unistd.h>
#include "subcmds.h"
#include "../hash_table/entry.h"
#include "../hash_table/hash_table.h"
#include "../file_io.h"

bool add_cmd(Cli *cli) {
    // Initialize structures and validate inputs
    struct Add a = cli->cmd.add;
    if (!a.command) {
        printf("Error invalid args: must provide command to alias.\n");
        return false;
    }
    HashTable *ht;
    create_hash_table(&ht, INITIAL_CAPACITY, LOAD_FACTOR);
    Entry *entry;

    // Pick the alias file name ('.env' if 'a.local', else '~/.aliases')
    const char *alias_fname = (a.local) ? AUTOENV_FNAME : ALIAS_FNAME;
    // Make sure the alias file exists
    if (access(alias_fname, F_OK) == -1) {
        FILE *f = fopen(alias_fname, "w"); 
        fclose(f);
    }
    FILE *alias_f = fopen(alias_fname, "r");
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
            printf("Duplicate: %s=\"%s\"\n", entry->alias, entry->command);
        return cleanup(0, 0, ht, tmp_f, TMP_FNAME);
    }

    // Write new aliases back to file and check for write permission
    if (!write_aliases(tmp_f, ht))
        return cleanup("Error: unable to write to alias file", 0, ht, tmp_f, TMP_FNAME);

    free_hash_table(ht);
    fclose(tmp_f);
    if (rename(TMP_FNAME, alias_fname)) {
        perror("Error renaming file.\n");
        return false;
    }
    return true;
}
