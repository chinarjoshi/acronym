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

    // Pick the alias file name ('.env' if 'a.local', else '~/.aliases')
    const char *alias_fname = (e.local) ? AUTOENV_FNAME : ALIAS_FNAME;
    // Make sure it exists
    if (access(alias_fname, F_OK) == -1) {
        FILE *f = fopen(alias_fname, "w"); 
        fclose(f);
    }
    FILE *alias_f = fopen(alias_fname, "r");
    if (!alias_f)
        return cleanup("Error (file I/O): aliases file cannot be opened: \"%s\".\n", alias_fname, ht, 0, 0, cli);

    // Read aliases into hash table and write non-matching lines to tmp
    FILE *tmp_f = read_aliases(alias_f, ht, true);
    if (!tmp_f)
        return cleanup(0, 0, ht, alias_f, 0, cli);

    // Serialize hash table to TOML_FNAME
    if (!ht_to_toml_file(ht, TOML_FNAME))
        return 0;

    // Empty table because entries will be arbitrarily added and removed
    empty_hash_table(ht);

    // Open the toml tmpfile with editor
    char command[128];
    char *editor = e.editor ? e.editor : getenv("EDITOR");
    snprintf(command, sizeof(command), "%s %s", editor, TOML_FNAME);

    int result = system(command);
    if (result)
        return cleanup("Error (system): failed to run command: \"%s\".\n", command, ht, 0, TOML_FNAME, cli);

    if (!toml_file_to_ht(ht, TOML_FNAME))
        return 0;
    remove(TOML_FNAME);

    // Write new aliases back to file and check for write permission
    if (!write_aliases(tmp_f, ht))
        return cleanup("Error (file I/O): unable to write to alias file: \"%s\".\n", alias_fname, ht, 0, TOML_FNAME, cli);

    free_hash_table(ht);
    fclose(tmp_f);
    free_Cli(cli);
    if (rename(TMP_FNAME, alias_fname))
        return cleanup("Error (file I/O): cannot rename file.\n", 0, 0, 0, TMP_FNAME, 0);
    return true;
}
