#include "subcmds.h"
#include "../hash_table/entry.h"
#include "../hash_table/hash_table.h"
#include "../file_io/file_io.h"
#include <stdlib.h>
#include <unistd.h>

bool delete_cmd(Cli *cli) {
    struct Delete d = cli->cmd.delete;
    HashTable *ht;
    create_hash_table(&ht, INITIAL_CAPACITY, LOAD_FACTOR);
    Entry *entry;

    // Open the aliases file
    FILE *alias_f = fopen(ALIASES_PATH, "r");
    if (!alias_f)
        return cleanup("Error (file I/O): aliases file cannot be opened: %s.\n", replace_home_with_tilde(ALIASES_PATH, PATH_BUFFER), ht, 0, 0);

    // Read aliases into hash table and write non-matching lines to tmp
    FILE *tmp_f = read_aliases(alias_f, ht, true, 0);
    if (!tmp_f)
        return cleanup(0, 0, ht, alias_f, 0);

    // Delete given aliases or sections, prompting unless force is given
    bool something_removed = false;
    while (d.aliases) {
        if (d.section) {
            if (!d.force) {
                char answer;
                printf("Delete section: \033[32m\"%s\"\033[0m from \033[36m%s\033[0m? [y/N]: ", d.aliases->data, replace_home_with_tilde(ALIASES_PATH, PATH_BUFFER));
                scanf(" %c", &answer);
                if (answer != 'y' && answer != 'Y')
                    continue;
            }
            if (remove_section(d.aliases->data, ht) == ERR_NOT_FOUND) {
                printf("Section not found: \033[31m\"%s\"\033[0m in \033[36m%s\033[0m\n", d.aliases->data, replace_home_with_tilde(ALIASES_PATH, PATH_BUFFER));
            } else {
                if (cli->verbosity)
                    printf("Deleted section: \033[32m\"%s\"\033[0m from \033[36m%s\033[0m\n", d.aliases->data, replace_home_with_tilde(ALIASES_PATH, PATH_BUFFER));
                something_removed = true;
            }
        } else {
            if (d.interactive) {
                char answer;
                printf("Delete: \033[32m\"%s\"\033[0m from \033[36m%s\033[0m? [y/N]: ", d.aliases->data, replace_home_with_tilde(ALIASES_PATH, PATH_BUFFER));
                scanf(" %c", &answer);
                if (answer != 'y' && answer != 'Y')
                    continue;
            }
            if (remove_entry(&entry, d.aliases->data, ht) == ERR_NOT_FOUND) {
                printf("Not found: \033[31m\"%s\"\033[0m in \033[36m%s\033[0m\n", d.aliases->data, replace_home_with_tilde(ALIASES_PATH, PATH_BUFFER));
            } else {
                if (cli->verbosity)
                    printf("Deleted: %s \033[34m= \033[32m\"%s\"\033[0m (\033[33m%s\033[0m) from \033[36m%s\033[0m\n", 
                           entry->alias, entry->command, entry->section, replace_home_with_tilde(ALIASES_PATH, PATH_BUFFER));
                something_removed = true;
            }
        }
        d.aliases = d.aliases->next;
    }
    if (!something_removed)
        return cleanup(0, 0, ht, tmp_f, TMP_MISMATCHES_PATH);

    // Write new aliases back to file and check for write permission
    if (!write_aliases(tmp_f, ht))
        return cleanup("Error (file I/O): unable to write to temporary alias file: \"%s\".\n", TMP_MISMATCHES_PATH, ht, tmp_f, TMP_MISMATCHES_PATH);

    free_hash_table(ht);
    fclose(tmp_f);

    if (rename(TMP_MISMATCHES_PATH, ALIASES_PATH))
        return cleanup("Error (file I/O): cannot override aliases.\n", 0, 0, 0, TMP_MISMATCHES_PATH);
    return true;
}
