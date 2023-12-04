#ifndef FILE_IO
#define FILE_IO
#include "../hash_table/hash_table.h"
#include <pcre.h>
#include <stdio.h>

extern char ACRONYM_DIRNAME[64];
extern char ALIAS_FNAME[78];
extern char TMP_TOML_FILE[78];
extern char TMP_MISMATCHES_FILE[78];
extern const char *AUTOENV_FNAME;

extern const char *ALIAS_PATTERN;
extern const char *FILE_DELIMITER;
extern const int OVECTOR_LEN;
FILE *read_aliases(FILE *f, HashTable *ht, bool output_non_matches);
bool write_aliases(FILE *f, HashTable *ht);
bool match_line(pcre *re, pcre_extra *extras, int *ovector, char *line,
                char *alias, char *command, char *section);
void free_re_resources(pcre *re, pcre_extra *extras, FILE *f);
void setup_fname_buffers();
void ensure_alias_file_exists();

#endif
