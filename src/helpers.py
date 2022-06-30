import toml
from collections import Counter


def acronym(command: list[str], use_flags: bool = False) -> str:
    'Returns an acronym of given command.'
    if use_flags:
        return ''.join(word.replace("-", "")[0] for word in command)
    return ''.join(word[0] for word in command if word[0] != "-")


def resolve_collisions(aliases: dict[str, dict[str, str]], short: str, command: list[str]) -> dict:
    for prefix in aliases.values():
        if short in prefix:
            short = input(f'Alias {short} is taken by {prefix[short]}. Please choose a custom alias.\n>>> ')
    else:
        aliases[command[0]][short] = ' '.join(command)

    return aliases


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

def cprint(string: str, color: str) -> None:
    colors = { 
    'red' = '\033[95m'
    'green' = '\033[94m'
    'yellow' = '\033[96m'
    'purple' = '\033[92m'
    'cyan' = '\033[93m'
    'grey' = '\033[91m'
    'black' = '\033[0m'
    UNDERLINE = '\033[4m'


    }
    print(f"\033[98m {string}\033[00m")
    pass
