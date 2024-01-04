#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <argp.h>
#include "parse_args.h"
#include "../file_io/file_io.h"

static const char *in_git_repo_cmd = "git rev-parse --is-inside-work-tree >/dev/null 2>&1";
static const char *sem_ver = "1.0.0";

static struct argp *current_argp;
static void print_help_and_exit(int exit_code) {
    printf("");
    exit(exit_code);
}

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
            current_argp = argp_subcmds[i].argp_parser;
            argp_parse(current_argp, argc, argv, 0, 0, cli);
            found = 1;
            break;
        }
    }

    if (!found) {
        printf("Invalid argument: found invalid subcommand \"%s\", try CRUD.\n\n", subcommand);
        print_help_and_exit(1);
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

        printf("%s/%s\n", dir, file);
        exit(0);
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
                        printf("Invalid argument: verbosity must be between 0-3, but was given \"%s\".\n\n", optarg);
                        print_help_and_exit(1);
                    }
                    cli->verbosity = verbosity;
                } else {
                    cli->verbosity = 1;
                }
                break;
            case 'q':
                cli->verbosity = 0;
                break;
            case 'h':
                print_help_and_exit(0);
            case 'V':
                printf("%s\n", sem_ver);
                exit(0);
            case '?':
            default:
                printf("Invalid argument: unknown option \"%c\".\n\n", opt);
                print_help_and_exit(1);
        }
    }
    return cli;
}

int create_parse_opt(int key, char *arg, struct argp_state *state) {
    Cli *cli = (Cli *)state->input;
    struct Create *create = &cli->cmd.create;
    switch (key) {
        case 'a':
            if (create->alias_override) {
                printf("Invalid argument: multiple alias overrides provided: \"%s\", \"%s\".\n\n", 
                       create->alias_override, arg);
                argp_state_help(state, stdout, ARGP_HELP_STD_ERR);
            }
            if (!(create->alias_override = strdup(arg))) {
                perror("Failed memory allocation in function \"create_parse_opt\".\n");
                exit(1);
            }
            break;
        case 's':
            if (create->section_override) {
                printf("Invalid argument: multiple section overrides provided: \"%s\", \"%s\".\n\n", 
                       create->section_override, arg);
                argp_state_help(state, stdout, ARGP_HELP_STD_ERR);
            }
            if (!(create->section_override = strdup(arg))) {
                perror("Failed memory allocation in function \"create_parse_opt\".\n");
                exit(1);
            }
            break;
        case 'c':
            if (create->comment) {
                printf("Invalid argument: multiple comments provided: \"%s\", \"%s\".\n\n", 
                       create->comment, arg);
                argp_state_help(state, stdout, ARGP_HELP_STD_ERR);
            }
            if (!(create->comment = malloc(strlen(arg) + 3))) {
                perror("Failed memory allocation in function \"create_parse_opt\".\n");
                exit(1);
            }
            sprintf(create->comment, "# %s", arg);
            break;
        case 'i':
            create->include_flags = true;
            break;
        case 'p':
            if (cli->scope != GLOBAL) {
                printf("Invalid argument: multiple scopes provided.\n\n");
                argp_state_help(state, stdout, ARGP_HELP_STD_ERR);
            }
            cli->scope = PROJ;
            break;
        case 'l':
            if (cli->scope != GLOBAL) {
                printf("Invalid argument: multiple scopes provided.\n\n");
                argp_state_help(state, stdout, ARGP_HELP_STD_ERR);
            }
            cli->scope = LOCAL;
            break;
        case 'h':
            argp_state_help(state, stdout, ARGP_HELP_STD_HELP);
            break;
        case ARGP_KEY_ARG:;
            if (create->command) {
                printf("Invalid argument: multiple commands provided: \"%s\", \"%s\".\n\n", 
                       create->command, arg);
                argp_state_help(state, stdout, ARGP_HELP_STD_ERR);
            }
            if (!(create->command = strdup(arg))) {
                perror("Failed memory allocation in function \"create_parse_opt\".\n");
                exit(1);
            }
            break;
        default:
            return ARGP_ERR_UNKNOWN;
    }
    return 0;
}

int delete_parse_opt(int key, char *arg, struct argp_state *state) {
    Cli *cli = (Cli *)state->input;
    struct Delete *delete = &cli->cmd.delete;
    switch (key) {
        case 's':
            delete->section = true;
            break;
        case 'f':
            delete->force = true;
            break;
        case 'i':
            delete->interactive = true;
            break;
        case 'p':
            if (cli->scope != GLOBAL) {
                printf("Invalid argument: multiple scopes provided.\n\n");
                argp_state_help(state, stdout, ARGP_HELP_STD_ERR);
            }
            cli->scope = PROJ;
            break;
        case 'l':
            if (cli->scope != GLOBAL) {
                printf("Invalid argument: multiple scopes provided.\n\n");
                argp_state_help(state, stdout, ARGP_HELP_STD_ERR);
            }
            cli->scope = LOCAL;
            break;
        case 'h':
            argp_state_help(state, stdout, ARGP_HELP_STD_HELP);
            break;
        case ARGP_KEY_ARG:;
            AliasListNode *new_node;
            if (!(new_node = malloc(sizeof(AliasListNode)))) {
                perror("Failed memory allocation in function \"delete_parse_opt\".\n");
                exit(1);
            }
            if (!(new_node->data = malloc(strlen(arg) + 1))) {
                free(new_node);
                perror("Failed memory allocation in function \"delete_parse_opt\".\n");
                exit(1);
            }
            strcpy(new_node->data, arg);
            new_node->next = delete->aliases;
            delete->aliases = new_node;
            break;
        default:
            return ARGP_ERR_UNKNOWN;
    }
    return 0;
}

int update_parse_opt(int key, char *arg, struct argp_state *state) {
    Cli *cli = (Cli *)state->input;
    struct Update *update = &cli->cmd.update;
    switch (key) {
        case 'e':
            if (update->editor) {
                printf("Invalid argument: multiple editors provided: \"%s\", \"%s\".\n\n", 
                       update->editor, arg);
                argp_state_help(state, stdout, ARGP_HELP_STD_ERR);
            }
            if (!(update->editor = strdup(arg))) {
                perror("Failed memory allocation in function \"update_parse_opt\".\n");
                exit(1);
            }
            break;
        case 'p':
            if (cli->scope != GLOBAL) {
                printf("Invalid argument: multiple scopes provided.\n\n");
                argp_state_help(state, stdout, ARGP_HELP_STD_ERR);
            }
            cli->scope = PROJ;
            break;
        case 'l':
            if (cli->scope != GLOBAL) {
                printf("Invalid argument: multiple scopes provided.\n\n");
                argp_state_help(state, stdout, ARGP_HELP_STD_ERR);
            }
            cli->scope = LOCAL;
            break;
        case 'h':
            argp_state_help(state, stdout, ARGP_HELP_STD_HELP);
            exit(0);
        default:
            return ARGP_ERR_UNKNOWN;
    }
    return 0;
}

int read_parse_opt(int key, char *arg, struct argp_state *state) {
    Cli *cli = (Cli *)state->input;
    struct Read *read = &cli->cmd.read;
    switch (key) {
        case 'a':
            read->use_aliases = true;
            break;
        case 's':
            read->use_sections = true;
            break;
        case 'g':
            // HACK: Global is the default, so to pass this parameter, make it not global
            cli->scope = LOCAL;
            break;
        case ARGP_KEY_ARG:;
            AliasListNode *new_node;
            if (!(new_node = malloc(sizeof(AliasListNode)))) {
                perror("Failed memory allocation in function \"read_parse_opt\".\n");
                exit(1);
            }
            if (!(new_node->data = malloc(strlen(arg) + 1))) {
                free(new_node);
                perror("Failed memory allocation in function \"read_parse_opt\".\n");
                exit(1);
            }
            strcpy(new_node->data, arg);
            new_node->next = read->prefixes;
            read->prefixes = new_node;
            break;
        case 'h':
            argp_state_help(state, stdout, ARGP_HELP_STD_HELP);
            exit(0);
        default:
            return ARGP_ERR_UNKNOWN;
    }
    return 0;
}

Cli *validate_args(Cli *cli) {
    bool valid = true;
    union Cmd c = cli->cmd;
    IS_IN_GIT_REPO = !system(in_git_repo_cmd);

    if ((cli->scope == PROJ || cli->scope == LOCAL) && !IS_IN_GIT_REPO) {
        printf("Invalid usage: must be in git repo to use project-wide or local project-wide scope.\n\n");
        valid = false;
    }

    switch (cli->type) {
        case CREATE:
            if (!c.create.command) {
                printf("Missing argument: must provide a command to alias.\n\n");
                valid = false;
            }
            break;
        case READ:
            if (c.read.use_aliases && c.read.use_sections) {
                printf("Invalid argument: cannot filter by both aliases and sections simutaneously.\n\n");
                valid = false;
            }
            break;
        case UPDATE:
            if (!c.update.editor)
                break;
            char command[128];
            snprintf(command, 128, "command -v %s > /dev/null", c.update.editor);
            if (system(command)) {
                printf("Invalid argument: editor program not found: \"%s\".\n\n", c.update.editor);
                valid = false;
            }
            break;
        case DELETE:
            if (c.delete.force && c.delete.interactive) {
                printf("Invalid argument: force and interactive flags cannot be true at same time.\n\n");
                valid = false;
            }
            if (!c.delete.aliases) {
                printf("Invalid argument: no %s provided.\n\n", c.delete.section ? "section" : "alias"); 
                valid = false;
            }
            break;
    }

    if (!valid) {
        argp_help(current_argp, stdout, ARGP_HELP_STD_HELP, "acronym");
        exit(1);
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
        case CREATE:
            free(cli->cmd.create.command);
            free(cli->cmd.create.alias_override);
            free(cli->cmd.create.section_override);
            break;
        case READ:
            free_alias_list(cli->cmd.read.prefixes);
            break;
        case UPDATE:
            free(cli->cmd.update.editor);
            break;
        case DELETE:
            free_alias_list(cli->cmd.delete.aliases);
            break;
    }
    free(cli);
}
