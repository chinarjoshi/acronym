#include "parse_args.h"
#include <argp.h>

const char *argp_program_version = "0.1.0";
const char *argp_program_bug_address = "chinarjoshi7@gmail.com";

struct argp_option add_options[] = {
    { "alias", 'a', "ALIAS", 0, "Optionally provide alias" },
    { "section", 's', "SECTION", 0, "Optionally provide section/category" },
    { "include-flags", 'i', 0, 0, "Include flags when generating default alias" },
    { "local", 'l', 0, 0, "Change env file in current directory" },
    { 0 }
};
struct argp add_argp = { add_options, add_parse_opt }; 

struct argp_option remove_options[] = {
    { "section", 's', 0, 0, "Remove a whole section instead of alias(es)" },
    { "force", 'f', 0, 0, "Never prompt, even if alias not found" },
    { "interactive", 'i', 0, 0, "Prompt before every removal" },
    { "local", 'l', 0, 0, "Change env file in current directory" },
    { 0 }
};
struct argp remove_argp = { remove_options, remove_parse_opt }; 

struct argp_option show_options[] = {
    { "section", 's', 0, 0, "Show aliases in a given section" },
    { "local", 'l', 0, 0, "Show only directory-specific aliases" },
    { "commit", 'c', "HASH", 0, "Show aliases from commit HASH in git index" },
    { 0 }
};
struct argp show_argp = { show_options, show_parse_opt }; 

struct argp_option edit_options[] = {
    { "editor", 'e', "EDITOR", 0, "Name of editor to use" },
    { "local", 'l', 0, 0, "Edit env file in current directory" },
    { 0 }
};
struct argp edit_argp = { edit_options, edit_parse_opt }; 

struct argp_option sync_options[] = {
    { "rollback", 'r', "N", OPTION_ARG_OPTIONAL, "Rollback alias file by N versions (default: 1)" },
    { "forward", 'f', "N", OPTION_ARG_OPTIONAL, "Forward alias file by N versions (default: 1)"},
    { 0 }
};
struct argp sync_argp = { sync_options, sync_parse_opt }; 

struct argp_option reccomend_options[] = {
    { "num-recs", 's', "N", 0, "Give the top N reccomended aliases (default: 3)" },
    { "interactive", 'i', 0, 0, "Automatically add reccomended aliases while prompting" },
    { 0 }
};
struct argp reccomend_argp = { reccomend_options, reccomend_parse_opt }; 

ArgpSubcmd argp_subcmds[] = {
    {"add", ADD, &add_argp},
    {"remove", REMOVE, &remove_argp},
    {"show", SHOW, &show_argp},
    {"edit", EDIT, &edit_argp},
    {"sync", SYNC, &sync_argp},
    {"reccomend", RECCOMEND, &reccomend_argp},
};
