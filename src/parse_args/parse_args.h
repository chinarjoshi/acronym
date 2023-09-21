#ifndef PARSE_ARGS_H
#define PARSE_ARGS_H
#include <argp.h>
#include <stdbool.h>
typedef struct AliasListNode AliasListNode;

struct Add {
    char *command; // Required
    char *alias_override;
    char *section_override;
    bool include_flags;
    bool local;
};

struct Remove {
    AliasListNode *aliases;
    bool section;
    bool force;
    bool interactive;
    bool local;
};

struct Edit {
    char *editor;
    bool local;
};

struct Show {
    AliasListNode *aliases;
    bool section;
    bool local;
};

struct Sync {
    char *rollback;
    char *forward;
    char *show;
};

struct Reccomend {
    int num_recs;
    bool interactive;
};

struct AliasListNode {
    struct AliasListNode *next;
    char *data;
};

union Cmd {
    struct Add add;
    struct Remove remove;
    struct Edit edit;
    struct Show show;
    struct Sync sync;
    struct Reccomend reccomend;
};

enum CmdType { ADD, REMOVE, EDIT, SHOW, SYNC, RECCOMEND };

typedef struct Cli {
    union Cmd cmd;
    enum CmdType type;
    int verbosity;
} Cli;

extern struct argp add_argp;
extern struct argp remove_argp;
extern struct argp show_argp;
extern struct argp edit_argp;

int add_parse_opt(int key, char *arg, struct argp_state *state);
int remove_parse_opt(int key, char *arg, struct argp_state *state);
int show_parse_opt(int key, char *arg, struct argp_state *state);
int edit_parse_opt(int key, char *arg, struct argp_state *state);
struct Cli *parse_args(int argc, char **argv);
struct Cli *parse_global_args(int argc, char **argv);
struct Cli *validate_args(Cli *cli);
void free_Cli(Cli *cli);
void free_AliasList(AliasListNode *node);

#endif
