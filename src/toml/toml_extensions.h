#ifndef TOML_EXTENSIONS_H
#define TOML_EXTENSIONS_H
#include "toml.h"
#include "../hash_table/hash_table.h"

int ht_to_toml_file(HashTable *ht, const char *toml_fname);
int toml_file_to_ht(HashTable *ht, const char *toml_fname);
void ht_to_toml_table(HashTable *ht, toml_table_t *root);
void toml_table_to_ht(HashTable *ht, toml_table_t *root);
int toml_add_subtable(toml_table_t *parent, toml_table_t *sub);
int toml_add_keyval(toml_table_t *table, toml_keyval_t *kv);
char *ht_to_toml_str(HashTable *ht);
char *toml_dumps(toml_table_t *table);
void toml_dump(toml_table_t *table, FILE *fp);

#endif
