#!/usr/bin/python3

import sys
import json
import toml
from collections import defaultdict

from helpers import *

JSON = '/home/c/projects/acronym/src/data/aliases.json'
SH = '/home/c/projects/acronym/src/data/aliases.sh'
CONFIG = '/etc/acronym.toml'
FLAGS = '--flags', '--section'

# Parse the arguments, if register then add the word to the json file,
def parse_args():
    with open(JSON) as f:
        aliases = defaultdict(dict, json.load(f))

    match sys.argv[1:]:
        case ['add', *cmd]:
            for segment in ' '.join(cmd).split(','):
                cmd = [w.strip() for w in segment if w not in FLAGS]
                if 'as' in segment:
                    short = cmd[-1]
                    cmd = cmd[:cmd.index('as')]
                else:
                    short = acronym(cmd, use_flags='--flags' in segment)

                resolve_collisions(aliases, short, cmd)

        case ['rm', *names]:
            if '--section' in names:
                for name in names:
                    if not aliases.pop(name, False):
                        print(f"Category '{names}' not found.")
            else:
                for name in names:
                    if not any(prefix.pop(name, False) for prefix in aliases.values() if name in prefix):
                        print(f"Alias '{name}' not found.")
            
            aliases = {k: v for k, v in aliases.items() if v}

        case ['suggest']:
            with open(CONFIG) as c:
                with open(toml.load(c)['hist_path']) as f:
                    history = Counter([line[15:-1] for line in f.readlines()[:500]])

            def recc_engine(s: str) -> int:
                return (len(s) * 2) ** 3 \
                    + (history[s] if history[s] < 20 else 20) * 2 \
                    * (0 if len(s) < 5 or history[s] < 5 else 1)
            
            print('Your most common commands with the default alias reccomended:')
            for i, command in enumerate(sorted(history, key=recc_engine)[:10]):
                print(f"{i + 1}. '{command}'  ->  '{acronym(command.split())}'")


        case ['print', *prefixes] if prefixes:
            print(json.dumps({k: v for k, v in aliases.items() if k in prefixes}, indent=4))

        case ['print']:
            print(json.dumps(aliases, indent=4))

        case [('-h' | '--help')]:
            print_help()

        case [_]:
            print('Incorrect usage')
            print_help()


    with open(JSON, 'w') as j, open(SH, 'w') as s:
        json.dump(aliases, j, indent=4)
        s.write(generate_aliases(aliases))


if __name__ == "__main__":
    parse_args()
