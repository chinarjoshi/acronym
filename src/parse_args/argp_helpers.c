#include "parse_args.h"
#include <argp.h>

const char *argp_program_version = "0.1.0";
const char *argp_program_bug_address = "chinarjoshi7@gmail.com";

struct argp_option create_options[] = {
    { "alias", 'a', "ALIAS", 0, "Optionally provide alias" },
    { "section", 's', "SECTION", 0, "Optionally provide section/category" },
    { "comment", 'c', "COMMENT", 0, "Optionally provide comment or description" },
    { "include-flags", 'i', 0, 0, "Include flags when generating default alias" },
    { "project", 'p', 0, 0, "Add to aliases file in current project" },
    { "local", 'l', 0, 0, "Add to your local aliases file in current project" },
    { 0, 'h', 0, OPTION_HIDDEN, "Show this help message" },
    { 0 }
};
struct argp create_argp = { create_options, create_parse_opt }; 

struct argp_option delete_options[] = {
    { "section", 's', 0, 0, "Remove a whole section instead of alias(es)" },
    { "force", 'f', 0, 0, "Never prompt, even if alias not found" },
    { "interactive", 'i', 0, 0, "Prompt before every removal" },
    { "project", 'p', 0, 0, "Remove from aliases file in current project" },
    { "local", 'l', 0, 0, "Remove from your local aliases file in current project" },
    { 0, 'h', 0, OPTION_HIDDEN, "Show this help message" },
    { 0 }
};
struct argp delete_argp = { delete_options, delete_parse_opt }; 

struct argp_option read_options[] = {
    { "global", 'g', 0, 0, "Include your global aliases, even when in a git repository" },
    { "aliases", 'a', 0, 0, "Make arguments match only alias names" },
    { "sections", 's', 0, 0, "Make arguments match only section names" },
    { 0, 'h', 0, OPTION_HIDDEN, "Show this help message" },
    { 0 }
};
struct argp read_argp = { read_options, read_parse_opt }; 

struct argp_option update_options[] = {
    { "editor", 'e', "EDITOR", 0, "Name of editor to use" },
    { "project", 'p', 0, 0, "Edit aliases file in current project" },
    { "local", 'l', 0, 0, "Edit your local aliases file in current project" },
    { 0, 'h', 0, OPTION_HIDDEN, "Show this help message" },
    { 0 }
};
struct argp update_argp = { update_options, update_parse_opt }; 

ArgpSubcmd argp_subcmds[] = {
    {"create", CREATE, &create_argp},
    {"read", READ, &read_argp},
    {"update", UPDATE, &update_argp},
    {"delete", DELETE, &delete_argp},
};
