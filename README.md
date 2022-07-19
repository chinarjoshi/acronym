[![Contributors][contributors-shield]][contributors-url]
[![Forks][forks-shield]][forks-url]
[![Stargazers][stars-shield]][stars-url]
[![Issues][issues-shield]][issues-url]
[![MIT License][license-shield]][license-url]

<!-- PROJECT LOGO -->
<br />
<p align="center">
  <a href="https://github.com/chinarjoshi/acronym">
  </a>
  <h1 align="center">Acronym</h1>
  <p align="center">
    A smart alias management system to shorten your shell commands.
    <br />
    <a href="https://github.com/chinarjoshi/acronym"><strong>Explore the docs »</strong></a>
    <br />
    <br />
    <a href="https://github.com/chinarjoshi/acronym">Report Bug</a>
    ·
    <a href="https://github.com/chinarjoshi/acronym">Request Feature</a>
  </p>
</p>

## Elevator pitch

`alias` is a POSIX shell command that replaces a single word with a string. As [linuxize](https://linuxize.com/post/how-to-create-bash-aliases/) describes about `alias`:

> If you often find youself typing a long command on the terminal, then you will find bash aliases handy... Bash aliases are essentially shortcuts that can save you from having to remember long commands and eliminate a great deal of typing when you are working on the command line.

So aliases are no doubt a boon for productive shell usage. The problem *acronym* attempts to solves is the difficulty keeping track of what aliases you've defined in your possibly long shell configuration, and the difficulty maintaining a consistent naming pattern.

*Acronym* solves this by greatly simplifying the process of defining new aliases in a standard and efficient way. Instead of having to edit your shell configuration, pick a memorable name that doesn't conflict with other aliases, and add the `alias` command, you would simply use the `acronym add` invocation to automatically use the command's acronym, or see <a href="#Usage">usage</a> for greater versatility.


# Demonstration

This demo showcases how a very long command with `sudo` and specific flags can be easily shortened to a two letter alias.

https://user-images.githubusercontent.com/68311366/179607402-bbbd1114-0cf8-4aa3-b20d-1b6989ee0e26.mp4

The best way to do this example without *acronym* in my opnion is:

```
echo 'alias pu="sudo pacman -Syu --noconfirm --color=auto"' >> /path/to/aliases.sh
```

To view them, `cat /path/to/aliases.sh`, where the output is in the format 
```
alias a="b"
alias m="n"
alias x="y"
```
And while this was the system I used before writing this tool, *acronym* allows for much needed abstraction. Note that the *acronym* commands come pre-registered for convenience, so to add an alias is `aa x`, to change the alias name is `ac x with y`, to remove it is `ar y`, and to print your aliases in toml format is `ap`, where the output is in the format
```
[acronym]
aa = "acronym add"
ar = "acronym rm"
ae = "acronym edit"
...

[pacman]
pu = "sudo pacman -Syu --noconfirm --colo=auto"
```

## Installation

1. Install package

* With pip:

`pip install acronym-alias`

* With AUR helper:

`yay -S acronym`

2. Source the aliases in shellrc

* With install script

`acronym install`

* Manually edit rc (use `pip show acronym` to find install dir, which is either under `~/.local/lib/...` or `/usr/lib/...`)

`. ~/.local/lib/python3.10/site-packages/acronym/data/aliases.sh`

If you're using zsh and want completion, add this line too:

`fpath+=(~/.local/share/zsh/site-functions)`


## Usage

```
Usage: acronym [OPTIONS] COMMAND [ARGS]...

Note: The main file, aliases.toml, is structured as the following:

[jupyter]
jn = "jupyter notebook"
jl = "jupyter lab"

Where [jupyter] is the section, jn is the alias, and "jupyter notebook" is the command.

Options:
  add ... --flags    Include command line flags in auto-generated acronym.
  rm ...  --section  Delete whole sections instead of aliases from aliases.toml.
  -h, --help       Show this message and exit.


Commands:
  add       Add provided CMD with auto-generated alias, or add multiple with comma seperation.
            Keywords: "CMD as ALIAS" to give custom ALIAS.
                      "CMD under SECTION" to give custom SECTION for organization purposes.
            See usage examples for more explaination.
  rm        Remove provided aliases.
  edit      Directly edit aliases.toml with $EDITOR.
  change    Change OLD alias name with NEW.
  suggest   Suggest pre-defined aliases based on shell command history.
  print     Pretty print given sections of aliases.toml, or print all contents
            if no args given.


Usage Examples:
  Add "git reset --hard" as an acronymed alias (ignoring flags)
  $ acronym add git reset --hard
  gr = "git reset --hard"

  Add cmd (including flags) using "--flags" flag
  $ acronym add git reset --hard --flags
  grh = "git reset --hard"

  Add cmd with custom alias name "greset" using "as" keyword
  $ acronym add git reset --hard as greset
  greset = "git reset --hard"

  Add cmd under section "etc", instead of section "git" using "under" keyword
  $ acronym add git reset --hard under etc
  gr = "git reset --hard"

  Add multiple aliases by comma seperation (with same rules as above)
  $ acronym add git reset --hard --flags, jupyter notebook
  grh = "git reset --hard"
  jn = "jupyter notebook"

  Remove aliases "gc" and "asdf"
  $ acronym rm gc asdf

  Remove sections "jupyter" and "etc"
  $ acronym rm jupyter etc --section

  Edit the configuration file
  $ acronym edit

  Replace alias "gr" to "greset" without changing its command
  $ acronym change gr with greset

  Get suggestions for more aliases based on shell history file
  $ acronym suggest

  Print sections "pip" and "apt"
  $ acronym print pip apt
  [pip]
  ...

  [apt]
  ...
```

<!-- MARKDOWN LINKS & IMAGES -->
[contributors-shield]: https://img.shields.io/github/contributors/chinarjoshi/visual-aid-transducer?style=for-the-badge
[contributors-url]: https://github.com/chinarjoshi/visual-aid-transducer/graphs/contributors
[forks-shield]: https://img.shields.io/github/forks/chinarjoshi/visual-aid-transducer?style=for-the-badge
[forks-url]: https://github.com/chinarjoshi/visual-aid-transducer/network/members
[stars-shield]: https://img.shields.io/github/stars/chinarjoshi/visual-aid-transducer?style=for-the-badge
[stars-url]: https://github.com/chinarjoshi/visual-aid-transducer/stargazers
[issues-shield]: https://img.shields.io/github/issues/chinarjoshi/visual-aid-transducer?style=for-the-badge
[issues-url]: https://github.com/chinarjoshi/visual-aid-transducer/issues
[license-shield]: https://img.shields.io/github/license/chinarjoshi/visual-aid-transducer?style=for-the-badge
[license-url]: https://github.com/chinarjoshi/visual-aid-transducer/blob/master/LICENSE
[linkedin-shield]: https://img.shields.io/badge/-LinkedIn-black.svg?style=for-the-badge&logo=linkedin&colorB=555
[product-screenshot]: images/screenshot.png
