#!/usr/bin/python3

import sys
import json

from helpers import *

ROOT = '/home/c/projects/acronym/src'
CONFIG = '/etc/acronym.toml'

JSON = ROOT + 'data/aliases.json'
SH = ROOT + 'data/aliases.sh'

FLAGS = '-o', '--options', '-w', '--word'

# Parse the arguments, if register then add the word to the json file,
def parse_args():
    with open(JSON) as f:
        aliases = json.load(f)

    match sys.argv[1:]:
        case [('register' | 'r'), *input] if 'as' in input:
            input = [word for word in input[:input.index('as')] if word not in FLAGS]
            alias = input[-1]

            if input[0] in aliases:
                aliases[input[0]][alias] = ' '.join(input)
            else:
                aliases[input[0]] = {alias: ' '.join(input)}

            aliases = check_collisions(aliases)

        case [('register' | 'r'), *input]:
            input = [word for word in input if word not in FLAGS]
            alias = acronym(input, use_flags='-o' in sys.argv or '--options' in sys.argv)

            if input[0] in aliases:
                aliases[input[0]][alias] = ' '.join(input)
            else:
                aliases[input[0]] = {alias: ' '.join(input)}

            aliases = check_collisions(aliases)

        case [('unregister' | 'u'), name]:
            if '-w' in sys.argv or '--word' in sys.argv:
                aliases.pop(name)
            else:
                if not any(prefix.pop(name, False) for prefix in aliases.values() if name in prefix):
                    print(f'Alias \'{name}\' not found')
            
            # Can't delete keys mid-iteration
            remove = [key for key, value in aliases.items() if not value]
            for item in remove:
                aliases.pop(item)

        case [('cheatsheet' | 'c'), *prefixes]:
            print([aliases[prefix] for prefix in aliases if prefix in prefixes])

        case [('cheatsheet' | 'c')]:
            print(aliases)

    with open(JSON, 'w') as j, open(SH, 'w') as s:
        json.dump(aliases, j, indent=4)
        s.write(generate_aliases(aliases))


if __name__ == "__main__":
    parse_args()
