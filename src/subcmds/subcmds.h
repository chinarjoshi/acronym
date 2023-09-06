#ifndef SUBCMDS_H
#define SUBCMDS_H
#include <stdbool.h>
#include "../parse_args/parse_args.h"

bool add_cmd(struct Add a);
bool remove_cmd(struct Remove r);
bool tree_cmd(struct Tree t);
bool show_cmd(struct Show s);
bool edit_cmd(struct Edit e);

#endif
