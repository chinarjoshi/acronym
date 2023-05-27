project('acronym', 'c',
  version : '1.0.0',
  license : 'MIT')

# Main executable
sources = ['src/main.c']
executable('acronym', sources : sources)

# Test suite
check_dep = dependency('check')
test_sources = ['tests/test_acronym.c']
e = executable('tests', sources : test_sources,
  dependencies : check_dep)
test('tests', e)
