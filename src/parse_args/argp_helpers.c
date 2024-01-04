#include "parse_args.h"
#include <argp.h>

const char *argp_program_version = "0.1.0";
const char *argp_program_bug_address = "chinarjoshi7@gmail.com";

// TODO: Flesh out these documentation strings
struct argp_option create_options[] = {
    { "alias", 'a', "ALIAS", 0, "Optionally provide alias", 0 },
    { "section", 's', "SECTION", 0, "Optionally provide section/category", 0 },
    { "comment", 'c', "COMMENT", 0, "Optionally provide comment or description", 0 },
    { "include-flags", 'i', 0, 0, "Include flags when generating default alias", 0 },
    { "project", 'p', 0, 0, "Add to aliases file in current project", 0 },
    { "local", 'l', 0, 0, "Add to your local aliases file in current project", 0 },
    { 0, 'h', 0, OPTION_HIDDEN, "Show this help message", 0 },
    { 0 }
};
struct argp create_argp = { create_options, create_parse_opt, "COMMAND", 
    "Create a new alias in the specified scope.", 0, 0, 0 };

struct argp_option delete_options[] = {
    { "section", 's', 0, 0, "Remove a whole section instead of alias(es)", 0 },
    { "force", 'f', 0, 0, "Never prompt, even if alias not found", 0 },
    { "interactive", 'i', 0, 0, "Prompt before every removal", 0 },
    { "project", 'p', 0, 0, "Remove from aliases file in current project", 0 },
    { "local", 'l', 0, 0, "Remove from your local aliases file in current project", 0 },
    { 0, 'h', 0, OPTION_HIDDEN, "Show this help message", 0 },
    { 0 }
};
struct argp delete_argp = { delete_options, delete_parse_opt, "[ALIAS ...]\n[SECTION ...]", 
    "Delete alias(es) or whole section(s) from the specified scope.", 0, 0, 0 };

struct argp_option read_options[] = {
    { "global", 'g', 0, 0, "Include your global aliases, even when in a git repository", 0 },
    { "aliases", 'a', 0, 0, "Make arguments match only alias names", 0 },
    { "sections", 's', 0, 0, "Make arguments match only section names", 0 },
    { 0, 'h', 0, OPTION_HIDDEN, "Show this help message", 0 },
    { 0 }
};
struct argp read_argp = { read_options, read_parse_opt, 
    "[COMMAND PATTERN ...]\n-a [ALIAS PATTERN ...]\n-s [SECTION PATTERN ...]", 
    "Read aliases from applicable scopes in TOML format, filtered by the given patterns.", 
    0, 0, 0 };

struct argp_option update_options[] = {
    { "editor", 'e', "EDITOR", 0, "Name of editor to use", 0 },
    { "project", 'p', 0, 0, "Edit aliases file in current project", 0 },
    { "local", 'l', 0, 0, "Edit your local aliases file in current project", 0 },
    { 0, 'h', 0, OPTION_HIDDEN, "Show this help message", 0 },
    { 0 }
};
struct argp update_argp = { update_options, update_parse_opt, 0, 
    "Update aliases by editing the given scope's file in TOML format.", 0, 0, 0 };

ArgpSubcmd argp_subcmds[] = {
    {"create", CREATE, &create_argp},
    {"read", READ, &read_argp},
    {"update", UPDATE, &update_argp},
    {"delete", DELETE, &delete_argp},
};
