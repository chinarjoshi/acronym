project('acronym', 'c',
  version : '1.0.0',
  license : 'MIT')

# Main executable
sources = ['src/ACEntry.c', 'src/ACHashTable.c', 'src/main.c']
executable('acronym', sources : sources)

# Test suite
check_dep = dependency('check')
test_sources = ['src/ACEntry.c', 'src/ACHashTable.c', 'tests/test_ACEntry.c', 'tests/test_ACHashTable.c', 'tests/test.c']
e = executable('tests', sources : test_sources,
  dependencies : check_dep)
test('test', e)
