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

int global_parse_opt(int key, char *arg, struct argp_state *state);
int add_parse_opt(int key, char *arg, struct argp_state *state);
int remove_parse_opt(int key, char *arg, struct argp_state *state);
int tree_parse_opt(int key, char *arg, struct argp_state *state);
int show_parse_opt(int key, char *arg, struct argp_state *state);
int edit_parse_opt(int key, char *arg, struct argp_state *state);
struct Cli parse_args(int argc, char **argv);
void free_Cli(Cli *cli);
void free_AliasList(AliasListNode *node);

#endif
