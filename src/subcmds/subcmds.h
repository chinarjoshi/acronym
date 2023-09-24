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

extern bool (*sub_cmds[])(Cli *);
bool add_cmd(Cli *cli);
bool remove_cmd(Cli *cli);
bool edit_cmd(Cli *cli);
bool show_cmd(Cli *cli);
bool sync_cmd(Cli *cli);
bool reccomend_cmd(Cli *cli);
char **get_env_paths(const char *start, int *num_paths);
enum PathCmp compare_paths(const char *env_fname, const char *directory);
int cleanup(const char *message, const char *message_arg, 
                          HashTable *ht, FILE *f, const char *fname_to_remove);
void free_env_paths(char **paths, int num_paths);
void filter_hash_table(HashTable *ht, AliasListNode *l, bool alias, bool section);

#endif
