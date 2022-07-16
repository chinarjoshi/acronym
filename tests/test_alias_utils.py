import acronym.alias_utils as utils
from tests.fixtures import example_command, example_aliases, example_aliases_output


def test_acronymize(example_command):
    assert utils.acronymize(example_command) == 'gr'
    assert utils.acronymize(example_command, use_flags=True) == 'grh'


def test_resolve_collisions(example_aliases, example_aliases_output, example_command):
    values = {v for y in example_aliases.values() for v in y.values()}
    ac = utils.Acronym(
        command=example_aliases,
        short=utils.acronymize(example_command),
        section=example_command[0],
        cmd_str=' '.join(example_command),
    )
    utils.resolve_collisions(example_aliases, values, ac)

    assert example_aliases == example_aliases_output


def test_generate_aliases(example_aliases):
    alias_sh = '''alias "jn"="jupyter notebook"
alias "jl"="jupyter lab"
alias "pi"="pip install"
alias "pu"="pip uninstall"'''
    assert utils.generate_aliases(example_aliases) == alias_sh
