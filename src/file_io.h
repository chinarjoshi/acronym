#ifndef FILE_IO
#define FILE_IO
#include <stdio.h>
#include <pcre.h>
#include "hash_table.h"

extern const char *TMP_FNAME;
extern const char *ALIAS_PATTERN;
extern const char *FILE_DELIMITER;
extern const int OVECTOR_LEN;
FILE *read_aliases(FILE *f, HashTable *ht);
bool write_aliases(FILE *f, HashTable *ht);
bool match_line(pcre *re, pcre_extra *extras, int *ovector, char *line, 
                       char *alias, char *command, char *section);

#endif
