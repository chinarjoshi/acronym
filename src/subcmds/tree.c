#include "subcmds.h"
#include "../hash_table/entry.h"
#include "../hash_table/hash_table.h"

bool tree_cmd(struct Tree t) {
            // Displays a tree of how an alias changes over directories by finding all .env files in directories after this,
            // or starting from ~ if 'all'
            // 1. Use find/fd to generate a list of */.env paths starting from the specified directory
            // 2. Search each file for lines of the form with "alias X=Y # Z"

            //const char *auth_fname = getenv("AUTOENV_AUTH_FILE");
            //FILE *auth_f = fopen(auth_fname, "r");
    return true;
}
