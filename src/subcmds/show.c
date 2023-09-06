#include "subcmds.h"
#include "../hash_table/entry.h"
#include "../hash_table/hash_table.h"

bool show_cmd(struct Show s) {
    // Shows active aliases in the current or specified directory, in TOML format
    // These are aliases defined in .env files starting one after ~ to the directory,
    //   where aliases closer to this directory have priority. If 'all', then includes
    //   ~/.aliases
    //struct Show s = cli->cmd.show;
    return true;
}
