#include "subcmds.h"
#include "../hash_table/entry.h"
#include "../hash_table/hash_table.h"
#include "../file_io.h"
#include <stdlib.h>
#include <unistd.h>

bool remove_cmd(Cli *cli) {
    // Initialize structures and validate inputs
    struct Remove r = cli->cmd.remove;
    if (!r.aliases) {
        printf("No %s provided.\n", r.recursive ? "section" : "alias"); 
    }
    HashTable *ht;
    create_hash_table(&ht, INITIAL_CAPACITY, LOAD_FACTOR);
    Entry *entry;

    // Pick the alias file name ('.env' if 'a.local', else '~/.aliases')
    const char *alias_fname = (r.local) ? AUTOENV_FNAME : ALIAS_FNAME;
    // Make sure the alias file exists
    if (access(alias_fname, F_OK) == -1) {
        FILE *f = fopen(alias_fname, "w"); 
        fclose(f);
    }
    FILE *alias_f = fopen(alias_fname, "r");
    if (!alias_f)
        return cleanup("Error: aliases file not found: %s\n", alias_fname, ht, 0, 0);

    // Read aliases into hash table and write non-matching lines to tmp
    FILE *tmp_f = read_aliases(alias_f, ht);
    if (!tmp_f)
        return cleanup(0, 0, ht, alias_f, 0);

    // Delete given aliases or sections, prompting unless force is given
    while (r.aliases) {
        if (r.recursive) {
            if (!r.force) {
                char answer;
                printf("Delete section: \"%s\"? [y/N]\n", r.aliases->data);
                scanf(" %c", &answer);
                if (answer != 'y' && answer != 'Y')
                    continue;
            }
            if (remove_section(r.aliases->data, ht) == ERR_NOT_FOUND) {
                printf("Section not found: %s\n", r.aliases->data);
            } else if (cli->verbosity) {
                printf("Deleted section: %s\n", r.aliases->data);
            }
        } else {
            if (r.interactive) {
                char answer;
                printf("Delete: \"%s\"? [y/N]\n", r.aliases->data);
                scanf(" %c", &answer);
                if (answer != 'y' && answer != 'Y')
                    continue;
            }
            if (remove_entry(&entry, r.aliases->data, ht) == ERR_NOT_FOUND) {
                printf("Not found: \"%s\"\n", r.aliases->data);
            } else if (cli->verbosity) {
                printf("Deleted: %s=\"%s\"\n", entry->alias, entry->command);
            }
        }
        r.aliases = r.aliases->next;
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
