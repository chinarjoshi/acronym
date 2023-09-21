#include "subcmds.h"
#include <unistd.h>
#include <string.h>
#include "../hash_table/entry.h"
#include "../hash_table/hash_table.h"
#include "../file_io.h"
#include "../toml/toml_extensions.h"

bool show_cmd(Cli *cli) {
    struct Show s = cli->cmd.show;

    // Read up until 'directory' if provided, or current directory
    char cwd[PATH_MAX];
    if (!getcwd(cwd, sizeof(cwd))) {
        perror("Error (system): getcwd() error.\n");
        return false;
    }

    // Get a list of .env file paths from AUTOENV_AUTH_FILE
    int num_paths;
    char **env_paths = get_env_paths(cwd, &num_paths);
    if (!env_paths)
        return false;

    HashTable *ht;
    create_hash_table(&ht, INITIAL_CAPACITY, LOAD_FACTOR);

    // Include the global alias file if not 'local'
    if (!s.local) {
        FILE *alias_f = fopen(ALIAS_FNAME, "r");
        if (!alias_f) {
            perror("Error (file I/O): unable to open alias file: \"%s\".\n");
            return false;
        }
        read_aliases(alias_f, ht, false);
    }

    // Load in .env aliases starting from root directory to current directory
    for (int i = 0; i < num_paths; i++) {
        FILE *env_f = fopen(env_paths[i], "r");
        if (!env_f) continue;
        read_aliases(env_f, ht, false);
    }

    if (s.aliases)
        filter_hash_table(ht, s.aliases, s.section);

    printf("%s", ht_to_toml_str(ht));
    free_hash_table(ht);
    free_env_paths(env_paths, num_paths);
    return true;
}
