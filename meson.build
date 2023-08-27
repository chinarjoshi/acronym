project('acronym', 'c',
  version : '1.0.0',
  license : 'MIT')

# Main executable
pcre_dep = dependency('pcre')
sources = ['src/main.c', 'src/entry.c', 'src/hash_table.c', 'src/parse_args.c', 'src/file_io.c', 'src/argp_helpers.c']
executable('acronym', sources : sources, dependencies : pcre_dep)

# Test suite
check_dep = dependency('check')
test_sources = ['src/entry.c', 'src/hash_table.c', 'src/parse_args.c', 'src/file_io.c', 'src/argp_helpers.c',
  'tests/test_entry.c', 'tests/test_hash_table.c', 'tests/test_parse_args.c', 'tests/test_file_io.c', 'tests/test.c']
e = executable('tests', sources : test_sources, dependencies : [check_dep, pcre_dep])
test('test', e)
