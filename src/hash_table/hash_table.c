#include <stdlib.h>
#include <string.h>
#include "entry.h"
#include "hash_table.h"
#include "../subcmds/subcmds.h"

const int INITIAL_CAPACITY = 51;
const float LOAD_FACTOR = .5;
Status create_hash_table(HashTable **data_out, int capacity,
                          float load_factor) {
    HashTable *ht = malloc(sizeof(HashTable));
    if (!ht)
        return ERR_OUT_OF_MEMORY;

    if (!(ht->backing_array = calloc(capacity, sizeof(Entry)))) {
        free(ht);
        return ERR_OUT_OF_MEMORY;
    }

    ht->size = 0;
    ht->capacity = capacity;
    ht->load_factor = load_factor;
    *data_out = ht;
    return SUCCESS;
}

Status add_entry(Entry *data, HashTable *ht) {
    // Check for resize
    if ((float)(ht->size + 1) / ht->capacity > ht->load_factor) {
        Status s = resize_backing_array(ht);
        if (s == ERR_OUT_OF_MEMORY)
            return ERR_OUT_OF_MEMORY;
    }
    
    int count = 0;
    int hash = hash_alias(data->alias, ht->capacity);

    // Find the first hole or removed value;
    while (ht->backing_array[hash] && !ht->backing_array[hash]->is_removed) {
        // If 'size' elements checked, then resize
        if (count > ht->size) {
            Status s = resize_backing_array(ht);
            if (s == ERR_OUT_OF_MEMORY)
                return ERR_OUT_OF_MEMORY;
        }

        // If key is in table, then override it and return ERR
        if (strcmp(ht->backing_array[hash]->alias, data->alias) == 0) {
            free(ht->backing_array[hash]);
            ht->backing_array[hash] = data;
            return ERR_DUPLICATE;
        }

        hash = (hash + 1) % ht->capacity;
        count++;
    }

    ht->backing_array[hash] = data;
    ht->size++;
    return SUCCESS;
}

Status remove_entry(Entry **data_out, char *alias, HashTable *ht) {
    if (ht->size == 0)
        return ERR_NOT_FOUND;

    int hash = hash_alias(alias, ht->capacity);
    while (ht->backing_array[hash] && !ht->backing_array[hash]->is_removed) {
        if (strcmp(ht->backing_array[hash]->alias, alias) == 0) {
            ht->backing_array[hash]->is_removed = true;
            *data_out = ht->backing_array[hash];
            ht->size--;
            return SUCCESS;
        }

        hash = (hash + 1) % ht->capacity;
    }

    return ERR_NOT_FOUND;
}

Status remove_section(char *section, HashTable *ht) {
    if (ht->size == 0)
        return ERR_NOT_FOUND;

    bool section_found = false;
    for (int i = 0; i < ht->capacity; i++) {
        if (ht->backing_array[i] && 
                strcmp(ht->backing_array[i]->section, section) == 0) {
            section_found = true;
            ht->backing_array[i]->is_removed = true;
            ht->size--;
        }
    }

    return (section_found) ? SUCCESS : ERR_NOT_FOUND;
}

Status resize_backing_array(HashTable *ht) {
    int new_capacity = ht->capacity * 2 + 1;
    Entry **new_array = calloc(new_capacity, sizeof(Entry));
    if (!new_array)
        return ERR_OUT_OF_MEMORY;

    int old_capacity = ht->capacity;
    Entry **old_array = ht->backing_array;

    ht->size = 0;
    ht->capacity = new_capacity;
    ht->backing_array = new_array;

    // Loop over array. If entry is non-null and not removed, then add it.
    // If entry is removed, then free it.
    for (int i = 0; i < old_capacity; i++)
        if (old_array[i]) {
            if (!old_array[i]->is_removed)
                add_entry(old_array[i], ht);
            else
                free_entry(old_array[i]);
        }

    free(old_array);
    return SUCCESS;
}

void empty_hash_table(HashTable *ht) {
    if (ht->size == 0)
        return;
    for (int i = 0; i < ht->capacity; i++) {
        if (ht->backing_array[i]) {
            free_entry(ht->backing_array[i]);
            ht->backing_array[i] = NULL;
        }
    }
}

void free_hash_table(HashTable *ht) {
    empty_hash_table(ht);
    free(ht->backing_array);
    free(ht);
}
