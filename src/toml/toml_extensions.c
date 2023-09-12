#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "toml.h"

// Add a subtable to an existing table
int toml_add_subtable(toml_table_t *parent, const char *key, toml_table_t *sub) {
    if (!parent || !key || !sub) return -1; // Validate inputs
    
    // Update table pointers and counters
    parent->ntab++;
    parent->tab = realloc(parent->tab, parent->ntab * sizeof(toml_table_t*));
    if (!parent->tab) return -1;  // Memory allocation failure
    
    sub->key = strdup(key);
    parent->tab[parent->ntab - 1] = sub;
    return 0;
}

// Add a key-value pair to a table
int toml_add_keyval(toml_table_t *table, const char *key, const char *val) {
    if (!table || !key || !val) return -1; // Validate inputs

    // Update key-value pointers and counters
    table->nkval++;
    table->kval = realloc(table->kval, table->nkval * sizeof(toml_keyval_t*));
    if (!table->kval) return -1; // Memory allocation failure
    
    toml_keyval_t *kv = malloc(sizeof(toml_keyval_t));
    if (!kv) return -1; // Memory allocation failure
    
    kv->key = strdup(key);
    kv->val = strdup(val);
    table->kval[table->nkval - 1] = kv;
    return 0;
}

// Dump a toml_table_t to a string (recursive)
// Assume buffer is pre-allocated
void toml_dump(toml_table_t *table, char *buffer, int *pos) {
    if (!table || !buffer || !pos) return;

    for (int i = 0; i < table->nkval; ++i) {
        sprintf(buffer + *pos, "%s = %s\n", table->kval[i]->key, table->kval[i]->val);
        *pos += strlen(table->kval[i]->key) + strlen(table->kval[i]->val) + 4;
    }

    for (int i = 0; i < table->ntab; ++i) {
        sprintf(buffer + *pos, "[%s]\n", table->tab[i]->key);
        *pos += strlen(table->tab[i]->key) + 3;
        toml_dump(table->tab[i], buffer, pos);
    }
}

static void toml_dump_helper(FILE *fp, toml_table_t *table, int indent) {
    if (!fp || !table) return;

    // Dump key-value pairs
    for (int i = 0; i < table->nkval; ++i) {
        for (int j = 0; j < indent; ++j) fprintf(fp, "  ");
        fprintf(fp, "%s = \"%s\"\n", table->kval[i]->key, table->kval[i]->val);
    }

    // Dump sub-tables
    for (int i = 0; i < table->ntab; ++i) {
        for (int j = 0; j < indent; ++j) fprintf(fp, "  ");
        fprintf(fp, "[%s]\n", table->tab[i]->key);
        toml_dump_helper(fp, table->tab[i], indent + 1);
    }
}

// Dump a toml_table_t to a file
void toml_dump_to_file(toml_table_t *table, const char *filename) {
    if (!table || !filename) return;

    FILE *fp = fopen(filename, "w");
    if (!fp) return;  // File opening failure

    toml_dump_helper(fp, table, 0);

    fclose(fp);
}
