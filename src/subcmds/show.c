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
    int orig_ht_size = 0;
    create_hash_table(&ht, INITIAL_CAPACITY, LOAD_FACTOR);

    // If not in git repo or -g is passed, then include global aliases file. 
    // Use "!= GLOBAL" because GLOBAL is actually the default value.
    if (!IS_IN_GIT_REPO || cli->scope != GLOBAL) {
        if (!include_aliases_file(PROJ_ALIASES_PATH, ht))
            return false;
        if (ht->size > orig_ht_size)
            printf("White aliases are from \033[34m%s\033[0m.\n", GLOBAL_ALIASES_PATH);
    }

    if (IS_IN_GIT_REPO) {
        // Include local aliases, highlighted differently to differentiate them
        if (!include_aliases_file(LOCAL_ALIASES_PATH, ht))
            return false;
        if (ht->size > orig_ht_size)
            printf("");
        orig_ht_size = ht->size;

        // And now project aliases
        if (!include_aliases_file(PROJ_ALIASES_PATH, ht))
            return false;
        if (ht->size > orig_ht_size)
            printf("");
        orig_ht_size = ht->size;
    }

    if (s.prefixes)
        filter_hash_table(ht, s.prefixes, s.use_aliases, s.use_sections);

    printf("%s", ht_to_toml_str(ht));
    free_hash_table(ht);
    return true;
}
