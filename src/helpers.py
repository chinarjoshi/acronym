def acronym(command: list[str], use_flags: bool = False) -> str:
    'Returns an acronym of given command.'
    if use_flags:
        return ''.join(word.replace("-", "")[0] for word in command)
    return ''.join(word[0] for word in command if word[0] != "-")


def resolve_collisions(aliases: dict[str, dict[str, str]], short: str, command: list[str]):
    for prefix in aliases.values():
        if short in prefix:
            short = input(f'Alias {short} is taken by {prefix[short]}. Please choose a custom alias.\n>>> ')
    else:
        aliases[command[0]][short] = ' '.join(command)


def generate_aliases(aliases: dict[str, dict[str, str]]) -> str:
    file = '''declare -A aliases=('''
    for key in aliases.values():
        for key, value in key.items():
            file += f"\n\t'{key}'\t'{value}'"
    return file + '''
)
for key value in "${(@kv)aliases[@]}"; do
    alias "$letter"="$command"
done
'''

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
