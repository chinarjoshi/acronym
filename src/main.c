#include "file_io/file_io.h"
#include "subcmds/subcmds.h"

int main(int argc, char **argv) {
    setup_fname_buffers();
    Cli *cli = parse_args(argc, argv);
    if (!cli) return 1;
    ensure_alias_file_exists();
    return !sub_cmds[cli->type](cli);
}
