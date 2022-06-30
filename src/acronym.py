#!/usr/bin/python3

import sys
import json
import toml
from collections import defaultdict

from helpers import *

ROOT = '/home/c/projects/acronym/src/'
CONFIG = '/etc/acronym.toml'

JSON = ROOT + 'data/aliases.json'
SH = ROOT + 'data/aliases.sh'

FLAGS = '-o', '--options', '-w', '--word'

# Parse the arguments, if register then add the word to the json file,
def parse_args():
    with open(JSON) as f:
        aliases = defaultdict(dict, json.load(f))

    match [w for w in sys.argv[1:] if w not in FLAGS]:
        case [('register' | 'r'), *input] if 'as' in input:
            short = input[-1]
            input = input[:input.index('as')]
            aliases = resolve_collisions(aliases, short, input)

        case [('register' | 'r'), *input]:
            short = acronym(input, use_flags='-o' in sys.argv or '--options' in sys.argv)
            aliases = resolve_collisions(aliases, short, input)

        case [('unregister' | 'u'), *names]:
            if '-w' in sys.argv or '--word' in sys.argv:
                for name in names:
                    aliases.pop(name)
            else:
                for name in names:
                    if not any(prefix.pop(name, False) for prefix in aliases.values() if name in prefix):
                        print(f'Alias \'{name}\' not found')
            
            aliases = {k: v for k, v in aliases.items() if v}

        case [('analyze' | 'a')]:
            with open(CONFIG) as c:
                with open(toml.load(c)['hist_path']) as f:
                    history = Counter([line[15:-1] for line in f.readlines()[:500]])

            def recc_engine(s: str) -> int:
                return len(s) ** 3 \
                    + (history[s] if history[s] < 20 else 20) * 2 \
                    * (0 if len(s) < 5 or history[s] < 5 else 1)
            
            print('Your most common commands with the default alias reccomended:')
            for i, command in enumerate(sorted(history, key=recc_engine, reverse=True)[:10]):
                print(f"{i + 1}. '{command}'  ->  '{acronym(command.split())}'")


        case [('cheatsheet' | 'c'), *prefixes] if prefixes:
            print(json.dumps({k: v for k, v in aliases.items() if k in prefixes}, indent=4))

        case [('cheatsheet' | 'c')]:
            print(json.dumps(aliases, indent=4))

    with open(JSON, 'w') as j, open(SH, 'w') as s:
        json.dump(aliases, j, indent=4)
        s.write(generate_aliases(aliases))


if __name__ == "__main__":
    parse_args()
