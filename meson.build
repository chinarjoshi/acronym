project('acronym', 'c',
  version : '1.0.0',
  license : 'MIT')

# Main executable
sources = ['src/main.c', 'src/ACEntry.c', 'src/ACHashTable.c', 'src/parse_args.c', 'src/argp_helpers.c']
executable('acronym', sources : sources)

# Test suite
check_dep = dependency('check')
test_sources = ['src/ACEntry.c', 'src/ACHashTable.c', 'src/parse_args.c', 'src/argp_helpers.c',
  'tests/test_ACEntry.c', 'tests/test_ACHashTable.c', 'tests/test_parse_args.c', 'tests/test.c']
e = executable('tests', sources : test_sources,
  dependencies : check_dep)
test('test', e)
