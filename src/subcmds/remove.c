#include "subcmds.h"
#include "../hash_table/entry.h"
#include "../hash_table/hash_table.h"
#include "../file_io/file_io.h"
#include <stdlib.h>
#include <unistd.h>

bool remove_cmd(Cli *cli) {
    struct Remove r = cli->cmd.remove;
    HashTable *ht;
    create_hash_table(&ht, INITIAL_CAPACITY, LOAD_FACTOR);
    Entry *entry;

    // Pick the alias file name ('.env' if 'a.local', else '~/.aliases')
    const char *alias_fname = (r.local) ? AUTOENV_FNAME : ALIAS_FNAME;
    FILE *alias_f = fopen(alias_fname, "r");
    if (!alias_f)
        return cleanup("Error (file I/O): aliases file not found: \"%s\".\n", alias_fname, ht, 0, 0);

    // Read aliases into hash table and write non-matching lines to tmp
    FILE *tmp_f = read_aliases(alias_f, ht, true);
    if (!tmp_f)
        return cleanup(0, 0, ht, alias_f, 0);

    // Delete given aliases or sections, prompting unless force is given
    bool something_removed = false;
    while (r.aliases) {
        if (r.section) {
            if (!r.force) {
                char answer;
                printf("Delete section: \033[32m\"%s\"\033[0m? [y/N]: ", r.aliases->data);
                scanf(" %c", &answer);
                if (answer != 'y' && answer != 'Y')
                    continue;
            }
            if (remove_section(r.aliases->data, ht) == ERR_NOT_FOUND) {
                printf("Section not found: \033[32m\"%s\"\033[0m\n", r.aliases->data);
            } else {
                if (cli->verbosity)
                    printf("Deleted section: \033[32m\"%s\"\033[0m\n", r.aliases->data);
                something_removed = true;
            }
        } else {
            if (r.interactive) {
                char answer;
                printf("Delete: \033[32m\"%s\"\033[0m? [y/N]: ", r.aliases->data);
                scanf(" %c", &answer);
                if (answer != 'y' && answer != 'Y')
                    continue;
            }
            if (remove_entry(&entry, r.aliases->data, ht) == ERR_NOT_FOUND) {
                printf("Not found: \033[32m\"%s\"\033[0m\n", r.aliases->data);
            } else {
                if (cli->verbosity)
                    printf("Deleted: %s \033[34m= \033[32m\"%s\"\033[0m (\033[33m%s\033[0m)\n", 
                           entry->alias, entry->command, entry->section);
                something_removed = true;
            }
        }
        r.aliases = r.aliases->next;
    }
    if (!something_removed)
        return cleanup(0, 0, ht, tmp_f, TMP_FNAME);

    // Write new aliases back to file and check for write permission
    if (!write_aliases(tmp_f, ht))
        return cleanup("Error (file I/O): unable to write to temporary alias file: \"%s\".\n", TMP_FNAME, ht, tmp_f, TMP_FNAME);

    free_hash_table(ht);
    fclose(tmp_f);
    if (rename(TMP_FNAME, alias_fname))
        return cleanup("Error (file I/O): cannot rename file.\n", 0, 0, 0, TMP_FNAME);
    return true;
}
