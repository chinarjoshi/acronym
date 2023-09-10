#include "subcmds/subcmds.h"

int main(int argc, char **argv) {
    // Parse command line options
    Cli *cli = parse_args(argc, argv);
    if (!cli)
        return 1;

    // Execute specified subcommand. The function is responsible for freeing
    // all allocated memory and returning correct exit code.
    set_alias_and_autoenv_fnames();
    switch (cli->type) {
        case ADD:
            return !add_cmd(cli);
        case REMOVE:
            return !remove_cmd(cli);
        case TREE:
            return !tree_cmd(cli);
        case SHOW:
            return !show_cmd(cli);
        case EDIT:
            return !edit_cmd(cli);
    }
}
