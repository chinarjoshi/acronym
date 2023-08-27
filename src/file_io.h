#ifndef FILE_IO
#define FILE_IO
#include <stdio.h>
#include "hash_table.h"

extern const char *TMP_FNAME;
FILE *read_aliases(FILE *f, HashTable *ht);
bool write_aliases(FILE *f, HashTable *ht);

#endif
