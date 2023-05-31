#include <stdio.h>
#include <stdbool.h>
#include <argp.h>
#include "parse_args.h"

const char *argp_program_version = "0.1.0";
const char *argp_program_bug_address = "chinarjoshi7@gmail.com";

struct Add {
    char *command;
    char *alias_override;
    char *section_override;
    bool include_flags;
    bool local;
};

struct Remove {
    char **aliases;
    bool whole_section;
    bool force;
    bool interactive;
    bool local;
};

union Subcommand {
    struct Add add;
    struct Remove remove;
    bool edit;
};

static int parse_opt(int key, char *arg, struct argp_state *state) {
    switch (key) {
        case 'd': 
            printf(".\n");
            break;
    }
    return 0;
}

int main(int argc, char **argv) {
    struct argp_option options[] = {
        { "to", 't', "ALIAS", OPTION_ARG_OPTIONAL, "Optionally provide alias name" },
        { "include-flags", 'i', 0, 0, "Include flags when generating default alias" },
        { "set", 's', "SET", OPTION_ARG_OPTIONAL, "Optionally change table in .acronym.toml" },
        {0},
    };
    struct argp argp = { options, parse_opt }; 
    return argp_parse(0, argc, argv, 0, 0, 0);
}
