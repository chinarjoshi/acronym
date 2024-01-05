# acronym
A program that syncs shell usage between development environments through ***aliases***. It implements CRUD operations on databases of aliases at the global, project-wide, and local project-wide scopes.

## Rational
Development environments are notoriously hard to keep consistent. One component is the difficulty establishing and remembering the commands to make your software, which often have
long and awkward flags that are a common source of error. 

My solution is a version-controlled file strictly containing aliases for commands to be used by the whole team, named **.aliases.sh**.
It can be version-controlled because its devoid of secrets, and it dodges the complexity and performance impact of a container. This comes with significant advantages:
1. *Enforces consistent shell usage during collaborating*: everyone is running identical commands, removing a component of the *"well it works on my machine"* problem
2. *Allows a uniform interface to all of your projects, regardless of the language or build system*: `build` can mean make, cmake, ninja, meson, scons, gradle, etc. based on the current project.
    Remember what the tool does, instead of its name
3. *Encourages ergonomic names to your long commands*: typing `l` instead of `ls -ghAFG --color=auto` reduces typos and saves time

## Usage

There are 4 operations (CRUD), and 3 scopes of databases. 
1. *global* defines aliases available everywhere. Default: `~/.aliases.sh`
2. *project-wide* defines aliases specific to a git-tracked directory, which should be version-controlled and used by all contributors. Example: `~/projects/example/.aliases.sh`
3. *local project-wide* is same as above, but is local to your machine and specified in the git ignore file.  Example: `~/projects/example/.env`

### `acronym create`



### `acronym read`

### `acronym update`

### `acronym delete`


## Installation
