from acronym.acronym import parse_args
from tests.fixtures import aliases, command, aliases_output


def test_parse_args_add(monkeypatch, aliases, aliases_output, command):
    monkeypatch.setattr('sys.argv', ['acronym', 'add'] + command)
    assert parse_args(aliases) == aliases_output


def test_parse_args_rm(monkeypatch, aliases, aliases_output, command):
    monkeypatch.setattr('sys.argv', ['acronym', 'add'] + command)
    assert parse_args(aliases) == aliases_output

def test_parse_args_rm(monkeypatch, aliases, aliases_output, command):
    monkeypatch.setattr('sys.argv', ['acronym', 'add'] + command)
    assert parse_args(aliases) == aliases_output

def test_parse_args_change(monkeypatch, aliases, aliases_output, command):
    monkeypatch.setattr('sys.argv', ['acronym', 'add'] + command)
    assert parse_args(aliases) == aliases_output

def test_parse_args_change(monkeypatch, aliases, aliases_output, command):
    monkeypatch.setattr('sys.argv', ['acronym', 'add'] + command)
    assert parse_args(aliases) == aliases_output
