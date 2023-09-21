#include "subcmds/subcmds.h"

int main(int argc, char **argv) {
    Cli *cli = parse_args(argc, argv);
    if (!cli) return 1;

    setup_fname_buffers();
    return !sub_cmds[cli->type](cli);
}
