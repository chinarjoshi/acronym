#include "subcmds/subcmds.h"

int main(int argc, char **argv) {
    Cli *cli = parse_args(argc, argv);
    if (!cli) {
        perror("Error: invalid command line arguments.");
        return 1;
    }

    switch (cli->type) {
        case ADD:
            return add_cmd(cli);
        case REMOVE:
            return remove_cmd(cli);
        case TREE:
            return tree_cmd(cli);
        case SHOW:
            return show_cmd(cli);
        case EDIT:
            return edit_cmd(cli);
    }
}
