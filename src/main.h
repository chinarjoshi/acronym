#ifndef MAIN_H
#define MAIN_H
#include "hash_table.h"
#include <stdio.h>

#define INITIAL_CAPACITY 51
#define LOAD_FACTOR .5

Status read_aliases(FILE *f, HashTable *ht);
Status write_aliases(FILE *f, HashTable *ht);

#endif
