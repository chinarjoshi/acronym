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

static toml_keyval_t *create_toml_keyval(const char *key, const char *val) {
    toml_keyval_t *kv = malloc(sizeof(toml_table_t));
    if (!kv) return NULL;
    kv->key = key; 
    kv->val = val; 
    return kv;
}

int ht_to_toml_file(HashTable *ht, const char *toml_fname) {
    FILE *fp = fopen(toml_fname, "w");
    if (!fp)
        return 0;
   
    toml_table_t *t = create_toml_table(0);
    ht_to_toml_table(ht, t);
    toml_dump(t, fp);
    fclose(fp);
    return 1;
}

char *ht_to_toml_str(HashTable *ht) {
    toml_table_t *t = create_toml_table(0);
    ht_to_toml_table(ht, t);
    return toml_dumps(t);
}

int toml_file_to_ht(HashTable *ht, const char *toml_fname) {
    FILE *fp = fopen(toml_fname, "r");
    if (!fp)
        return 0;
   
    char errbuf[128];
    toml_table_t *t = toml_parse_file(fp, errbuf, sizeof(errbuf));
    toml_table_to_ht(ht, t);
    fclose(fp);
    return 1;
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
                if (!section) return;
                toml_add_subtable(root, section);
            }
            toml_keyval_t *kv = create_toml_keyval(entry->alias, entry->command);
            if (!kv) return;
            toml_add_keyval(section, kv);
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
            const char *key = toml_key_in(section, j);
            toml_datum_t d = toml_string_in(section, key);
            create_entry(&entry, d.u.s, key, section->key, 0);
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

    // Insert subtable in alphabetical order
    int i, inserted = 0;
    for (i = 0; i < parent->ntab; i++) {
        if (!inserted && strcasecmp(parent->tab[i]->key, sub->key) > 0) {
            new_tab[i] = sub;
            inserted = 1;
            parent->ntab++;
        }
        new_tab[i + inserted] = parent->tab[i];
    }

    if (!inserted) {
        new_tab[i] = sub;
        parent->ntab++;
    }

    free(parent->tab);
    parent->tab = new_tab;
    return 1;
}

// Compare two strings based on length, then case-insensitive string comparison
static int alias_cmp(const char *s1, const char *s2) {
    int s1_len = strlen(s1);
    int s2_len = strlen(s2);
    if (s1_len != s2_len)
        return s1_len - s2_len;
    return strcasecmp(s1, s2);
}

// Add a key-value pair to a table
int toml_add_keyval(toml_table_t *table, toml_keyval_t *kv) {
    if (!table)
        return 0;
    
    toml_keyval_t **new_kv = malloc((table->nkval + 1) * sizeof(toml_table_t));
    if (!new_kv)
        return 0;

    // Insert subtable in alphabetical order
    int i, inserted = 0;
    for (i = 0; i < table->nkval; i++) {
        if (!inserted && alias_cmp(table->kval[i]->key, kv->key) > 0) {
            new_kv[i] = kv;
            inserted = 1;
            table->nkval++;
        }
        new_kv[i + inserted] = table->kval[i];
    }

    if (!inserted) {
        new_kv[i] = kv;
        table->nkval++;
    }

    free(table->kval);
    table->kval = new_kv;
    return 1;
}

// Dump a toml_table_t to a string. Must free return value
char *toml_dumps(toml_table_t *table) {
    if (!table)
        return 0;

    // First determine length of string, including ANSI escape codes
    int len = 0;
    toml_table_t *tab;
    for (int i = 0; i < table->ntab; i++) {
        tab = table->tab[i];
        len += strlen(tab->key) + 28; // \033[1;34m[\033[1;35m%s\033[1;34m]\033[0m\n
        for (int j = 0; j < tab->nkval; j++) {
            // "%s = \"%s\"\n"
            // %s \033[34m=\033[32m\"%s\"\033[0m\n
            len += strlen(tab->kval[j]->key) + strlen(tab->kval[j]->val) + 20;
        }
        if (i < table->ntab - 1)
            len += 1; // "\n"
    }
    len += 1; // \0

    char *toml_str = malloc(len);

    int pos = 0;
    for (int i = 0; i < table->ntab; i++) {
        tab = table->tab[i];
        sprintf(toml_str + pos, "\033[1;34m[\033[1;35m%s\033[1;34m]\033[0m\n", tab->key);
        pos += strlen(tab->key) + 28;
        for (int j = 0; j < tab->nkval; j++) {
            sprintf(toml_str + pos, "%s \033[34m= \033[32m\"%s\"\033[0m\n", tab->kval[j]->key, tab->kval[j]->val);
            pos += strlen(tab->kval[j]->key) + strlen(tab->kval[j]->val) + 20;
        }
        if (i < table->ntab - 1) {
            sprintf(toml_str + pos, "\n");
            pos += 1; // "\n"
        }
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
        if (i < table->ntab - 1) {
            fprintf(fp, "\n");
        }
    }
}
