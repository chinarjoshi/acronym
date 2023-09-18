#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "toml.h"
#include "toml_extensions.h"
#include "../hash_table/hash_table.h"

static toml_table_t *create_toml_table(const char *key) {
    toml_table_t *t = calloc(1, sizeof(toml_table_t));
    if (!t) return NULL;
    t->key = key; 
    return t;
}

int ht_to_toml_file(HashTable *ht, const char *toml_fname) {
    FILE *fp = fopen(toml_fname, "w");
    if (!fp)
        return -1;
   
    toml_table_t *t = create_toml_table(0);
    ht_to_toml_table(ht, t);
    toml_dump(t, fp);
    
    fclose(fp);
    return 0;
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
    if (!ht || !root)
        return;

    for (int i = 0; i < ht->capacity; i++) {
        Entry *entry = ht->backing_array[i];
        if (entry && !entry->is_removed) {
            toml_table_t *section = toml_table_in(root, entry->section);
            if (!section) {
                section = create_toml_table(entry->section);
                toml_add_subtable(root, section);
            }
            toml_add_keyval(section, entry->alias, entry->command);
        }
    }
}

void toml_table_to_ht(HashTable *ht, toml_table_t *root) {
    if (!ht || !root)
        return;

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
int toml_add_subtable(toml_table_t *parent, toml_table_t *sub) {
    if (!parent || !sub)
        return 0;
    
    toml_table_t **new_tab = malloc((parent->ntab + 1) * sizeof(toml_table_t));
    if (!new_tab)
        return 0;
    parent->ntab++;

    // Insert subtable in alphabetical order
    int i;
    while (i < parent->ntab) {
        if (strcmp(sub->key, parent->tab[i]->key) > 0) {
            new_tab[i] = sub;
            continue;
        }
        new_tab[i] = parent->tab[i];
        i++;
    }

    free(parent->tab);
    parent->tab = new_tab;
    return 1;
}

// Add a key-value pair to a table
int toml_add_keyval(toml_table_t *table, const char *key, const char *val) {
    if (!table)
        return 0;
    
    toml_keyval_t **new_kv = malloc((table->nkval + 1) * sizeof(toml_table_t));
    if (!new_kv)
        return 0;
    table->nkval++;

    // Insert subtable in alphabetical order
    int i;
    while (i < table->nkval) {
        if (strcmp(key, table->kval[i]->key) > 0) {
            new_kv[i]->key = strdup(key);
            new_kv[i]->val = strdup(val);
            continue;
        }
        new_kv[i] = table->kval[i];
        i++;
    }

    free(table->kval);
    table->kval = new_kv;
    return 1;
}

// Dump a toml_table_t to a string. Must free return value
char *toml_dumps(toml_table_t *table) {
    if (!table)
        return 0;

    // First determine length of string.
    int len = 0;
    toml_table_t *tab;
    for (int i = 0; i < table->ntab; i++) {
        tab = table->tab[i];
        len += strlen(tab->key) + 3; // [%s]\n 
        for (int j = 0; j < tab->nkval; j++) {
            // "%s = \"%s\"\n"
            len += strlen(tab->kval[j]->key) + strlen(tab->kval[j]->val) + 6;
        }
        len += 1; // "\n"
    }
    len += 1; // \0

    char *toml_str = malloc(len);

    int pos;
    for (int i = 0; i < table->ntab; i++) {
        tab = table->tab[i];
        sprintf(toml_str + pos, "[%s]\n", tab->key);
        pos += strlen(tab->key) + 3;
        for (int j = 0; j < tab->nkval; j++) {
            sprintf(toml_str + pos, "%s = \"%s\"\n", tab->kval[j]->key, tab->kval[j]->val);
            len += strlen(tab->kval[j]->key) + strlen(tab->kval[j]->val) + 6;
        }
        sprintf(toml_str + pos, "\n");
        len += 1; // "\n"
    }
    toml_str[pos + 1] = '\0';
    return toml_str;
}

void toml_dump(toml_table_t *table, FILE *fp) {
    if (!fp || !table)
        return;

    toml_table_t *tab;
    for (int i = 0; i < table->ntab; i++) {
        tab = table->tab[i];
        fprintf(fp, "[%s]\n", tab->key);
        for (int j = 0; j < tab->nkval; j++) {
            fprintf(fp, "%s = \"%s\"\n", tab->kval[j]->key, tab->kval[j]->val);
        }
        fprintf(fp, "\n");
    }
}
