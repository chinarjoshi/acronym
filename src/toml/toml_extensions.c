#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "toml.h"
#include "toml_extensions.h"
#include "../hash_table/hash_table.h"
#define TOML_INDENT 4

int ht_to_toml_file(HashTable *ht, const char *toml_fname) {
    FILE *fp = fopen(toml_fname, "w");
    if (!fp)
        return -1;
   
    toml_table_t *t = 0;
    ht_to_toml_table(ht, t);
    toml_dump(t, fp, TOML_INDENT);
    
    fclose(fp);
    return 0;
}

static toml_table_t *create_toml_table(const char *key) {
    toml_table_t *t = calloc(1, sizeof(toml_table_t));
    if (!t) return NULL;
    t->key = key; 
    return t;
}

int toml_file_to_ht(HashTable *ht, const char *toml_fname) {
    FILE *fp = fopen(toml_fname, "r");
    if (!fp)
        return -1;
   
    char errbuf[128];
    toml_table_t *t = toml_parse_file(fp, errbuf, sizeof(errbuf));
    toml_table_to_ht(ht, t);
    
    fclose(fp);
    return 0;
}

void ht_to_toml_table(HashTable *ht, toml_table_t *root) {
    if (!root) {
        root = create_toml_table(0);
        if (!root) return;
    }

    for (int i = 0; i < ht->capacity; i++) {
        Entry *entry = ht->backing_array[i];
        if (entry && !entry->is_removed) {
            toml_table_t *section = toml_table_in(root, entry->section);
            if (!section) {
                section = create_toml_table(entry->section);
                toml_add_subtable(root, entry->section, section);
            }
            toml_add_keyval(section, entry->alias, entry->command);
        }
    }
}

void toml_table_to_ht(HashTable *ht, toml_table_t *root) {
    // Loop through each subtable (section)
    Entry *entry;
    for (int i = 0; i < root->ntab; ++i) {
        toml_table_t *section = root->tab[i];
        for (int j = 0; j < section->nkval; ++j) {
            toml_keyval_t *toml_entry = section->kval[j];
            create_entry(&entry, toml_entry->val, toml_entry->key, section->key, 0);
            add_entry(entry, ht);
        }
    }
}

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
void toml_dumps(toml_table_t *table, char *buffer, int *pos) {
    if (!table || !buffer || !pos) return;

    for (int i = 0; i < table->nkval; ++i) {
        sprintf(buffer + *pos, "%s = %s\n", table->kval[i]->key, table->kval[i]->val);
        *pos += strlen(table->kval[i]->key) + strlen(table->kval[i]->val) + 4;
    }

    for (int i = 0; i < table->ntab; ++i) {
        sprintf(buffer + *pos, "[%s]\n", table->tab[i]->key);
        *pos += strlen(table->tab[i]->key) + 3;
        toml_dumps(table->tab[i], buffer, pos);
    }
}

void toml_dump(toml_table_t *table, FILE *fp, int indent) {
    if (!fp || !table) return;

    // Dump key-value pairs
    for (int i = 0; i < table->nkval; ++i) {
        for (int j = 0; j < indent; ++j) fprintf(fp, "  ");
        fprintf(fp, "%s = \"%s\"\n", table->kval[i]->key, table->kval[i]->val);
    }

    // Dump sub-tables
    for (int i = 0; i < table->ntab; ++i) {
        for (int j = 0; j < indent; ++j)
            fprintf(fp, "  ");
        fprintf(fp, "[%s]\n", table->tab[i]->key);
        toml_dump(table->tab[i], fp, indent * 2);
    }
}
