#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <argp.h>
#include "parse_args.h"

const char *argp_program_version = "0.1.0";
const char *argp_program_bug_address = "chinarjoshi7@gmail.com";

int global_parse_opt(int key, char *arg, struct argp_state *state) {
    if (key != ARGP_KEY_ARG) {
        return ARGP_ERR_UNKNOWN;
    }

    struct argp argp = {0};
    if (strcmp(arg, "add") == 0) {
        argp.options = add_options;
        argp.parser = add_parse_opt;
    } else if (strcmp(arg, "remove") == 0) {

    } else if (strcmp(arg, "tree") == 0) {

    } else if (strcmp(arg, "show") == 0) {

    } else if (strcmp(arg, "edit") == 0) {

    } else {
        argp_error(state, "%s is not a valid command.", arg);
        return 1;
    }
    return 0;
}

int add_parse_opt(int key, char *arg, struct argp_state *state) {
    return 0;
}

int remove_parse_opt(int key, char *arg, struct argp_state *state) {
    return 0;
}
int tree_parse_opt(int key, char *arg, struct argp_state *state) {
    return 0;
}
int show_parse_opt(int key, char *arg, struct argp_state *state) {
    return 0;
}
int edit_parse_opt(int key, char *arg, struct argp_state *state) {
    return 0;
}

struct Cli parse_args(int argc, char **argv) {
    struct Cli cli;
    struct argp global_argp = { 0, global_parse_opt }; 
    argp_parse(&global_argp, argc, argv, 0, 0, &cli);
    return cli;
}
