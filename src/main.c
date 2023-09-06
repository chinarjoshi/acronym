#include <stdio.h>
#include <pcre.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "file_io.h"
#include "subcmds/subcmds.h"
#include "parse_args/parse_args.h"
#include "hash_table/entry.h"
#include "hash_table/hash_table.h"
#include "toml/toml.h"
#define INITIAL_CAPACITY 51
#define LOAD_FACTOR .5
const char *TOML_FNAME = "/tmp/acronym_aliases.toml";
static int cleanup(const char *message, const char *message_arg, 
                          HashTable *ht, FILE *f, const char *fname_to_remove);

int main(int argc, char **argv) {
    Cli *cli = parse_args(argc, argv);
    if (!cli) {
        perror("Error: invalid command line arguments.");
        return 1;
    }

    Entry *entry;
    HashTable *ht;
    FILE *alias_f, *tmp_f;
    const char *alias_fname, *env_fname = getenv("AUTOENV_ENV_FILENAME");
    if (!env_fname)
        env_fname = ".env";
    switch (cli->type) {
        case ADD:;
            struct Add a = cli->cmd.add;
            create_hash_table(&ht, INITIAL_CAPACITY, LOAD_FACTOR);
            // Open aliases file according to 'local'
            alias_fname = (a.local) ? env_fname : "~/.aliases";
            alias_f = fopen(alias_fname, "w+");
            if (!alias_f)
                return cleanup("Error: aliases file cannot be opened: %s\n", alias_fname, ht, 0, 0);

            // Read aliases into hash table and write non-matching lines to tmp
            tmp_f = read_aliases(alias_f, ht);
            if (!tmp_f)
                return cleanup(0, 0, ht, alias_f, 0);

            // Create entry from command line args
            create_entry(&entry, a.command, a.alias_override, a.section_override, a.include_flags);

            // Add new entry to hash table and check for duplicate
            if (add_entry(entry, ht) == ERR_DUPLICATE) {
                if (cli->verbosity)
                    printf("Duplicate alias: %s=\"%s\"\n", entry->alias, entry->command);
                free_entry(entry);
                return cleanup(0, 0, ht, tmp_f, TMP_FNAME);
            }

            // Write new aliases back to file and check for write permission
            if (!write_aliases(tmp_f, ht))
                return cleanup("Error: unable to write to alias file", 0, ht, tmp_f, TMP_FNAME);

            free_hash_table(ht);
            fclose(tmp_f);
            rename(TMP_FNAME, alias_fname);
            break;

        case REMOVE:;
            struct Remove r = cli->cmd.remove;
            create_hash_table(&ht, INITIAL_CAPACITY, LOAD_FACTOR);
            // Open aliases file according to 'local'
            alias_fname = (r.local) ? env_fname : "~/.aliases";
            FILE *alias_f = fopen(alias_fname, "w+");
            if (!alias_f)
                return cleanup("Error: aliases file not found: %s\n", alias_fname, ht, 0, 0);

            // Read aliases into hash table and write non-matching lines to tmp
            FILE *tmp_f = read_aliases(alias_f, ht);
            if (!tmp_f)
                return cleanup(0, 0, ht, alias_f, 0);

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
                        printf("Section not found: %s\n", r.aliases->data);
                    } else if (cli->verbosity) {
                        printf("Deleted section: %s\n", r.aliases->data);
                    }
                } else {
                    if (r.interactive) {
                        char answer;
                        printf("Delete alias: \"%s\"? [y/N]\n", r.aliases->data);
                        scanf(" %c", &answer);
                        if (answer != 'y' && answer != 'Y')
                            continue;
                    }
                    if (remove_entry(&entry, r.aliases->data, ht) == ERR_NOT_FOUND) {
                        printf("Alias not found: %s\n", r.aliases->data);
                    } else if (cli->verbosity) {
                        printf("Deleted alias: %s\n", r.aliases->data);
                    }
                }
                r.aliases = r.aliases->next;
            }

            // Write new aliases back to file and check for write permission
            if (!write_aliases(tmp_f, ht))
                return cleanup("Error: unable to write to alias file", 0, ht, tmp_f, TMP_FNAME);

            free_hash_table(ht);
            fclose(tmp_f);
            rename(TMP_FNAME, alias_fname);
            break;

        case TREE:;
            // Displays a tree of how an alias changes over directories by finding all .env files in directories after this,
            // or starting from ~ if 'all'
            // 1. Use find/fd to generate a list of */.env paths starting from the specified directory
            // 2. Search each file for lines of the form with "alias X=Y # Z"

            //const char *auth_fname = getenv("AUTOENV_AUTH_FILE");
            //FILE *auth_f = fopen(auth_fname, "r");

            break;

        case SHOW:;
            // Shows active aliases in the current or specified directory, in TOML format
            // These are aliases defined in .env files starting one after ~ to the directory,
            //   where aliases closer to this directory have priority. If 'all', then includes
            //   ~/.aliases
            //struct Show s = cli->cmd.show;

            break;

        case EDIT:;
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
            struct Edit e = cli->cmd.edit;
            create_hash_table(&ht, INITIAL_CAPACITY, LOAD_FACTOR);
            // Open aliases file according to 'local'
            alias_fname = (e.local) ? env_fname : "~/.aliases";
            alias_f = fopen(alias_fname, "w+");
            if (!alias_f)
                return cleanup("Error: aliases file cannot be opened: %s\n", alias_fname, ht, 0, 0);

            // Read aliases into hash table and write non-matching lines to tmp
            tmp_f = read_aliases(alias_f, ht);
            if (!tmp_f)
                return cleanup(0, 0, ht, alias_f, 0);

            // Now we have existing aliases in ht. Convert ht into a toml file, open the file with editor, 
            // convert the new toml file back into ht, and write it to original file
            FILE *toml_f = fopen(TOML_FNAME, "w+");
            if (!toml_f)
                return cleanup("Error: toml tmpfile cannot be opened: %s\n", TOML_FNAME, ht, 0, 0);

            // ht_to_toml(ht, toml_f);

            char *editor = getenv("EDITOR");
            if (e.editor) {
                if (access(e.editor, X_OK)) {
                    editor = e.editor; 
                } else {
                    return cleanup("Error: editor program not found: %s\n", e.editor, ht, toml_f, TOML_FNAME);
                }
            }

            int result = system(editor);
            if (!result)
                return cleanup("Error: failed to launch the editor: %s\n", e.editor, ht, toml_f, TOML_FNAME);

            // toml_to_ht(ht, toml_f);

            // Write new aliases back to file and check for write permission
            if (!write_aliases(tmp_f, ht))
                return cleanup("Error: unable to write to alias file", e.editor, ht, toml_f, TOML_FNAME);

            free_hash_table(ht);
            fclose(tmp_f);
            rename(TMP_FNAME, alias_fname);
            break;
    }
}

static int cleanup(const char *message, const char *message_arg, 
                          HashTable *ht, FILE *f, const char *fname_to_remove) {
    if (message) {
        if (message_arg)
            fprintf(stderr, message, message_arg); 
        else
            fprintf(stderr, "%s", message); 
    }
    free_hash_table(ht);
    fclose(f);
    remove(fname_to_remove);
    return 1;
}
