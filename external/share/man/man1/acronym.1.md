---
title: acronym
section: 1
header: User Manual
footer: acronym 1.0
date: July 2022
---

# NAME

acronym - A smart alias management system to shorten your shell commands 

# SYNOPSIS

acronym \[OPTIONS] COMMAND \[ARGS]...

## Command options

**add**
: Add provided CMD with auto-generated alias, or add multiple with comma seperation.
Keywords: "CMD as ALIAS" to give custom ALIAS. "CMD under SECTION" to give custom SECTION for organization purposes. See usage examples for more explaination.

**rm**
: Remove provided aliases.

**edit**
: Directly edit aliases.toml with $EDITOR.

**change**
: Change OLD alias name with NEW.

**suggest**
: Suggest pre-defined aliases based on shell command history.

**print**
: Pretty print given sections of aliases.toml, or print all contents if no args given.

# DESCRIPTION

Acronym is a command line tool to manage your shell aliases. The tool auto-generates the most succinct name possible from any number of input commands, and lets you provide custom names for the commands. A data file, aliases.toml, is automatically managed and can be directly edited.

Note: The data file, aliases.toml, is structured like the following:


| \[jupyter]
| jn = "jupyter notebook"
| jl = "jupyter lab"

Where \[jupyter] is the section, jn is the alias, and "jupyter notebook" is the command.

# OPTIONS

add ... **--flags**    
: Include command line flags in auto-generated acronym.

rm ...  **--section**
: Delete whole sections instead of aliases from aliases.toml.

**-h**, **--help**
: Displays help messaeg.

# EXAMPLES

| Add "git reset --hard" as an acronymed alias (ignoring flags)
| $ **acronym add git reset --hard**
| gr = "git reset --hard"

| Add cmd (including flags) using "--flags" flag
| $ **acronym add git reset --hard --flags**
| grh = "git reset --hard"

| Add cmd with custom alias name "greset" using "as" keyword
| $ **acronym add git reset --hard as greset**
| greset = "git reset --hard" 

| Add cmd under section "etc", instead of section "git" using "under" keyword
| $ **acronym add git reset --hard under etc**
| gr = "git reset --hard"

| Add multiple aliases by comma seperation (with same rules as above)
| $ **acronym add git reset --hard --flags, jupyter notebook**
| grh = "git reset --hard"
| jn = "jupyter notebook"

| Remove aliases "gc" and "asdf"
| $ **acronym rm gc asdf**

| Remove sections "jupyter" and "etc" 
| $ **acronym rm jupyter etc --section**

 
| Print sections "pip" and "apt"
| $ **acronym print pip apt**
| \[pip]
| ...

| \[apt]
| ...

# BUGS

Bug tracker: https://github.com/chinarjoshi/acronym/issues
