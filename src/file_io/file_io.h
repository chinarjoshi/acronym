#ifndef FILE_IO
#define FILE_IO
#include "../hash_table/hash_table.h"
#include "../parse_args/parse_args.h"
#include <pcre.h>
#include <stdio.h>

extern bool IS_IN_GIT_REPO;
extern char *ALIASES_PATH;
extern char GLOBAL_ALIASES_PATH[PATH_MAX];
extern char PROJ_ALIASES_PATH[PATH_MAX];
extern char LOCAL_ALIASES_PATH[PATH_MAX];

extern char TMP_MISMATCHES_PATH[PATH_MAX];
extern char TMP_TOML_PATH[PATH_MAX];

extern const char *ALIAS_PATTERN;
extern const char *FILE_DELIMITER;
extern const int OVECTOR_LEN;
FILE *read_aliases(FILE *f, HashTable *ht, bool output_non_matches);
bool write_aliases(FILE *f, HashTable *ht);
bool match_line(pcre *re, pcre_extra *extras, int *ovector, char *line,
                char *alias, char *command, char *section);
void free_re_resources(pcre *re, pcre_extra *extras, FILE *f);
void setup_path_buffers(Scope scope);

#endif
