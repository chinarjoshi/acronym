# acronym
A program that syncs shell usage between development environments through ***aliases***. It implements CRUD operations on databases of aliases at the global, project-wide, and local project-wide scopes.

## Rational
Development environments are notoriously hard to keep consistent. One component of this problem is difficulty establishing and remembering the commands to develop, build, and run your software, often with
lengthy, awkward, and mistakable flags that are a common source of error. I see 2 good solutions:
1. *Directory specific environments*: This age-old solution uses a shell script to set environmental variables to tame the most complex of build flags. But one major problem, environmental files should NOT be
   version-controlled because it's a common security hazard for secrets. This means the environment and aliases must be synchronized seperately.
3. *Containerization*: The ideal solution is dockerize the development environment for perfect consistency, but is an unpopular practice because of the added complexity and performance impact.

My solution is a version-controlled file strictly containing aliases for commands to be used by the whole team, named **.aliases.sh**. This comes with significant advantages
1. *Enforces consistent shell usage across development environments during collaborating*: everyone is running identical commands, removing a component of the *"well it works on my machine"* problem
2. *Allows a uniform interface to all of your projects, regardless of the language or build system*: `build` can mean make, cmake, ninja, meson, scons, gradle, etc. based on the current project. Remember what the
   tool does, instead of its name
4. *Encourages ergonomic and memorable names to your long commands*: typing `l` instead of `ls -ghAFG --color=auto` reduces typos and saves time

## Usage

## Installation
