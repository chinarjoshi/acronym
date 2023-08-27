#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <argp.h>
#include "parse_args.h"

struct Cli *parse_global_options(int argc, char **argv) {
    if (argc < 2) {
        printf("invalid use: provide option or subcommand\n");
        return NULL;
    }

    Cli *cli = calloc(1, sizeof(Cli));
    if (!cli)
        return NULL;

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
                        printf("Verbosity must be between 0-3. Provided: %s", optarg);
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
                printf("0.1.0\n");
                return NULL;
                break;
            case '?':
            default:
                printf("Error: invalid option\n");
                return NULL;
                break;
        }
    }
    return cli;
}

struct Cli *parse_args(int argc, char **argv) {
    Cli *cli = parse_global_options(argc, argv);
    if (!cli)
        return NULL;

    // Move forward argc and argv to subcommand
    argc -= optind;
    argv += optind;
    char *subcommand = argv[0];
    if (strcmp(subcommand, "add") == 0) {
        cli->type = ADD; 
        argp_parse(&add_argp, argc, argv, 0, 0, &cli->cmd.add);
    } else if (strcmp(subcommand, "remove") == 0) {
        cli->type = REMOVE; 
        argp_parse(&remove_argp, argc, argv, 0, 0, &cli->cmd.remove);
    } else if (strcmp(subcommand, "tree") == 0) {
        cli->type = TREE; 
        argp_parse(&tree_argp, argc, argv, 0, 0, &cli->cmd.tree);
    } else if (strcmp(subcommand, "show") == 0) {
        cli->type = SHOW; 
        argp_parse(&show_argp, argc, argv, 0, 0, &cli->cmd.show);
    } else if (strcmp(subcommand, "edit") == 0) {
        cli->type = EDIT; 
        argp_parse(&edit_argp, argc, argv, 0, 0, &cli->cmd.edit);
    } else {
        printf("Invalid subcommand: %s", subcommand);
    }
    return cli;
}

int add_parse_opt(int key, char *arg, struct argp_state *state) {
    struct Add *add = state->input;
    switch (key) {
        case 'a':
            if (!(add->alias_override = malloc(strlen(arg) + 1)))
                return 1;
            strcpy(add->alias_override, arg);
            break;
        case 's':
            if (!(add->alias_override = malloc(strlen(arg) + 1)))
                return 1;
            strcpy(add->section_override, arg);
            break;
        case 'i':
            add->include_flags = true;
            break;
        case 'l':
            add->local = true;
            break;
        case ARGP_KEY_ARG:;
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
    struct Remove *remove = state->input;
    switch (key) {
        case 'r':
            if (remove->interactive) {
                perror("Force and interactive flags cannot be true at same time.");
                return 1;
            }
            remove->recursive = true;
            break;
        case 'f':
            remove->force = true;
            break;
        case 'i':
            if (remove->force) {
                perror("Force and interactive flags cannot be true at same time.");
                return 1;
            }
            remove->interactive = true;
            break;
        case 'l':
            remove->local = true;
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

int tree_parse_opt(int key, char *arg, struct argp_state *state) {
    struct Tree *tree = state->input; 
    switch (key) {
        case 'd':
            if (!(tree->directory = malloc(strlen(arg) + 1)))
                return 1;
            strcpy(tree->directory, arg);
            break;
        case 'a':
            tree->all = true;
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
            new_node->next = tree->aliases;
            tree->aliases = new_node;
            break;
        default:
            return ARGP_ERR_UNKNOWN;
    }
    return 0;
}

int show_parse_opt(int key, char *arg, struct argp_state *state) {
    struct Show *show = state->input;
    switch (key) {
        case 'd':
            if (!(show->directory = malloc(strlen(arg) + 1)))
                return 1;
            strcpy(show->directory, arg);
            break;
        case 'a':
            show->all = true;
            break;
        default:
            return ARGP_ERR_UNKNOWN;
    }
    return 0;
}

int edit_parse_opt(int key, char *arg, struct argp_state *state) {
    struct Edit *edit = state->input;
    switch (key) {
        case 'e':
            if (!(edit->editor = malloc(strlen(arg) + 1)))
                return 1;
            strcpy(edit->editor, arg);
            break;
        case 'l':
            edit->local = true;
            break;
        default:
            return ARGP_ERR_UNKNOWN;
    }
    return 0;
}

void free_AliasList(AliasListNode *node) {
    AliasListNode *tmp;
    while (node->next) {
        free(node->data);
        tmp = node->next;
        free(node);
        node = tmp;
    } 
    free(node);
}

void free_Cli(Cli *cli) {
    switch (cli->type) {
        case ADD:
            free(cli->cmd.add.command);
            free(cli->cmd.add.alias_override);
            free(cli->cmd.add.section_override);
            break;
        case REMOVE:
            free_AliasList(cli->cmd.remove.aliases);
            break;
        case TREE:
            free_AliasList(cli->cmd.tree.aliases);
            break;
        case SHOW:
            free(cli->cmd.show.directory);
            break;
        case EDIT:
            free(cli->cmd.edit.editor);
            break;
    }
    free(cli);
}
