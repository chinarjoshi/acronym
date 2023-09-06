#include "subcmds.h"
#include <unistd.h>
#include "../file_io.h"
#include "../hash_table/entry.h"
#include "../hash_table/hash_table.h"

bool edit_cmd(Cli *cli) {
    // Edit either global or directory-specific aliases, in TOML format for readability
    // 1. Open either ~/.aliases or ./.env
    // 2. Loop over all lines and find lines starting with 'alias'
    // 3. Add entries to hash table by finding the alias, command, and optionally section
    // 4. Convert hash table to TOML format, and write to tmpfile
    // 5. Edit tmpfile using 'EDITOR'
    // 6. Convert TOML format back to hash table
    // 7. Create a single string from the hash table in the form of "alias a=command # section\nalias ..."
    // 8. If in local mode, then prune existing alias commands from .env, and concatenate new aliases to the pruned file
    // 9. Write this string to correct file
    struct Edit e = cli->cmd.edit;
    HashTable *ht;
    create_hash_table(&ht, INITIAL_CAPACITY, LOAD_FACTOR);
    // Open aliases file according to 'local'
    const char *alias_fname = get_alias_fname(e.local);
    FILE *alias_f = fopen(alias_fname, "w+");
    if (!alias_f)
        return cleanup("Error: aliases file cannot be opened: %s\n", alias_fname, ht, 0, 0);

    // Read aliases into hash table and write non-matching lines to tmp
    FILE *tmp_f = read_aliases(alias_f, ht);
    if (!tmp_f)
        return cleanup(0, 0, ht, alias_f, 0);

    // Now we have existing aliases in ht. Convert ht into a toml file, open the file with editor, 
    // convert the new toml file back into ht, and write it to original file
    FILE *toml_f = fopen(TOML_FNAME, "w+");
    if (!toml_f)
        return cleanup("Error: toml tmpfile cannot be opened: %s\n", TOML_FNAME, ht, 0, 0);

    // ht_to_toml(ht, toml_f);

    char *editor = getenv("EDITOR");
    if (e.editor) {
        if (access(e.editor, X_OK)) {
            editor = e.editor; 
        } else {
            return cleanup("Error: editor program not found: %s\n", e.editor, ht, toml_f, TOML_FNAME);
        }
    }

    int result = system(editor);
    if (!result)
        return cleanup("Error: failed to launch the editor: %s\n", e.editor, ht, toml_f, TOML_FNAME);

    // toml_to_ht(ht, toml_f);

    // Write new aliases back to file and check for write permission
    if (!write_aliases(tmp_f, ht))
        return cleanup("Error: unable to write to alias file", e.editor, ht, toml_f, TOML_FNAME);

    free_hash_table(ht);
    fclose(tmp_f);
    rename(TMP_FNAME, alias_fname);
    return true;
}
