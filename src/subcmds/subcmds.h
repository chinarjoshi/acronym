#ifndef SUBCMDS_H
#define SUBCMDS_H
#include <stdbool.h>
#include "../parse_args/parse_args.h"
#include "../hash_table/hash_table.h"

enum PathCmp {
    PATH_EQ,
    PATH_CHILD,
    PATH_PARENT,
    PATH_UNRELATED
};

extern const char *AUTOENV_FNAME;
extern char TMP_FNAME[64];
extern char TOML_FNAME[64];
extern char ALIAS_FNAME[64];
extern bool (*sub_cmds[])(Cli *);

bool add_cmd(Cli *cli);
bool remove_cmd(Cli *cli);
bool tree_cmd(Cli *cli);
bool show_cmd(Cli *cli);
bool edit_cmd(Cli *cli);
bool is_valid_dir(const char *dir);
void setup_fname_buffers();
char **get_env_paths(const char *start, int *num_paths);
enum PathCmp compare_paths(const char *env_fname, const char *directory);
int cleanup(const char *message, const char *message_arg, 
                          HashTable *ht, FILE *f, const char *fname_to_remove);

#endif
