#include "subcmds.h"
#include <unistd.h>
#include <string.h>
#include "../hash_table/entry.h"
#include "../hash_table/hash_table.h"
#include "../file_io/file_io.h"
#include "../toml/toml_extensions.h"

bool read_cmd(Cli *cli) {
    struct Read r = cli->cmd.read;

    HashTable *ht;
    int orig_ht_size = 0;
    bool added_global = false, added_local = false, added_proj = false;
    int global_cc = 0, local_cc = 33, proj_cc = 36; // Color codes
    create_hash_table(&ht, INITIAL_CAPACITY, LOAD_FACTOR);

    // First lets find out whether there are any proj or local aliases,
    // if in a git repo and -g is not passed in
    bool non_empty_git_aliases_files = false;
    if (IS_IN_GIT_REPO) {
        if (!include_aliases_file(PROJ_ALIASES_PATH, ht, 0))
            return false;
        if (!include_aliases_file(LOCAL_ALIASES_PATH, ht, 0))
            return false;

        non_empty_git_aliases_files = ht->size > 0;
        empty_hash_table(ht);
    }

    // Include global aliases under any of the following conditions:
    // -g is passed in,
    // or not in a git repo
    // or in a git repo, but empty aliases files
    if (cli->scope != GLOBAL || !IS_IN_GIT_REPO || !non_empty_git_aliases_files) {
        if (!include_aliases_file(GLOBAL_ALIASES_PATH, ht, global_cc))
            return false;
        if (r.prefixes)
            filter_hash_table(ht, r.prefixes, r.use_aliases, r.use_sections, global_cc);
        added_global = ht->size > orig_ht_size;
        orig_ht_size = ht->size;
    }

    if (non_empty_git_aliases_files) {
        // Include local aliases, highlighted differently to differentiate them
        if (!include_aliases_file(LOCAL_ALIASES_PATH, ht, local_cc))
            return false;
        if (r.prefixes)
            filter_hash_table(ht, r.prefixes, r.use_aliases, r.use_sections, local_cc);
        added_local = ht->size > orig_ht_size;
        orig_ht_size = ht->size;

        // And now project aliases
        if (!include_aliases_file(PROJ_ALIASES_PATH, ht, proj_cc))
            return false;
        if (r.prefixes)
            filter_hash_table(ht, r.prefixes, r.use_aliases, r.use_sections, proj_cc);
        added_proj = ht->size > orig_ht_size;
        orig_ht_size = ht->size;
    }


    if (added_global || added_local || added_proj) {
        printf("Color key: ");
        if (added_global) {
            printf("\033[%dm%s\033[0m", global_cc, replace_home_with_tilde(GLOBAL_ALIASES_PATH, PATH_BUFFER));
            if (added_proj || added_local)
                printf(", ");
        }
        if (added_proj) {
            printf("\033[%dm%s\033[0m", proj_cc, replace_home_with_tilde(PROJ_ALIASES_PATH, PATH_BUFFER));
            if (added_local)
                printf(", ");
        }
        if (added_local) {
            printf("\033[%dm%s\033[0m", local_cc, replace_home_with_tilde(LOCAL_ALIASES_PATH, PATH_BUFFER));
        }
        printf("\n\n%s", ht_to_toml_str(ht));
    } else {
        printf("Nothing to show.\n");
        return false;
    }

    free_hash_table(ht);
    return true;
}
