"""Helper functions for aliases.py."""

import os
from pathlib import Path
from dataclasses import dataclass


ROOT = Path(__file__).parent.resolve() / "data"
SH = ROOT / "aliases.sh"
TOML = ROOT / "aliases.toml"
HELP = ROOT.parent / "help.txt"
KEYWORDS = ROOT / "keywords.toml"
FLAGS = "--flags", "--section"
HISTFILE = os.environ.get("HISTFILE", False)

@dataclass
class Acronym:
    """Aggregate data for an acronym.

    Data in constructor can either be given individually, or can take
    acommand such as "git checkout -b".
    Args:
        command (list[str]): Command split on spaces.
        cmd_str (str): Command as a string.
        short (str): Short form of acronym.
        section (str): Section of aliases.toml file.

        OR

        segment (str): Single command. 
    """
    command: list[str]
    cmd_str: str
    short: str
    section: str

    @classmethod
    def from_segment(cls, segment: str):
        cmd = [w.strip(' \'"') for w in segment.split() if w not in FLAGS]

        section = cmd[0]
        if 'sudo' in cmd:
            section = cmd[1]
        if 'under' in cmd:
            section = cmd[cmd.index('under') + 1]
            cmd = cmd[:cmd.index('under')]

        if 'as' in cmd:
            i = cmd.index('as')
            short = cmd[i + 1]
            cmd = cmd[:i]
        else:
            filtered = [w for w in cmd if w != 'sudo']
            short = acronymize(filtered, use_flags='--flags' in segment)

        return cls(
            command=cmd,
            cmd_str=' '.join(cmd),
            short=short,
            section=section,
        )


def acronymize(command: list[str], use_flags: bool = False) -> str:
    """Create an acronym from a given command.

    Args:
        command (list[str]): Command split on spaces.
        use_flags (bool): Whether or not to include command flags.

    Returns:
        (str): Shortened form of a command.
    """
    if use_flags:
        return ''.join(word.replace("-", "")[0] for word in command)
    return ''.join(word[0] for word in command if word[0] != "-")


def resolve_collisions(aliases: dict[str, dict[str, str]], values: set[str], ac: Acronym):
    """Insert a new acronym into the dictionary.

    Ensure that the new acronym does not collide with existing acronyms.

    Args:
        aliases (dict[str, dict[str, str]]): Aliases dictionary.
        values (set[str]): Set of acronyms for performant membership check.
        ac (Acronym): Aggregate data for an acronym.

    Returns:
        (dict[str, dict[str, str]]): Modified aliases dictionary.
    """
    if ac.short in values:
        ac.short = input(
            f"Warning: alias '{ac.short}' is taken. \
            Please choose a custom alias, or press return to skip.\n>>> "
        )
        if not ac.short:
            return

    aliases[ac.section][ac.short] = ac.cmd_str


def generate_aliases(aliases: dict[str, dict[str, str]]) -> str:
    """Generate shell script from aliases dictionary.

    To be sourced by shell rc file.

    Args:
        aliases (dict[str, dict[str, str]]): Aliases dictionary.

    Returns:
        (str): Shell script containing alias commands.
    """
    return '\n'.join(f'alias "{k}"="{v}"' for key in aliases.values() for k, v in key.items())
