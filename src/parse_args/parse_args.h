#ifndef PARSE_ARGS_H
#define PARSE_ARGS_H
#include <argp.h>
#include <stdbool.h>
#define NUM_SUBCMDS 5
typedef struct AliasListNode AliasListNode;

struct Create {
    char *command; // Required
    char *alias_override;
    char *section_override;
    char *comment;
    bool include_flags;
};

struct Read {
    AliasListNode *prefixes;
    bool use_aliases;
    bool use_sections;
};

struct Update {
    char *editor;
};

struct Delete {
    AliasListNode *aliases;
    bool section;
    bool force;
    bool interactive;
};

struct AliasListNode {
    struct AliasListNode *next;
    char *data;
};

typedef union Cmd {
    struct Create create;
    struct Read read;
    struct Update update;
    struct Delete delete;
} Cmd;

typedef enum CmdType { CREATE, READ, UPDATE, DELETE} CmdType;
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

extern struct argp create_argp;
extern struct argp read_argp;
extern struct argp update_argp;
extern struct argp delete_argp;
extern struct ArgpSubcmd argp_subcmds[];
extern const char *help_message;

int create_parse_opt(int key, char *arg, struct argp_state *state);
int delete_parse_opt(int key, char *arg, struct argp_state *state);
int read_parse_opt(int key, char *arg, struct argp_state *state);
int update_parse_opt(int key, char *arg, struct argp_state *state);

struct Cli *parse_args(int argc, char **argv);
struct Cli *parse_global_options(int argc, char **argv);
struct Cli *validate_args(Cli *cli);
void free_cli(Cli *cli);
void free_alias_list(AliasListNode *node);

#endif
