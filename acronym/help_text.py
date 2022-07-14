"""Workaround: putting help text in program file allows for escape codes."""

HELP_TEXT = '''Usage: \033[1macronym\033[0m [OPTIONS] COMMAND [ARGS]...

\033[1mNote\033[0m: The main file, aliases.toml, is structured as the following:

[jupyter]
jn = "jupyter notebook"
jl = "jupyter lab"

Where [jupyter] is the section, jn is the alias, and "jupyter notebook" is the command.

\033[1mOptions:\033[0m
  add ... \033[1m--flags\033[0m    Include command line flags in auto-generated acronym.
  rm ...  \033[1m--section\033[0m  Delete whole sections instead of aliases from aliases.toml.
  \033[1m-h\033[0m, \033[1m--help\033[0m       Show this message and exit.



\033[1mCommands:\033[0m
  \033[91madd\033[0m       Add provided CMD with auto-generated alias, or add multiple with comma seperation.
            Keywords: "CMD \033[1mas\033[0m ALIAS" to give custom ALIAS.
                      "CMD \033[1munder\033[0m SECTION" to give custom SECTION for organization purposes.
            See usage examples for more explaination.
  \033[93mrm\033[0m        Remove provided aliases.
  \033[92medit\033[0m      Directly edit aliases.toml with $EDITOR.
  \033[96mchange\033[0m    Change OLD alias name with NEW.
  \033[94msuggest\033[0m   Suggest pre-defined aliases based on shell command history.
  \033[95mprint\033[0m     Pretty print given sections of aliases.toml, or print all contents
            if no args given.
  

\033[1mUsage Examples:\033[0m
  Add "git reset --hard" as an acronymed alias (ignoring flags)
  $ \033[93macronym add git reset --hard\033[0m
  \033[96mgr = "git reset --hard"\033[0m

  Add cmd (including flags) using "--flags" flag
  $ \033[93macronym add git reset --hard --flags\033[0m
  \033[96mgrh = "git reset --hard"\033[0m

  Add cmd with custom alias name "greset" using "as" keyword
  $ \033[93macronym add git reset --hard as greset\033[0m
  \033[96mgreset = "git reset --hard" \033[0m

  Add cmd under section "etc", instead of section "git" using "under" keyword
  $ \033[93macronym add git reset --hard under etc\033[0m
  \033[96mgr = "git reset --hard"\033[0m

  Add multiple aliases by comma seperation (with same rules as above)
  $ \033[93macronym add git reset --hard --flags, jupyter notebook\033[0m
  \033[96mgrh = "git reset --hard"\033[0m
  \033[96mjn = "jupyter notebook"\033[0m

  Remove aliases "gc" and "asdf"
  $ \033[93macronym rm gc asdf\033[0m

  Remove sections "jupyter" and "etc" 
  $ \033[93macronym rm jupyter etc --section\033[0m

  Print sections "pip" and "apt"
  $ \033[93macronym print pip apt\033[0m
  \033[96m[pip]
  ...

  [apt]
  ...\033[0m
  '''
