#include "hash_table/hash_table.h"
#include "subcmds/subcmds.h"

int main(int argc, char **argv) {
    // Parse command line options
    Cli *cli = parse_args(argc, argv);
    if (!cli)
        return 1;

    // Set useful data structures and globals
    HashTable *ht;
    create_hash_table(&ht, INITIAL_CAPACITY, LOAD_FACTOR);
    set_alias_and_autoenv_fnames();

    // Execute specified subcommand. The function is responsible for freeing
    // all allocated memory and returning correct exit code.
    switch (cli->type) {
        case ADD:
            return !add_cmd(cli, ht);
        case REMOVE:
            return !remove_cmd(cli, ht);
        case TREE:
            return !tree_cmd(cli, ht);
        case SHOW:
            return !show_cmd(cli, ht);
        case EDIT:
            return !edit_cmd(cli, ht);
    }
}
