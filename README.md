[![Contributors][contributors-shield]][contributors-url]
[![Forks][forks-shield]][forks-url]
[![Stargazers][stars-shield]][stars-url]
[![Issues][issues-shield]][issues-url]
[![MIT License][license-shield]][license-url]
[![LinkedIn][linkedin-shield]][linkedin-url]

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
    <a href="https://github.com/chinarjoshi/visual-aid-transducer/issues">Report Bug</a>
    ·
    <a href="https://github.com/chinarjoshi/visual-aid-transducer/issues">Request Feature</a>
  </p>
</p>

<!-- TABLE OF CONTENTS -->
<details open="open">
  <summary>Table of Contents</summary>
  <ol>
    <li>
      <a href="#about-the-project">Elevator pitch</a>
    </li>
    <li><a href="#usage">Usage</a></li>
    <li><a href="#license">License</a></li>
    <li><a href="#contact">Contact</a></li>
    <li><a href="#acknowledgements">Acknowledgements</a></li>
    <li><a href="#gallery">Gallery</a></li>
  </ol>
</details>

## Elevator pitch

As [linuxize](https://linuxize.com/post/how-to-create-bash-aliases/) describes about aliases:

> If you often find youself typing a long command on the terminal, then you will find bash aliases handy... Bash aliases are essentially shortcuts that can save you from having to remember long commands and eliminate a great deal of typing when you are working on the command line.

The problem *acronym* solves is the difficulty keeping track of what aliases you've defined in your shell configuration, and greatly simplifies the process of defining new aliases in a standard and efficient way.


An acronym *(abbreviation formed from the initial letters of words)* is extracted from any input command, which is the most efficient way to remember a string. For example, `git reset --hard` is aliased to `grh`, which reduces typing from 16 characters to 3 characters (5x less typing).

Every input command will have exactly one output alias through the naming rules, and in case of collisions asks the user for a custom name.

# Installation


1. Install package

* With pip:

`pip install acronym-alias`

* With AUR helper:

`yay -S acronym`

2. Source the aliases in shellrc

`acronym install`


# Usage

commands:
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

  Print sections "pip" and "apt"
  $ acronym print pip apt
  [pip]
  ...

  [apt]
  ...

<!-- MARKDOWN LINKS & IMAGES -->
<!-- https://www.markdownguide.org/basic-syntax/#reference-style-links -->
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
[linkedin-url]: https://www.linkedin.com/in/chinar-joshi-905493207/
[product-screenshot]: images/screenshot.png
