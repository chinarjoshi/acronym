project('acronym', 'c',
  version : '1.0.0',
  license : 'MIT')

# Main executable
pcre_dep = dependency('libpcre')
sources = [
  'src/main.c', 
  'src/file_io.c',
  'src/hash_table/entry.c', 
  'src/hash_table/hash_table.c', 
  'src/parse_args/parse_args.c', 
  'src/parse_args/argp_helpers.c', 
  'src/subcmds/add.c',
  'src/subcmds/remove.c',
  'src/subcmds/edit.c',
  'src/subcmds/tree.c',
  'src/subcmds/show.c',
]
executable('acronym', sources : sources, dependencies : pcre_dep)

# Test suite
check_dep = dependency('check')

test_sources = [
  'src/file_io.c',
  'src/hash_table/entry.c', 
  'src/hash_table/hash_table.c', 
  'src/parse_args/parse_args.c', 
  'src/parse_args/argp_helpers.c', 
  'src/subcmds/add.c',
  'src/subcmds/remove.c',
  'src/subcmds/edit.c',
  'src/subcmds/tree.c',
  'src/subcmds/show.c',
  'tests/test_entry.c',
  'tests/test_hash_table.c',
  'tests/test_parse_args.c',
  'tests/test_file_io.c',
  'tests/test.c',
]
e = executable('tests', sources : test_sources, dependencies : [check_dep, pcre_dep])
test('test', e)
