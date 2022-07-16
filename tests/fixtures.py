from collections import defaultdict
from pytest import fixture

@fixture
def aliases():
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


@fixture
def aliases_output():
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


@fixture
def command():
    return ['git', 'reset', '--hard']
