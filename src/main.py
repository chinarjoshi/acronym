import subprocess
import json
import toml
import argparse


CONFIG = toml.load('/home/c/projects/acronym/config.toml')

# word1 word2 -{switch1}{switch2} --flag1 --flag2

# git clone -l --depth 1 --recurse-submodules
# gcldr


def parse_args():
    parser = argparse.ArgumentParser()
    parser.add_argument('-o', action='store_true')
    parser.add_argument('--options', action='store_true')
    parser.parse_args()
    return {'options': parser.o or parser.options}

# Rule 1
def acronym(command: str):
    commands = command.split()
    
    words = [c for c in commands if '-' not in c]
    flags = [c[2:] for c in commands if '--' in c]
    switches = [c for c in commands if c.count('-') == 1]
    
    query = ''
    for c in words:
        query += c[0]
    for c in flags:
        query += c[0]
    for c in switches:
        query += c

    return query

def register():
    pass

def unregister():
    pass

print(acronym('sudo pacman -Syu --color=auto'))

def main():
    args = parse_args()
    with open(CONFIG['alias_path'], 'w') as f:
        f.write(update_aliases())

if __name__ == '__main__':
    main()
