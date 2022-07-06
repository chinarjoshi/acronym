from dataclasses import dataclass

@dataclass
class Acronym:
    command: list[str]  # Input command list
    cmd_str: str        # Input command string
    short: str          # Alias
    section: str        # Category


def acronymize(command: list[str], use_flags: bool = False) -> str:
    'Returns an acronym of given command.'
    if use_flags:
        return ''.join(word.replace("-", "")[0] for word in command)
    return ''.join(word[0] for word in command if word[0] != "-")


def resolve_collisions(aliases: dict[str, dict[str, str]], values: set, ac: Acronym):
    if ac.short in values:
        ac.short = input(f"Warning: alias '{ac.short}' is taken. Please choose a custom alias, or press return to skip.\n>>> ")
        if not ac.short:
            return

    aliases[ac.section][ac.short] = ac.cmd_str


def generate_aliases(aliases: dict[str, dict[str, str]]) -> str:
    return '\n'.join(f'alias {k}="{v}"' for key in aliases.values() for k, v in key.items())


def cprint(string: str, c: str) -> None:
    colors = { 
        'red': '95',
        'green': '94',
        'yellow': '96',
        'purple': '92',
        'cyan': '93',
        'grey': '91',
        'black': '0',
        'underline': '4',
    }
    print(f"\033[{colors.get(c, 0)}m {string}\033[00m")


def print_help():
    pass
