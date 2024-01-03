#include "parse_args.h"
#include <argp.h>

const char *argp_program_version = "0.1.0";
const char *argp_program_bug_address = "chinarjoshi7@gmail.com";

struct argp_option add_options[] = {
    { "alias", 'a', "ALIAS", 0, "Optionally provide alias" },
    { "section", 's', "SECTION", 0, "Optionally provide section/category" },
    { "comment", 'c', "COMMENT", 0, "Optionally provide comment or description" },
    { "include-flags", 'i', 0, 0, "Include flags when generating default alias" },
    { "project", 'p', 0, 0, "Add to aliases file in current project" },
    { "local", 'l', 0, 0, "Add to your local aliases file in current project" },
    { 0 }
};
struct argp add_argp = { add_options, add_parse_opt }; 

struct argp_option remove_options[] = {
    { "section", 's', 0, 0, "Remove a whole section instead of alias(es)" },
    { "force", 'f', 0, 0, "Never prompt, even if alias not found" },
    { "interactive", 'i', 0, 0, "Prompt before every removal" },
    { "project", 'p', 0, 0, "Remove from aliases file in current project" },
    { "local", 'l', 0, 0, "Remove from your local aliases file in current project" },
    { 0 }
};
struct argp remove_argp = { remove_options, remove_parse_opt }; 

struct argp_option show_options[] = {
    { "global", 'g', 0, 0, "Include your global aliases, even when in a git repository" },
    { "aliases", 'a', 0, 0, "Make arguments match only alias names" },
    { "sections", 's', 0, 0, "Make arguments match only section names" },
    { 0 }
};
struct argp show_argp = { show_options, show_parse_opt }; 

struct argp_option edit_options[] = {
    { "editor", 'e', "EDITOR", 0, "Name of editor to use" },
    { "project", 'p', 0, 0, "Edit aliases file in current project" },
    { "local", 'l', 0, 0, "Edit your local aliases file in current project" },
    { 0 }
};
struct argp edit_argp = { edit_options, edit_parse_opt }; 

ArgpSubcmd argp_subcmds[] = {
    {"add", ADD, &add_argp},
    {"remove", REMOVE, &remove_argp},
    {"show", SHOW, &show_argp},
    {"edit", EDIT, &edit_argp},
};
