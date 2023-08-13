#include <stdio.h>
#include "main.h"
#include "parse_args.h"
#include "entry.h"
#include "hash_table.h"
#include "toml/toml.h"

int main(int argc, char **argv) {
    Cli *cli = parse_args(argc, argv);
    if (!cli)
        return 0;

    Entry *entry;
    HashTable *ht;
    char *file_path = "~/.aliases";
    switch (cli->type) {
        case ADD:;
            struct Add a = cli->cmd.add;
            create_hash_table(&ht, INITIAL_CAPACITY, LOAD_FACTOR);
            if (a.local)
                file_path = "./.env";    

            FILE *f = fopen(file_path, "r+");
            read_aliases(f, ht);
            create_entry(&entry, a.command, a.alias_override, a.section_override, a.include_flags); 
            fclose(f);

            if (a.local) {
                // Read .env line by line, ignoring alias lines
                // concatenate 
            } 

            write_aliases(f, ht);

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

        case REMOVE:;
            struct Remove r = cli->cmd.remove;
            create_hash_table(&ht, INITIAL_CAPACITY, LOAD_FACTOR);
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

        case SHOW:;
            // Shows active aliases in the current or specified directory, in TOML format
            // These are aliases defined in .env files starting one after ~ to the directory,
            //   where aliases closer to this directory have priority. If 'all', then includes
            //   ~/.aliases
            struct Show s = cli->cmd.show;

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

Status read_aliases(FILE *f, HashTable *ht) {
    char alias[32], command[256], section[32];
    char *section_ptr = NULL;
    Entry *entry;

    while (1) {
        if (fscanf(f, "alias %31s=%255s", alias, command) != 2) // Check if line is alias
            continue;
        if (fscanf(f, " ## %31s", section) == 1) // And optionally a category
            section_ptr = section;
        else
            section_ptr = NULL;

        if (create_entry(&entry, command, alias, section_ptr, false) == ERR_OUT_OF_MEMORY)
            return ERR_OUT_OF_MEMORY;

        add_entry(entry, ht);
    }
    return SUCCESS;
}

Status write_aliases(FILE *f, HashTable *ht) {
    Entry *entry;
    for (int i = 0; i < ht->capacity; i++) {
        entry = ht->backing_array[i];
        if (entry && !entry->is_removed) {
            fprintf(f, "alias %s=%s ## %s\n", entry->alias, entry->command, entry->section);
        }
    }
    return SUCCESS;
}

char **get_env_locations() {
    FILE *f = fopen("", "r"); 
    char buf[128];

    while (fgets(buf, 128, f))
    
}

Status hash_table_to_toml(HashTable *ht) {
    return 0;
}
