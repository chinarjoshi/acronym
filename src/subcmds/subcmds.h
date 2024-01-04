#ifndef SUBCMDS_H
#define SUBCMDS_H
#include <stdbool.h>
#include "../parse_args/parse_args.h"
#include "../hash_table/hash_table.h"

extern bool (*sub_cmds[])(Cli *);
bool add_cmd(Cli *cli);
bool remove_cmd(Cli *cli);
bool edit_cmd(Cli *cli);
bool show_cmd(Cli *cli);
int cleanup(const char *message, const char *message_arg, 
                          HashTable *ht, FILE *f, const char *fname_to_remove);
void filter_hash_table(HashTable *ht, AliasListNode *l, bool alias, bool section, int reset_cc);
bool include_aliases_file(char *path, HashTable *ht, int color_code);
char* replace_home_with_tilde(const char* path, char *buffer);

#endif
