import os
from setuptools import setup
from pathlib import Path

ROOT = Path(__file__).parent.resolve()
HOME = Path(os.environ['HOME']).resolve()
BASH = Path('/usr/share/bash-completion/completions/_acronym')
ZSH = Path('/usr/share/zsh/site-functions/_acronym')
COMP = ROOT / 'src/acronym-alias/completion/'
BASHRC = HOME / '.bashrc'
ZSHRC = HOME / '.zshrc'
README = ROOT / 'README.md'

s = setup(data_files=[('man/man1', ['docs/acronym.1'])])

path = s.command_obj['install'].install_lib

# Move completion scripts to corresponding directories
if BASH.parent.exists():
    (COMP / '_acronym.bash').rename(BASH)
if ZSH.parent.exists():
    (COMP / '_acronym.zsh').rename(ZSH)

print(path)
print(s)
# # Append sourcing line to .bashrc and .zshrc
# if BASHRC.exists():
#     with BASHRC.open('a') as f:
#         f.write(f'. {path}')

# if ZSHRC.exists():
#     with ZSHRC.open('a') as f:
#         f.write(f')
