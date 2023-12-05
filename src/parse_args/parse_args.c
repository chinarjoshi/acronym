#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <argp.h>
#include "parse_args.h"
#include "../file_io/file_io.h"

struct Cli *parse_args(int argc, char **argv) {
    Cli *cli = parse_global_options(argc, argv);
    if (!cli)
        return NULL;

    // Move forward argc and argv to subcommand
    argc -= optind;
    argv += optind;
    char *subcommand = argv[0];

    int found = 0;
    for (size_t i = 0; i < NUM_SUBCMDS; i++) {
        if (!strcmp(subcommand, argp_subcmds[i].name)) {
            cli->type = argp_subcmds[i].type;
            argp_parse(argp_subcmds[i].argp_parser, argc, argv, 0, 0, cli);
            found = 1;
            break;
        }
    }

    if (!found) {
        printf("Error (invalid args): invalid subcommand: %s\n", subcommand);
        return NULL;
    }

    return validate_args(cli);
}

struct Cli *parse_global_options(int argc, char **argv) {
    // If invoked without arguments, then print the alias file path
    if (argc < 2) {
        const char *dir = getenv("ACRONYM_GLOBAL_DIR");
        dir = (dir) ? dir : getenv("HOME");

        const char *file = getenv("ACRONYM_FILENAME");
        file = (file) ? file : ".aliases.sh";

        printf("%s/%s", dir, file);
        return NULL;
    }

    Cli *cli = calloc(1, sizeof(Cli));
    if (!cli)
        return NULL;
    cli->verbosity = 1;

    // Stop option processing at first subcommand, and v has an optional argument
    char *short_options = "+v::qhV";
    struct option long_options[] = {
        { "verbose", optional_argument, 0, 'v' },
        { "quiet", no_argument, 0, 'q' },
        { "help", no_argument, 0, 'h' },
        { "version", no_argument, 0, 'V' },
        { 0, 0, 0, 0 }
    };

    int opt;
    opterr = 0;
    while ((opt = getopt_long(argc, argv, short_options, long_options, 0)) != -1) {
        switch (opt) {
            case 'v':
                if (optarg) {
                    int verbosity = atoi(optarg);
                    if (verbosity < 0 || verbosity > 3) {
                        printf("Error (invalid args): verbosity must be between 0-3. Provided: %s\n", optarg);
                        return NULL;
                    }
                    cli->verbosity = verbosity;
                } else {
                    cli->verbosity = 1;
                }
                break;
            case 'q':
                cli->verbosity = 0;
                break;
            case 'h': // TODO: flesh out these options
                // Print help
                printf("Help message\n");
                return NULL;
                break;
            case 'V':
                // Print the version
                printf("1.0.0\n");
                return NULL;
                break;
            case '?':
            default:
                printf("Error (invalid args): unknown option: \"%c\".\n", opt);
                return NULL;
                break;
        }
    }
    return cli;
}

int add_parse_opt(int key, char *arg, struct argp_state *state) {
    struct Add *add = &((Cli *)state->input)->cmd.add;
    switch (key) {
        case 'a':
            if (add->alias_override) {
                printf("Error (invalid args): multiple alias overrides provided.\n");
                return 1;
            }
            if (!(add->alias_override = malloc(strlen(arg) + 1)))
                return 1;
            strcpy(add->alias_override, arg);
            break;
        case 's':
            if (add->section_override) {
                printf("Error (invalid args): multiple section overrides provided.\n");
                return 1;
            }
            if (!(add->section_override = malloc(strlen(arg) + 1)))
                return 1;
            strcpy(add->section_override, arg);
            break;
        case 'i':
            add->include_flags = true;
            break;
        case 'p':
            ((Cli *)state->input)->scope = PROJ;
            break;
        case 'l':
            ((Cli *)state->input)->scope = LOCAL;
            break;
        case ARGP_KEY_ARG:;
            if (add->command) {
                printf("Error (invalid args): multiple commands provided.\n");
                return 1;
            }
            if (!(add->command = malloc(strlen(arg) + 1)))
                return 1;
            strcpy(add->command, arg);
            break;
        default:
            return ARGP_ERR_UNKNOWN;
    }
    return 0;
}

int remove_parse_opt(int key, char *arg, struct argp_state *state) {
    struct Remove *remove = &((Cli *)state->input)->cmd.remove;
    switch (key) {
        case 's':
            remove->section = true;
            break;
        case 'f':
            remove->force = true;
            break;
        case 'i':
            remove->interactive = true;
            break;
        case 'p':
            ((Cli *)state->input)->scope = PROJ;
            break;
        case 'l':
            ((Cli *)state->input)->scope = LOCAL;
            break;
        case ARGP_KEY_ARG:;
            AliasListNode *new_node;
            if (!(new_node = malloc(sizeof(AliasListNode))))
                return 1;
            if (!(new_node->data = malloc(strlen(arg) + 1))) {
                free(new_node);
                return 1;
            }
            strcpy(new_node->data, arg);
            new_node->next = remove->aliases;
            remove->aliases = new_node;
            break;
        default:
            return ARGP_ERR_UNKNOWN;
    }
    return 0;
}

int edit_parse_opt(int key, char *arg, struct argp_state *state) {
    struct Edit *edit = &((Cli *)state->input)->cmd.edit;
    switch (key) {
        case 'e':
            if (edit->editor) {
                printf("Error (invalid args): multiple editors provided.\n");
                return 1;
            }
            if (!(edit->editor = malloc(strlen(arg) + 1)))
                return 1;
            strcpy(edit->editor, arg);
            break;
        case 'p':
            ((Cli *)state->input)->scope = PROJ;
            break;
        case 'l':
            ((Cli *)state->input)->scope = LOCAL;
            break;
        default:
            return ARGP_ERR_UNKNOWN;
    }
    return 0;
}

int show_parse_opt(int key, char *arg, struct argp_state *state) {
    struct Show *show = &((Cli *)state->input)->cmd.show;
    switch (key) {
        case 'a':
            show->alias = true;
            break;
        case 's':
            show->section = true;
            break;
        case 'p':
            ((Cli *)state->input)->scope = PROJ;
            break;
        case 'l':
            ((Cli *)state->input)->scope = LOCAL;
            break;
        case 'c':
            if (show->commit_hash) {
                printf("Error (invalid args): multiple commit hashes provided.\n");
                return 1;
            }
            if (!(show->commit_hash = malloc(strlen(arg) + 1)))
                return 1;
            strcpy(show->commit_hash, arg);
            break;
        case ARGP_KEY_ARG:;
            AliasListNode *new_node;
            if (!(new_node = malloc(sizeof(AliasListNode))))
                return 1;
            if (!(new_node->data = malloc(strlen(arg) + 1))) {
                free(new_node);
                return 1;
            }
            strcpy(new_node->data, arg);
            new_node->next = show->prefixes;
            show->prefixes = new_node;
            break;
        default:
            return ARGP_ERR_UNKNOWN;
    }
    return 0;
}

int undo_parse_opt(int key, char *arg, struct argp_state *state) {
    switch (key) {
        case 'p':
            ((Cli *)state->input)->scope = PROJ;
            break;
        case 'l':
            ((Cli *)state->input)->scope = LOCAL;
            break;
    }
    return 0;
}

Cli *validate_args(Cli *cli) {
    union Cmd c = cli->cmd;
    bool invalid = false;
    switch (cli->type) {
        case ADD:
            if (!c.add.command) {
                printf("Error (invalid args): must provide command to alias.\n");
                invalid = true;
            }
            break;
        case REMOVE:
            if (c.remove.force && c.remove.interactive) {
                printf("Error (invalid args): force and interactive flags " \
                       "cannot be true at same time.\n");
                invalid = true;
            }
            if (!c.remove.aliases) {
                printf("Error (invalid args): no %s provided.\n", 
                       c.remove.section ? "section" : "alias"); 
                invalid = true;
            }
            break;
        case EDIT:
            if (!c.edit.editor)
                break;
            char command[128];
            snprintf(command, 128, "command -v %s > /dev/null", c.edit.editor);
            if (system(command)) {
                printf("Error (system): editor program not found: \"%s\".\n", c.edit.editor);
                invalid = true;
            }
            break;
        case SHOW:
            if (c.show.alias && c.show.section) {
                printf("Error (invalid args): alias and section flags" \
                       "cannot be true at the same time.\n");
                invalid = true;
            }
            break;
        case UNDO:
            break;
    }
    if (invalid) {
        free_cli(cli);
        return NULL;
    }
    return cli;
}

void free_alias_list(AliasListNode *node) {
    AliasListNode *tmp;
    while (node) {
        tmp = node;
        free(node->data);
        node = node->next;
        free(tmp);
    } 
}

void free_cli(Cli *cli) {
    switch (cli->type) {
        case ADD:
            free(cli->cmd.add.command);
            free(cli->cmd.add.alias_override);
            free(cli->cmd.add.section_override);
            break;
        case REMOVE:
            free_alias_list(cli->cmd.remove.aliases);
            break;
        case SHOW:
            free_alias_list(cli->cmd.show.prefixes);
            break;
        case EDIT:
            free(cli->cmd.edit.editor);
            break;
        case UNDO:
            break;
    }
    free(cli);
}
