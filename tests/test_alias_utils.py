import pytest
from collections import defaultdict

from acronym.alias_utils import Acronym, acronymize, resolve_collisions, generate_aliases


@pytest.fixture
def example_aliases():
    return defaultdict(dict, {
        'jupyter': {
            'jn': 'jupyter notebook',
            'jl': 'jupyter lab'
        },
        'pip': {
            'pi': 'pip install',
            'pu': 'pip uninstall'
        }
    })


@pytest.fixture
def example_aliases_output():
    return defaultdict(dict, {
        'jupyter': {
            'jn': 'jupyter notebook',
            'jl': 'jupyter lab'
        },
        'pip': {
            'pi': 'pip install',
            'pu': 'pip uninstall'
        },
        'git': {
            'gr': 'git reset --hard'
        }
    })


@pytest.fixture
def example_command():
    return ['git', 'reset', '--hard']


def test_acronymize(example_command):
    assert acronymize(example_command) == 'gr'
    assert acronymize(example_command, use_flags=True) == 'grh'


def test_resolve_collisions(example_aliases, example_aliases_output, example_command):
    values = {v for y in example_aliases.values() for v in y.values()}
    ac = Acronym(
        command=example_aliases,
        short=acronymize(example_command),
        section=example_command[0],
        cmd_str=' '.join(example_command),
    )
    resolve_collisions(example_aliases, values, ac)

    assert example_aliases == example_aliases_output


def test_generate_aliases(example_aliases):
    alias_sh = '''alias "jn"="jupyter notebook"
alias "jl"="jupyter lab"
alias "pi"="pip install"
alias "pu"="pip uninstall"'''
    assert generate_aliases(example_aliases) == alias_sh
