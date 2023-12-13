#include "subcmds.h"
#include <unistd.h>
#include <string.h>
#include "../hash_table/entry.h"
#include "../hash_table/hash_table.h"
#include "../file_io/file_io.h"
#include "../toml/toml_extensions.h"

bool show_cmd(Cli *cli) {
    struct Show s = cli->cmd.show;

    HashTable *ht;
    create_hash_table(&ht, INITIAL_CAPACITY, LOAD_FACTOR);

    if (IS_IN_GIT_REPO) {
        // Include local aliases, highlighted differently to differentiate them
        if (!include_aliases_file(LOCAL_ALIASES_PATH, ht))
            return false;

        // And now project aliases
        if (!include_aliases_file(PROJ_ALIASES_PATH, ht))
            return false;
    }

    // If not in git repo or -g is passed, then include global aliases file. 
    // Use "!= GLOBAL" because GLOBAL is actually the default value.
    if (!IS_IN_GIT_REPO || cli->scope != GLOBAL) {
        FILE *alias_f = fopen(GLOBAL_ALIASES_PATH, "r");
        if (!alias_f) {
            perror("Error (file I/O): unable to open alias file: \"%s\".\n");
            return false;
        }
        read_aliases(alias_f, ht, false);
    }

    if (s.prefixes) {
        filter_hash_table(ht, s.prefixes, s.use_aliases, s.use_sections);
    }

    printf("%s", ht_to_toml_str(ht));
    free_hash_table(ht);
    return true;
}
