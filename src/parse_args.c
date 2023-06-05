#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <argp.h>
#include "parse_args.h"

const char *argp_program_version = "0.1.0";
const char *argp_program_bug_address = "chinarjoshi7@gmail.com";

int global_parse_opt(int key, char *arg, struct argp_state *state) {
    Cli *cli = state->input; 
    switch (key) {
        case 'v':
            if (arg) {
                int verbosity = atoi(arg);
                if (verbosity < 0 || verbosity > 3) {
                    argp_error(state, "Verbosity must be between 0-3. Provided: %s", arg);
                }
                cli->verbosity = verbosity;
            } else {
                cli->verbosity = 1;
            }
            break;
        case 'q':
            global_parse_opt('v', "0", state);
            break;
        case ARGP_KEY_ARG:;
            int argc = state->argc - state->next + 1;
            char **argv = &state->argv[state->next - 1];
            if (strcmp(arg, "add") == 0) {
                cli->type = ADD; 
                argp_parse(&add_argp, argc, argv, 0, 0, &cli->cmd.add);
            } else if (strcmp(arg, "remove") == 0) {
                cli->type = REMOVE; 
                argp_parse(&remove_argp, argc, argv, 0, 0, &cli->cmd.remove);
            } else if (strcmp(arg, "tree") == 0) {
                cli->type = TREE; 
                argp_parse(&tree_argp, argc, argv, 0, 0, &cli->cmd.tree);
            } else if (strcmp(arg, "show") == 0) {
                cli->type = SHOW; 
                argp_parse(&show_argp, argc, argv, 0, 0, &cli->cmd.show);
            } else if (strcmp(arg, "edit") == 0) {
                cli->type = EDIT; 
                argp_parse(&edit_argp, argc, argv, 0, 0, &cli->cmd.edit);
            } else {
                argp_error(state, "Subcommand not found: %s", arg);
            }
            break;
        default:
            return ARGP_ERR_UNKNOWN;
    }
    return 0;
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
            char *command = state->argv[state->next];
            if (!(add->alias_override = malloc(strlen(command) + 1)))
                return 1;
            strcpy(add->alias_override, command);
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
            remove->recursive = true;
            break;
        case 'f':
            remove->force = true;
            break;
        case 'i':
            remove->interactive = true;
            break;
        case 'l':
            remove->local = true;
            break;
        case ARGP_KEY_ARG:
            
    }
    return 0;
}
int tree_parse_opt(int key, char *arg, struct argp_state *state) {
    return 0;
}
int show_parse_opt(int key, char *arg, struct argp_state *state) {
    return 0;
}
int edit_parse_opt(int key, char *arg, struct argp_state *state) {
    return 0;
}

struct Cli parse_args(int argc, char **argv) {
    Cli cli;
    argp_parse(&global_argp, argc, argv, 0, 0, &cli);
    return cli;
}
