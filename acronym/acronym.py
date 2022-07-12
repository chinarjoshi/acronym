import os
import sys
import toml
from pathlib import Path
from collections import defaultdict

from acronym.alias_utils import *
from acronym.help_text import HELP_TEXT

ROOT = Path(__file__).parent.resolve() / 'data'
SH = ROOT / 'aliases.sh'
TOML = ROOT / 'aliases.toml'
HELP = ROOT.parent / 'help.txt'
KEYWORDS = ROOT / 'keywords.toml'
FLAGS = '--flags', '--section'
HISTFILE = os.environ.get('HISTFILE', False)

# Parse the arguments, if register then add the word to the json file,
def main():
    with open(TOML) as f:
        aliases = defaultdict(dict, toml.load(f))

    match sys.argv[1:]:
        case ['add', *cmd]:
            for segment in ' '.join(cmd).split(','):
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

                ac = Acronym(cmd, ' '.join(cmd), short, section)
                values = {k for x in aliases.values() for k in x}

                resolve_collisions(aliases, values, ac)
                print(f'{short} = "{ac.cmd_str}"')

        case ['edit']:
            os.system(f"{os.environ.get('EDITOR', 'vim')} {str(TOML)}")
            exit(0)

        case ['rm', *names]:
            filtered = [w for w in names if w not in FLAGS]
            if '--section' in names:
                for name in filtered:
                    if not aliases.pop(name, False):
                        print(f"Category '{name}' not found.")
            else:
                for name in filtered:
                    if not any(prefix.pop(name, False) for prefix in aliases.values() if name in prefix):
                        print(f"Alias '{name}' not found.")
            
            aliases = {k: v for k, v in aliases.items() if v}

        case ['change', old, 'with', new]:
            for section in aliases:
                if old in section:
                    aliases[section][new] = aliases[section].pop(old)
                    break
            else:
                print(f"Alias '{old}' not found to be replaced.")
                exit(1)

        case ['suggest']:
            with open(HISTFILE) as f, open(KEYWORDS) as k:
                history = [line[15:-1] if line[0] == ':' else line.strip() for line in f.readlines()[:1000]]
                recs = toml.load(k)

            recs = toml.dumps({k: v for k, v in recs.items() if k in history and k not in aliases})
            if recs:
                print('The following aliases are reccomended based on your command history:' + '\n' + recs)
            else:
                print('No current reccomendations')

        case ['print', *prefixes] if prefixes:
            print('\n' + toml.dumps({k: v for k, v in aliases.items() if k in prefixes}))

        case ['print']:
            print('\n' + toml.dumps(aliases))

        case [('help' | '-h' | '--help')]:
            print(HELP_TEXT)

        case ['install']:
            content = f'. {str(Path(__file__).parent)}/data/aliases.sh'
            home = Path(os.environ['HOME']).resolve()

            shellrc = home / ('.bashrc' if 'bash' in os.environ['SHELL'] else '.zshrc')
            if 'zsh' in os.environ['SHELL']:
                content += f"\nfpath+={home / '.local/share/zsh/site-functions'}"
            if os.environ.get('BASHRC', False):
                shellrc = Path(os.environ['BASHRC'])
            elif os.environ.get('ZSHRC', False):
                shellrc = Path(os.environ['ZSHRC'])

            if not shellrc.exists():
                print(f'''\
{shellrc} not found.
Please provide either environmental variable (ZSHRC | BASHRC), for example:
"ZSHRC=/home/c/.config/zsh/.zshrc acronym install"
or manually add the following line to your shell config file:

{content}''')
                exit(1)
            with open(shellrc, 'a') as f:
                print(f"Appending '{content}' to {shellrc}")
                f.write(content)

        case [*_]:
            print('Incorrect usage.')
            print(HELP_TEXT)
            exit(1)


    with open(TOML, 'w') as f, open(SH, 'w') as s:
        toml.dump(aliases, f)
        s.write(generate_aliases(aliases))


if __name__ == "__main__":
    main()
