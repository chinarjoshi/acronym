#include <stdio.h>
#include "main.h"
#include "parse_args.h"
#include "entry.h"
#include "hash_table.h"
#include "toml.h"

int main(int argc, char **argv) {
    Cli *cli = parse_args(argc, argv);
    if (!cli)
        return 0;

    switch (cli->type) {
        case ADD:
            // Adds a global or directory-local alias.
            // 1. Open either ~/.aliases or ./.env
            // 2. Loop over all lines and find lines starting with 'alias'
            // 3. Add entries to hash table by finding the alias, command, and optionally section
            // 4. Add the new provided alias-command pair
            // (Note: Use of hash table in this operation is because the whole file must be read to enforce formatting)
            // 5. Create a single string from the hash table in the form of "alias a=command # section\nalias ..."
            // 6. If in local mode, then prune alias commands from .env, and concatenate aliases to the pruned file
            // 7. Write this string to correct file

            break;

        case REMOVE:
            // Removes a global or directory-local alias.
            // 1. Open either ~/.aliases or ./.env
            // 2. Loop over all lines and find lines starting with 'alias'
            // 3. Add entries to hash table by finding the alias, command, and optionally section
            // 4. Remove the specified aliases
            // 5. Create a single string from the hash table in the form of "alias a=command # section\nalias ..."
            // 6. If in local mode, then prune existing alias commands from .env, and concatenate new aliases to the pruned file
            // 7. Write this string to correct file

            break;

        case TREE:
            // Displays a tree of how an alias changes over directories by finding all .env files in directories after this,
            // or starting from ~ if 'all'
            // 1. Use find/fd to generate a list of */.env paths starting from the specified directory
            // 2. Search each file for lines of the form with "alias X=Y # Z"

            break;

        case SHOW:
            // Shows active aliases in the current or specified directory, in TOML format
            // These are aliases defined in .env files starting one after ~ to the directory,
            //   where aliases closer to this directory have priority. If 'all', then includes
            //   ~/.aliases

            break;

        case EDIT:
            // Edit either global or directory-specific aliases, in TOML format for readability
            // 1. Open either ~/.aliases or ./.env
            // 2. Loop over all lines and find lines starting with 'alias'
            // 3. Add entries to hash table by finding the alias, command, and optionally section
            // 4. Convert hash table to TOML format, and write to tmpfile
            // 5. Edit tmpfile using 'EDITOR'
            // 6. Convert TOML format back to hash table
            // 7. Create a single string from the hash table in the form of "alias a=command # section\nalias ..."
            // 8. If in local mode, then prune existing alias commands from .env, and concatenate new aliases to the pruned file
            // 9. Write this string to correct file

            break;
    }
}

Status read_aliases(char *path, HashTable *ht);
Status write_aliases(char *path, HashTable *ht);
Status hash_table_to_toml()
