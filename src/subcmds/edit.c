#include "subcmds.h"
#include <unistd.h>
#include "../file_io.h"
#include "../hash_table/entry.h"
#include "../hash_table/hash_table.h"
#include "../toml/toml_extensions.h"

bool edit_cmd(Cli *cli) {
    struct Edit e = cli->cmd.edit;
    HashTable *ht;
    create_hash_table(&ht, INITIAL_CAPACITY, LOAD_FACTOR);

    // Pick the alias file ('.env' if 'a.local', else '~/.aliases')
    const char *alias_fname = (e.local) ? AUTOENV_FNAME : ALIAS_FNAME;
    // Make sure it exists
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

    // Serialize hash table to TOML_FNAME
    if (!ht_to_toml_file(ht, TOML_FNAME)) return 0;

    // Open the toml tmpfile with editor
    char command[64];
    char *editor = getenv("EDITOR");
    if (e.editor) {
        if (access(e.editor, X_OK)) {
            editor = e.editor; 
        } else {
            return cleanup("Error: editor program not found: %s\n", e.editor, ht, 0, TOML_FNAME);
        }
    }
    snprintf(command, sizeof(command), "%s %s", editor, TOML_FNAME);

    int result = system(command);
    if (!result)
        return cleanup("Error: failed to run command: %s\n", command, ht, 0, TOML_FNAME);

    if (!toml_file_to_ht(ht, TOML_FNAME)) return 0;

    // Write new aliases back to file and check for write permission
    if (!write_aliases(tmp_f, ht))
        return cleanup("Error: unable to write to alias file", e.editor, ht, 0, TOML_FNAME);

    free_hash_table(ht);
    fclose(tmp_f);
    rename(TMP_FNAME, alias_fname);
    return true;
}
