#ifndef PARSE_ARGS_H
#define PARSE_ARGS_H
#include <argp.h>
#include <stdbool.h>

struct Add {
    char *command; // Required
    char *alias_override;
    char *section_override;
    bool include_flags;
    bool local;
};

typedef struct AliasListNode {
    struct AliasListNode *next;
    char *data;
} AliasListNode;

struct Remove {
    AliasListNode *aliases;
    bool recursive;
    bool force;
    bool interactive;
    bool local;
};

struct Tree {
    AliasListNode *aliases;
    int depth;
    bool all;
};

struct Show {
    char *directory;
    bool all;
};

struct Edit {
    char *editor;
    bool local;
};

union Cmd {
    struct Add add;
    struct Remove remove;
    struct Tree tree;
    struct Show show;
    struct Edit edit;
};

enum CmdType { ADD, REMOVE, TREE, SHOW, EDIT };

typedef struct Cli {
    union Cmd cmd;
    enum CmdType type;
    int verbosity;
} Cli;

const struct argp_option global_options[] = {
    { "verbose", 'v', "VERBOSITY", OPTION_ARG_OPTIONAL, "Level of output verbosity (0-3)"},
    { 0 }
};

int global_parse_opt(int key, char *arg, struct argp_state *state);
const struct argp global_argp = { global_options, global_parse_opt }; 

const struct argp_option add_options[] = {
    { "alias", 'a', "ALIAS", 0, "Optionally provide alias" },
    { "section", 's', "SECTION", 0, "Optionally provide section/category" },
    { "include-flags", 'i', 0, 0, "Include flags when generating default alias" },
    { "local", 'l', 0, 0, "Change env file in current directory" },
    { 0 }
};

int add_parse_opt(int key, char *arg, struct argp_state *state);
const struct argp add_argp = { add_options, add_parse_opt }; 

const struct argp_option remove_options[] = {
    { "recursive", 'r', 0, 0, "Remove a whole section instead of alias(es)" },
    { "force", 'f', 0, 0, "Never prompt, even if alias not found" },
    { "interactive", 'i', 0, 0, "Prompt before every removal" },
    { "local", 'l', 0, 0, "Change env file in current directory" },
    { 0 }
};

int remove_parse_opt(int key, char *arg, struct argp_state *state);
const struct argp remove_argp = { remove_options, remove_parse_opt }; 

const struct argp_option tree_options[] = {
    { "depth", 'L', "DEPTH", 0, "Max display depth of the tree" },
    { "all", 'a', 0, 0, "Start from home directory" },
    { 0 }
};

int tree_parse_opt(int key, char *arg, struct argp_state *state);
const struct argp tree_argp = { tree_options, tree_parse_opt }; 

const struct argp_option show_options[] = {
    { "directory", 'd', "DIRECTORY", 0, "Directory to show active aliases" },
    { "all", 'a', 0, 0, "Show all active aliases, not just overriden ones" },
    { 0 }
};

int show_parse_opt(int key, char *arg, struct argp_state *state);
const struct argp show_argp = { show_options, show_parse_opt }; 

const struct argp_option edit_options[] = {
    { "editor", 'e', "EDITOR", 0, "Name of editor to use" },
    { "local", 'l', 0, 0, "Edit env file in current directory" },
    { 0 }
};

int edit_parse_opt(int key, char *arg, struct argp_state *state);
const struct argp edit_argp = { edit_options, edit_parse_opt }; 

#endif
