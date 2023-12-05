#ifndef PARSE_ARGS_H
#define PARSE_ARGS_H
#include <argp.h>
#include <stdbool.h>
#define NUM_SUBCMDS 5
typedef struct AliasListNode AliasListNode;

struct Add {
    char *command; // Required
    char *alias_override;
    char *section_override;
    bool include_flags;
};

struct Remove {
    AliasListNode *aliases;
    bool section;
    bool force;
    bool interactive;
};

struct Edit {
    char *editor;
};

struct Show {
    AliasListNode *prefixes;
    char *commit_hash;
    bool alias;
    bool section;
};

struct AliasListNode {
    struct AliasListNode *next;
    char *data;
};

typedef union Cmd {
    struct Add add;
    struct Remove remove;
    struct Edit edit;
    struct Show show;
} Cmd;

typedef enum CmdType { ADD, REMOVE, EDIT, SHOW, UNDO } CmdType;
typedef enum Scope { GLOBAL, PROJ, LOCAL } Scope;

typedef struct Cli {
    union Cmd cmd;
    CmdType type;
    Scope scope;
    int verbosity;
} Cli;

typedef struct ArgpSubcmd {
    const char *name;
    enum CmdType type;
    struct argp *argp_parser;
} ArgpSubcmd;

extern struct argp add_argp;
extern struct argp remove_argp;
extern struct argp show_argp;
extern struct argp edit_argp;
extern struct argp undo_argp;
extern struct ArgpSubcmd argp_subcmds[];

int add_parse_opt(int key, char *arg, struct argp_state *state);
int remove_parse_opt(int key, char *arg, struct argp_state *state);
int show_parse_opt(int key, char *arg, struct argp_state *state);
int edit_parse_opt(int key, char *arg, struct argp_state *state);
int undo_parse_opt(int key, char *arg, struct argp_state *state);

struct Cli *parse_args(int argc, char **argv);
struct Cli *parse_global_options(int argc, char **argv);
struct Cli *validate_args(Cli *cli);
void free_cli(Cli *cli);
void free_alias_list(AliasListNode *node);

#endif
