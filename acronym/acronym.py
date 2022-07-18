"""Parse command line args and modify aliases.toml file."""

import os
import sys
from collections import defaultdict
from pathlib import Path

import toml

import acronym.alias_utils as utils
from acronym.help_text import HELP_TEXT


def main():
    """Read aliases.toml file, modify it, and write it back to file.

    This is the entrypoint of tool.
    """
    with open(utils.TOML) as f:
        aliases = defaultdict(dict, toml.load(f))

    aliases = parse_args(aliases)

    with open(utils.TOML, "w") as f, open(utils.SH, "w") as s:
        toml.dump(aliases, f)
        s.write(utils.generate_aliases(aliases))


def parse_args(aliases: dict[str, dict[str, str]]) -> dict[str, dict[str, str]]:
    """Parse command line args and modify given aliases dictionary.

    Match sys.argv and look for enumerated commands and flags.

    Args:
        aliases (dict[str, dict[str, str]]): Aliases straight from aliases.toml file.

    Returns:
        Modified aliases dictionary.
        If command is not "add", "rm", or "change", then return unmodified dict.
    """
    match sys.argv[1:]:
        case ["add", *cmd]:
            if not cmd:
                print("Incorrect usage: args needed after add command.")
                print('See "acronym --help" for more info.')
                exit(1)
            values = {k for x in aliases.values() for k in x}
            for segment in " ".join(cmd).split(","):
                ac = utils.Acronym.from_segment(segment)
                utils.resolve_collisions(aliases, values, ac)
                print(f'{ac.short} = "{ac.cmd_str}"')

        case ["edit"]:
            os.system(f"{os.environ.get('EDITOR', 'vim')} {str(utils.TOML)}")
            exit(0)

        case ["rm", *names]:
            if not names:
                print("Incorrect usage: names needed after rm command")
                print('See "acronym --help" for more info.')
                exit(1)
            filtered = [w for w in names if w not in utils.FLAGS]
            if "--section" in names:
                for name in filtered:
                    if not aliases.pop(name, False):
                        print(f"Category '{name}' not found.")
            else:
                for name in filtered:
                    if not any(
                        prefix.pop(name, False)
                        for prefix in aliases.values()
                        if name in prefix
                    ):
                        print(f"Alias '{name}' not found.")

            aliases = {k: v for k, v in aliases.items() if v}

        case ["change", old, "with", new]:
            if not (old and new):
                print("Incorrect usage: args needed after change command.")
                print('See "acronym --help" for more info.')
                exit(1)
            for section in aliases:
                if old in section:
                    aliases[section][new] = aliases[section].pop(old)
                    break
            else:
                print(f"Alias '{old}' not found to be replaced.")
                exit(1)

        case ["suggest"]:
            with open(utils.HISTFILE) as f, open(utils.KEYWORDS) as k:
                history = [
                    line[15:-1] if line[0] == ":" else line.strip()
                    for line in f.readlines()[:1000]
                ]
                recs = toml.load(k)

            recs = toml.dumps(
                {k: v for k, v in recs.items() if k in history and k not in aliases}
            )
            if recs:
                print(
                    "The following aliases are reccomended based on your command history:"
                    + "\n"
                    + recs
                )
            else:
                print("No current reccomendations")

        case ["print", *prefixes] if prefixes:
            print(
                "\n" + toml.dumps({k: v for k, v in aliases.items() if k in prefixes})
            )

        case ["print"]:
            print("\n" + toml.dumps(aliases))

        case [("help" | "-h" | "--help")]:
            print(HELP_TEXT)

        case ["install"]:
            content = f". {utils.SH}"
            home = Path(os.environ["HOME"]).resolve()

            default_rc = home / (
                ".bashrc" if "bash" in os.environ["SHELL"] else ".zshrc"
            )
            if "zsh" in os.environ["SHELL"]:
                content += f"\nfpath+={home / '.local/share/zsh/site-functions'}"

            provided_rc = input(
                f'Provide absolute path to rc file, or type "None" to add the line yourself. \
            [Default: {default_rc}]'
            )

            if provided_rc:
                if provided_rc == "None":
                    print(
                        f"Add this line to your rc file to source the aliases.\n\n{content}"
                    )
                elif not Path(provided_rc).exists():
                    print(
                        f"Path does not exist. \
                    Add this line to your rc file to source the aliases.\n\n{content}"
                    )
                    exit(1)
            else:
                provided_rc = default_rc

            with open(provided_rc, "a") as f:
                print(f"Appending '{content}' to {provided_rc}")
                f.write(content)

        case [*_]:
            print("Incorrect usage.")
            print(HELP_TEXT)
            exit(1)

    return aliases


if __name__ == "__main__":
    main()
