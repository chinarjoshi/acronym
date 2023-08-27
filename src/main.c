#include <stdio.h>
#include <pcre.h>
#include <stdlib.h>
#include <string.h>
#include "parse_args.h"
#include "file_io.h"
#include "entry.h"
#include "hash_table.h"
#include "toml/toml.h"
#define INITIAL_CAPACITY 51
#define LOAD_FACTOR .5

int main(int argc, char **argv) {
    Cli *cli = parse_args(argc, argv);
    if (!cli) {
        perror("Error reading command line arguments.");
        return 1;
    }

    FILE *f, *tmp;
    Entry *entry;
    HashTable *ht;
    const char *fname;
    const char *env_fname = getenv("AUTOENV_ENV_FILENAME");
    switch (cli->type) {
        case ADD:;
            // Adds a global or directory-local alias.
            // 1. Open either ~/.aliases or ./.env
            // 2. Loop over all lines and find lines starting with 'alias'
            // 3. Add entries to hash table by finding the alias, 
            //      command, and optionally section
            // 4. Add the new provided alias-command pair
            // 5. Create a single string from the hash table in the 
            //      form of "alias a=command # section\nalias ..."
            // 6. If in local mode, then prune alias commands from .env, 
            //      and concatenate aliases to the pruned file
            // 7. Write this string to correct file
            struct Add a = cli->cmd.add;
            create_hash_table(&ht, INITIAL_CAPACITY, LOAD_FACTOR);
            // Open aliases file
            fname = (a.local) ? env_fname : "~/.aliases";
            f = fopen(fname, "r");
            if (!f) {
                fprintf(stderr, "Aliases file not found: %s", fname);
                return 1;
            }
            // Read aliases into hash table and write non-matching lines to tmp
            tmp = read_aliases(f, ht);
            if (!tmp) {
                fprintf(stderr, "Error reading aliases file: %s", fname);
                return 2;
            }
            create_entry(&entry, a.command, a.alias_override, a.section_override, a.include_flags);
            if (add_entry(entry, ht) == ERR_DUPLICATE && cli->verbosity) {
                printf("Duplicate alias: %s=\"%s\"", entry->alias, entry->command);
                return 3;
            }
            write_aliases(tmp, ht);
            fclose(tmp);
            rename(TMP_FNAME, fname);
            break;

        case REMOVE:;
            // Removes a global or directory-local alias.
            // 1. Open either ~/.aliases or ./.env
            // 2. Loop over all lines and find lines starting with 'alias'
            // 3. Add entries to hash table by finding the alias, command, and optionally section
            // 4. Remove the specified aliases
            // 5. Create a single string from the hash table in the form of "alias a=command # section\nalias ..."
            // 6. If in local mode, then prune existing alias commands from .env, and concatenate new aliases to the pruned file
            // 7. Write this string to correct file
            struct Remove r = cli->cmd.remove;
            create_hash_table(&ht, INITIAL_CAPACITY, LOAD_FACTOR);
            fname = (r.local) ? "env" : "~/.aliases";
            f = fopen(fname, "r");
            if (!f) {
                fprintf(stderr, "Aliases file not found: %s", fname);
                return 1;
            }

            tmp = read_aliases(f, ht);
            if (!tmp) {
                fprintf(stderr, "Error reading aliases file: %s", fname);
                return 1;
            }

            // Delete given aliases or sections, prompting unless force is given
            while (r.aliases) {
                if (r.recursive) {
                    if (!r.force) {
                        char answer;
                        printf("Delete section: \"%s\"? [y/N]\n", r.aliases->data);
                        scanf(" %c", &answer);
                        if (answer != 'y' && answer != 'Y')
                            continue;
                    }
                    if (remove_section(r.aliases->data, ht) == ERR_NOT_FOUND) {
                        printf("Section not found: %s", r.aliases->data);
                    } else if (cli->verbosity) {
                        printf("Deleted section: %s", r.aliases->data);
                    }
                } else {
                    if (r.interactive) {
                        char answer;
                        printf("Delete section: \"%s\"? [y/N]\n", r.aliases->data);
                        scanf(" %c", &answer);
                        if (answer != 'y' && answer != 'Y')
                            continue;
                    }
                    if (remove_entry(&entry, r.aliases->data, ht) == ERR_NOT_FOUND) {
                        printf("Alias not found: %s", r.aliases->data);
                    } else if (cli->verbosity) {
                        printf("Deleted alias: %s", r.aliases->data);
                    }
                }
                r.aliases = r.aliases->next;
            }
            write_aliases(f, ht);
            fclose(tmp);
            rename(TMP_FNAME, fname);
            break;

        case TREE:
            // Displays a tree of how an alias changes over directories by finding all .env files in directories after this,
            // or starting from ~ if 'all'
            // 1. Use find/fd to generate a list of */.env paths starting from the specified directory
            // 2. Search each file for lines of the form with "alias X=Y # Z"

            break;

        case SHOW:;
            // Shows active aliases in the current or specified directory, in TOML format
            // These are aliases defined in .env files starting one after ~ to the directory,
            //   where aliases closer to this directory have priority. If 'all', then includes
            //   ~/.aliases
            //struct Show s = cli->cmd.show;

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

Status hash_table_to_toml(HashTable *ht) {
    return 0;
}
