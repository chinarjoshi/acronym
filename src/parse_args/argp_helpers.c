#include "parse_args.h"
#include <argp.h>

const char *argp_program_version = "0.1.0";
const char *argp_program_bug_address = "chinarjoshi7@gmail.com";

// TODO: Flesh out these documentation strings
struct argp_option create_options[] = {
    { "project", 'p', 0, 0, "Add to project-wide aliases file", 0 },
    { "local", 'l', 0, 0, "Add to local project-wide aliases file", 1 },
    { "alias", 'a', "ALIAS", 0, "Optionally provide alias", 2 },
    { "section", 's', "SECTION", 0, "Optionally provide section/category", 3 },
    { "comment", 'c', "COMMENT", 0, "Optionally provide comment or description", 4 },
    { "include-flags", 'i', 0, 0, "Include flags when generating default alias", 5 },
    { 0, 'h', 0, OPTION_HIDDEN, "Show this help message", 0 },
    { 0 }
};
struct argp create_argp = { create_options, create_parse_opt, "[COMMAND]", 
    "Adds a new alias to the database by providing the command.", 0, 0, 0 };

struct argp_option delete_options[] = {
    { "project", 'p', 0, 0, "Remove from project-wide aliases file", 0 },
    { "local", 'l', 0, 0, "Remove from local project-wide aliases file", 1 },
    { "section", 's', 0, 0, "Remove whole sections instead of aliases", 2 },
    { "force", 'f', 0, 0, "Never prompt, even if alias not found", 3 },
    { "interactive", 'i', 0, 0, "Prompt before every removal", 4 },
    { 0, 'h', 0, OPTION_HIDDEN, "Show this help message", 0 },
    { 0 }
};
struct argp delete_argp = { delete_options, delete_parse_opt, "[ALIAS/SECTION ...]", 
    "Remove aliases or whole sections from the database.", 0, 0, 0 };

struct argp_option read_options[] = {
    { "global", 'g', 0, 0, "Include your global aliases, even when in a git repository", 0 },
    { "aliases", 'a', 0, 0, "Make patterns match aliases", 1 },
    { "sections", 's', 0, 0, "Make patterns match sections", 2 },
    { 0, 'h', 0, OPTION_HIDDEN, "Show this help message", 0 },
    { 0 }
};
struct argp read_argp = { read_options, read_parse_opt, 
    "[PATTERN ...]", 
    "Shows your aliases in TOML format, optionally filtered by the given patterns.", 
    0, 0, 0 };

struct argp_option update_options[] = {
    { "project", 'p', 0, 0, "Edit the project-wide aliases file", 0 },
    { "local", 'l', 0, 0, "Edit the local project-wide aliases file", 1 },
    { "editor", 'e', "EDITOR", 0, "Name of editor to use", 2 },
    { 0, 'h', 0, OPTION_HIDDEN, "Show this help message", 0 },
    { 0 }
};
struct argp update_argp = { update_options, update_parse_opt, 0, 
    "Edit the database in TOML format using your default editor.", 0, 0, 0 };

ArgpSubcmd argp_subcmds[] = {
    {"create", CREATE, &create_argp},
    {"read", READ, &read_argp},
    {"update", UPDATE, &update_argp},
    {"delete", DELETE, &delete_argp},
};

const char *long_help_message = \
"acronym 1.0.0\n"
"Chinar Joshi <chinarjoshi7@gmail.com>\n"
"\n"
"acronym syncs shell usage between development environments through aliases.\n"
"\n"
"There are 3 alias database scopes:\n"
"\n"
"  global        defines aliases available everywhere\n"
"  project-wide  defines aliases specific to a git-tracked directory, which should be\n"
"                     version-controlled and used by all contributors.\n"
"  local         same as above, but is local to your machine and in git ignore\n"
"\n"
"Every entry in the database has the following string fields:\n"
"\n"
"  alias     the new command you type\n"
"  command   the actual command the alias runs\n"
"  section   the category the alias falls under (for organization)\n"
"  comment   an optional description\n"
"\n"
"Usage: acronym [OPTION ...] [SUBCOMMAND]\n"
"\n"
"  -q, --quiet      Suppress output\n"
"  -v, --verbose    Use verbose output\n"
"  -h, --help       Give this help list\n"
"  -V, --version    Print program version\n"
"\n"
"There are 4 subcommands (CRUD):\n"
"\n"
"  create   Adds a new alias to the database by providing the command.\n"
"  read     Shows your aliases in TOML format, optionally filtered by the given patterns.\n"
"  update   Edit the database in TOML format using your default editor.\n"
"  delete   Remove aliases or whole sections from the database.\n";

const char *short_help_message = \
"Usage: acronym [OPTION ...] [SUBCOMMAND]\n"
"\n"
"  -q, --quiet      Suppress output\n"
"  -v, --verbose    Use verbose output\n"
"  -h, --help       Give full help list\n"
"  -V, --version    Print program version\n"
"\n"
"There are 4 subcommands (CRUD):\n"
"\n"
"  create   Adds a new alias to the database by providing the command.\n"
"  read     Shows your aliases in TOML format, optionally filtered by the given patterns.\n"
"  update   Edit the database in TOML format using your default editor.\n"
"  delete   Remove aliases or whole sections from the database.\n";
