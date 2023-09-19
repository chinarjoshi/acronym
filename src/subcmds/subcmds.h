#ifndef SUBCMDS_H
#define SUBCMDS_H
#include <stdbool.h>
#include "../parse_args/parse_args.h"
#include "../hash_table/hash_table.h"

extern const char *AUTOENV_FNAME;
extern char TMP_FNAME[64];
extern char TOML_FNAME[64];
extern char ALIAS_FNAME[64];
bool add_cmd(Cli *cli);
bool remove_cmd(Cli *cli);
bool tree_cmd(Cli *cli);
bool show_cmd(Cli *cli);
bool edit_cmd(Cli *cli);
void set_alias_and_autoenv_fnames();
int cleanup(const char *message, const char *message_arg, 
                          HashTable *ht, FILE *f, const char *fname_to_remove, Cli *cli);

#endif
