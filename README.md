# acronym
A tool that syncs shell usage between development environments through [***aliases***](https://tldp.org/LDP/abs/html/aliases.html).

## Rational
Development environments are notoriously hard to keep consistent. One component is the difficulty establishing and remembering the commands to make your software, which often have
long and awkward flags that are a common source of error. 

This solution is a new version-controlled file strictly containing aliases for commands to be used by the whole team, named `.aliases.sh`.
It can be version-controlled because its devoid of secrets, and it dodges the complexity and performance impact of a container. This comes with significant advantages:
1. *Enforces consistent shell usage when collaborating*: everyone is running identical commands, which simplifies new developer onboarding and removes a component of the *"well it works on my machine"* problem
2. *Allows a uniform interface to all of your projects, regardless of the language or build system*: `build` can mean make, cmake, ninja, meson, scons, gradle, etc. based on the current project.
    Remember what the tool does, instead of its name
3. *Encourages ergonomic names to your long commands*: typing `l` instead of `ls -ghAFG --color=auto` reduces typos and saves time

## Usage

Acronym manages *databases* of aliases that compile to plain shell scripts. Only one person needs the `acronym` executable for everyone to benefit. To use global aliases, add this command to your .bashrc/.zshrc: `. $(acronym -g)`[^1].
To use project-wide aliases, all contributors can simply source the `.aliases.sh` file located in your git repository, either manually or with [autoenv](https://github.com/hyperupcall/autoenv).

There are 3 alias database scopes:
1. *global* defines aliases available everywhere. Default: `~/.aliases.sh`
2. *project-wide* defines aliases specific to a git-tracked directory, which should be version-controlled and used by all contributors. Example: `~/projects/example/.aliases.sh`
3. *local project-wide* is same as above, but is local to your machine and specified in the git ignore file [^2].  Example: `~/projects/example/.env`

Every entry in the database has the following string fields:
 - alias: the new command you type
 - command: the actual command the alias runs
 - section: the category the alias falls under (for organization)
 - comment: an optional description

There are 4 database operations (CRUD) [^3]:

### `acronym create [OPTION ...] [COMMAND]`

Adds a new alias to the specified database scope by providing the command. By default, it will auto-generate the alias from the acronym of the command, and the section from the first word of the command
(unless its sudo), but you can provide the alias, section, and comment. The scope is *global* by default.
```
  -p, --project              Add to project-wide alias database
  -l, --local                Add to local project-wide alias database
  -a, --alias=ALIAS          Optionally provide alias
  -c, --comment=COMMENT      Optionally provide a description
  -s, --section=SECTION      Optionally provide section/category
  -i, --include-flags        Include flags when generating default alias
```
![create_demo2](https://github.com/chinarjoshi/acronym/assets/68311366/9f21e4e5-925d-4e45-946d-ce5ffd6c4bb8)

### `acronym read [OPTION ...] [PATTERN ...]`

Shows your aliases in *TOML format*, optionally filtered by the given patterns. If you're in a git repository, it will show the *project-wide* and *local project-wide* scopes.
If neither of those files exist or you're not in a git repository, it will show the *global* scope. The patterns match the *command* field by default.
```
  -g, --global               Include your global aliases, even when in a git repository
  -a, --aliases              Make patterns match aliases
  -s, --sections             Make patterns match sections
```
![read_demo](https://github.com/chinarjoshi/acronym/assets/68311366/d4f49bd5-3f84-4fe3-b0f9-dec5a6087cc3)

### `acronym update [OPTION ...]`

Edit the database in *TOML format* using your default editor. Here you can create, update, or delete entries and have the output converted back to shell script format. The scope is *global* by default.
```
  -p, --project              Edit project-wide alias database
  -l, --local                Edit local project-wide alias database
  -e, --editor=EDITOR        Name of editor to use
```
![update_demo](https://github.com/chinarjoshi/acronym/assets/68311366/30b9611b-dc1e-44bb-808b-c383dcdb743a)

### `acronym delete [OPTION ...] [ALIAS/SECTION ...]`

Remove aliases or whole sections from the database.
```
  -p, --project              Remove from project-wide alias database
  -l, --local                Remove from local project-wide alias database
  -s, --section              Remove whole sections instead of aliases
  -f, --force                Never prompt, even if alias not found
  -i, --interactive          Prompt before every removal
```
![delete_demo2](https://github.com/chinarjoshi/acronym/assets/68311366/df78d0d9-8dfb-48bb-8fd3-b4ef56c0b103)

## Installation

Binaries are available for x86_64 Linux and MacOS.

If you're an **Arch Linux** user, you can install acronym from the AUR
```
$ yay -S acronym
```

If you're a **MacOS Homebrew** user, you can install acronym from a tap
```
$ brew tap chinarjoshi/tap
$ brew install acronym
```

Otherwise, you can download the binary from the releases tab and install it with the following command
```
$ chmod +x acronym* && sudo mv acronym* /usr/local/bin/acronym
```

## Configuration

Acronym can be configured through the following environmental variables:
* `ACRONYM_FILENAME`: The name of the global and project-wide alias file. Default: `.aliases.sh`
* `ACRONYM_LOCAL_FILENAME`: The name of the local project-wide alias file. Default: `.env`
* `ACRONYM_GLOBAL_DIR`: The directory that contains the global alias file. Useful if you want it to be somewhere like dotfiles. Default: `~`

[^1]: Invoking `acronym` with no parameters prints the path to the global aliases file
[^2]: The `.env` file can still contain any other code. Local mode will just append all aliases to the end of the file, and top it off
with the command `. ./.aliases.sh`, meaning local aliases *cannot* override project-wide aliases
[^3]: The alias database incurs no performance penalty because it is converted to a plain shell script of `alias` commands for the shell to source.
