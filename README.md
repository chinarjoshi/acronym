# acronym

Acronym is a smart system to shorten your shell commands.

As [linuxize](https://linuxize.com/post/how-to-create-bash-aliases/) describes:

> Do you often find yourself typing a long command on the command line or searching the bash history for a previously typed command? If your answer to any of those questions is yes, then you will find bash aliases handy... Bash aliases are essentially shortcuts that can save you from having to remember long commands and eliminate a great deal of typing when you are working on the command line. 

However, this very useful feature is often underused because the alias itself can be easily forgotten and it is difficult to establish a strict pattern between your aliases. But enter acronym...

## Quick Pitch

1. Optimized

An acronym *(abbreviation formed from the initial letters of words)* is extracted from any input command + options, which is the most efficient way to remember a string. For example, `git reset --hard` is aliased to `grh`, which reduces typing from 16 characters to 3 characters (5x less typing).

2. Standardized

Every input command will have exactly one output alias through the two strict naming rules, and in case of collisions asks the user for a custom name.

3. Configurable

The `acronym.toml` file contains all configuration options, and no values are hard coded. The script creates an `aliases.json` file that represents the [trie](https://stackoverflow.com/questions/4737904/difference-between-tries-and-trees) data structure of stored aliases which can be manually or automatically edited through the command line interface.

4. Convenient

Acronym offers a large degree of convenience to quicken your work flow in the command line. You could try to manually alias every common command from your memory, but acronym goes a step further by analyzing your shell history file for reccomendations and providing an intuitive command line interface and pretty printed alias cheatsheet. 

# Installation

# Usage

Register an alias:

`acronym register *cmd*`
`acronym r *cmd`

Or many at a time while giving a custom alias:

`acronym register *cmd1*, *cmd2* as *custom*, *cmd3*`

Unregister an alias:

`acronym`

`acronym register git commit, git clone as gcl, pip install`

Print managed aliases:

`acronym cheatsheet`


