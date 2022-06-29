def acronym(command: list, use_flags: bool) -> str:
    'Returns an acronym of given command.'
    if use_flags:
        return ''.join(word.replace("-", "")[0] for word in command if word not in FLAGS)
    return ''.join(word[0] for word in command if word[0] != "-")


def check_collisions(aliases: dict):
    # Preliminary check (fast)
    seen = set()
    collisions = {}
    for name in aliases.values():
        for alias in name:
            if alias in seen:
                collisions[alias] = aliases[name][alias]
            else:
                seen.add(alias)
    if collisions:
        return(handle_collisions(aliases, collisions))
    return(aliases)


def handle_collisions(aliases: dict, collisions: dict):
    print('WARNING: The following aliases are in collision.')

    for key, value in collisions.items():
        print(f'{key} ({value})')
    print()
    for key, value in collisions.items():
        aliases[value[0]] = input(f'Replace \'{key}\' with: ')

    return aliases


def generate_aliases(aliases: dict) -> str:
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

def reccomendations(history: str) -> list:
    pass
