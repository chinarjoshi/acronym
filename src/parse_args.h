#ifndef PARSE_ARGS_H
#define PARSE_ARGS_H
#include <argp.h>
#include <stdbool.h>
#include <stdio.h>

struct Add {
    char *command; // Required
    char *alias_override;
    char *section_override;
    bool include_flags;
    bool local;
};

struct Remove {
    char **aliases; // Required
    bool whole_section;
    bool force;
    bool interactive;
    bool local;
};

struct Tree {
    char *path;
    bool all;
};

struct Show {
    char *path;
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

struct Cli {
    union Cmd cmd;
    enum CmdType type;
};

#endif
