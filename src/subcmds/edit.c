#include "subcmds.h"
#include <unistd.h>
#include "../file_io/file_io.h"
#include "../hash_table/entry.h"
#include "../hash_table/hash_table.h"
#include "../toml/toml_extensions.h"

bool edit_cmd(Cli *cli) {
    struct Edit e = cli->cmd.edit;
    HashTable *ht;
    create_hash_table(&ht, INITIAL_CAPACITY, LOAD_FACTOR);

    // Open the aliases file
    FILE *alias_f = fopen(ALIASES_PATH, "r");
    if (!alias_f)
        return cleanup("Error (file I/O): aliases file cannot be opened: %s.\n", ALIASES_PATH, ht, 0, 0);

    // Read aliases into hash table and write non-matching lines to tmp
    FILE *tmp_f = read_aliases(alias_f, ht, true);
    if (!tmp_f)
        return cleanup(0, 0, ht, alias_f, 0);

    // Serialize hash table to TMP_TOML_PATH
    if (!ht_to_toml_file(ht, TMP_TOML_PATH))
        return 0;

    // Empty table because entries will be arbitrarily added and removed
    empty_hash_table(ht);

    // Open the toml tmpfile with editor
    char command[128];
    char *editor = e.editor ? e.editor : getenv("EDITOR");
    snprintf(command, sizeof(command), "%s %s", editor, TMP_TOML_PATH);

    int result = system(command);
    if (result)
        return cleanup("Error (system): failed to run command: \"%s\".\n", command, ht, 0, TMP_TOML_PATH);

    if (!toml_file_to_ht(ht, TMP_TOML_PATH))
        return 0;
    remove(TMP_TOML_PATH);

    // Write new aliases back to file and check for write permission
    if (!write_aliases(tmp_f, ht))
        return cleanup("Error (file I/O): unable to write to alias file: \"%s\".\n", ALIASES_PATH, ht, 0, TMP_TOML_PATH);

    free_hash_table(ht);
    fclose(tmp_f);
    if (rename(TMP_MISMATCHES_PATH, ALIASES_PATH))
        return cleanup("Error (file I/O): cannot rename file.\n", 0, 0, 0, TMP_MISMATCHES_PATH);
    return true;
}
