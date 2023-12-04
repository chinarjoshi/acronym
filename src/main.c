#include "file_io/file_io.h"
#include "subcmds/subcmds.h"

int main(int argc, char **argv) {
    Cli *cli = parse_args(argc, argv);
    if (!cli) return 1;
    setup_path_buffers(cli->mode); 
    return !sub_cmds[cli->type](cli);
}
