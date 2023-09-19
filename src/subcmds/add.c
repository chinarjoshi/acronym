#include <sys/stat.h>
#include <unistd.h>
#include "subcmds.h"
#include "../hash_table/entry.h"
#include "../hash_table/hash_table.h"
#include "../file_io.h"

bool add_cmd(Cli *cli) {
    struct Add a = cli->cmd.add;
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
        return cleanup("Error (file I/O): aliases file cannot be opened: %s.\n", alias_fname, ht, 0, 0, cli);

    // Read aliases into hash table and write non-matching lines to tmp
    FILE *tmp_f = read_aliases(alias_f, ht);
    if (!tmp_f)
        return cleanup(0, 0, ht, alias_f, 0, cli);

    // Create entry from command line args
    create_entry(&entry, a.command, a.alias_override, a.section_override, a.include_flags);

    // Add new entry to hash table and check for duplicate
    if (add_entry(entry, ht) == ERR_DUPLICATE) {
        if (cli->verbosity)
            printf("Duplicate: %s=\"%s\"\n", entry->alias, entry->command);
        return cleanup(0, 0, ht, tmp_f, TMP_FNAME, cli);
    }

    // Write new aliases back to file and check for write permission
    if (!write_aliases(tmp_f, ht))
        return cleanup("Error (file I/O): unable to write to temporary alias file: \"%s\".", TMP_FNAME, ht, tmp_f, TMP_FNAME, cli);

    free_hash_table(ht);
    fclose(tmp_f);
    free_Cli(cli);
    if (rename(TMP_FNAME, alias_fname))
        return cleanup("Error (file I/O): cannot rename file.\n", 0, 0, 0, TMP_FNAME, 0);
    return true;
}
