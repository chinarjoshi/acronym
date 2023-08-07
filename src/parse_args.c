#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <argp.h>
#include "parse_args.h"

const char *argp_program_version = "0.1.0";
const char *argp_program_bug_address = "chinarjoshi7@gmail.com";

struct argp_option global_options[] = {
    { "verbose", 'v', "VERBOSITY", OPTION_ARG_OPTIONAL, "Level of output verbosity (0-3)"},
    { 0 }
};
struct argp global_argp = { global_options, global_parse_opt }; 

struct argp_option add_options[] = {
    { "alias", 'a', "ALIAS", 0, "Optionally provide alias" },
    { "section", 's', "SECTION", 0, "Optionally provide section/category" },
    { "include-flags", 'i', 0, 0, "Include flags when generating default alias" },
    { "local", 'l', 0, 0, "Change env file in current directory" },
    { 0 }
};
struct argp add_argp = { add_options, add_parse_opt }; 

struct argp_option remove_options[] = {
    { "recursive", 'r', 0, 0, "Remove a whole section instead of alias(es)" },
    { "force", 'f', 0, 0, "Never prompt, even if alias not found" },
    { "interactive", 'i', 0, 0, "Prompt before every removal" },
    { "local", 'l', 0, 0, "Change env file in current directory" },
    { 0 }
};
struct argp remove_argp = { remove_options, remove_parse_opt }; 

struct argp_option tree_options[] = {
    { "depth", 'L', "DEPTH", 0, "Max display depth of the tree" },
    { "all", 'a', 0, 0, "Start from home directory" },
    { 0 }
};
struct argp tree_argp = { tree_options, tree_parse_opt }; 

struct argp_option show_options[] = {
    { "directory", 'd', "DIRECTORY", 0, "Directory to show active aliases" },
    { "all", 'a', 0, 0, "Show all active aliases, not just overriden ones" },
    { 0 }
};
struct argp show_argp = { show_options, show_parse_opt }; 

struct argp_option edit_options[] = {
    { "editor", 'e', "EDITOR", 0, "Name of editor to use" },
    { "local", 'l', 0, 0, "Edit env file in current directory" },
    { 0 }
};
struct argp edit_argp = { edit_options, edit_parse_opt }; 


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
                argp_error(state, "Invalid subcommand: %s", arg);
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
            if (add->command)
                argp_error(state, "Only add one command at a time.");
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
    }
    return 0;
}

int tree_parse_opt(int key, char *arg, struct argp_state *state) {
    struct Tree *tree = state->input; 
    switch (key) {
        case 'd':
            tree->depth = atoi(arg);
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
        case 'l':
            edit->local = true;
            break;
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
}

struct Cli parse_args(int argc, char **argv) {
    Cli cli;
    argp_parse(&global_argp, argc, argv, 0, 0, &cli);
    return cli;
}

