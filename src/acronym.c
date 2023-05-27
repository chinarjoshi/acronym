#include <stdlib.h>
#include "hashtable.h"
#include <string.h>

Acronym *create_acronym(char *command) {
    Acronym *acronym;
    if (!(acronym = malloc(sizeof(Acronym)))) {
        return NULL;
    }

    if (!(acronym->command = malloc(strlen(command) + 1))) {
        free(acronym);
        return NULL;
    }

    if (!(acronym->section))

    return acronym;
}

char *create_section_name(char *command) {
    // First pass: find length of first word
    int first_word_len = 0;
    while (command[first_word_len] && command[first_word_len] != ' ') {
        first_word_len++;
    }

    // Second pass: copy over characters
    char *section = malloc(first_word_len + 1);
    if (!section) {
        return NULL;
    }
    for (int i = 0; i < first_word_len; i++) {
        section[i] = command[i];
    }
    section[first_word_len + 1] = '\0';
    return section;
}
