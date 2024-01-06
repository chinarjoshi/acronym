project('acronym', 'c',
  version : '1.0.0',
  license : 'MIT',
  default_options : ['c_args=-Wall -Wextra'])

# Main executable
pcre_dep = dependency('libpcre', static : true)
sources = [
  'src/main.c', 
  'src/file_io/file_io.c',
  'src/file_io/helpers.c',
  'src/hash_table/entry.c', 
  'src/hash_table/hash_table.c', 
  'src/parse_args/parse_args.c', 
  'src/parse_args/argp_helpers.c', 
  'src/subcmds/create.c',
  'src/subcmds/read.c',
  'src/subcmds/update.c',
  'src/subcmds/delete.c',
  'src/subcmds/helpers.c',
  'src/toml/toml.c',
  'src/toml/toml_extensions.c',
]
executable('acronym', sources : sources, dependencies : pcre_dep)

# Test suite

test_sources = [
  'src/file_io/file_io.c',
  'src/file_io/helpers.c',
  'src/hash_table/entry.c', 
  'src/hash_table/hash_table.c', 
  'src/parse_args/parse_args.c', 
  'src/parse_args/argp_helpers.c', 
  'src/subcmds/create.c',
  'src/subcmds/read.c',
  'src/subcmds/update.c',
  'src/subcmds/delete.c',
  'src/subcmds/helpers.c',
  'src/toml/toml.c',
  'src/toml/toml_extensions.c',
  'tests/test_entry.c',
  'tests/test_hash_table.c',
  'tests/test_parse_args.c',
  'tests/test_file_io.c',
  'tests/test_subcmds.c',
  'tests/test.c',
]

if get_option('enable-tests')
  check_dep = dependency('check')
  e = executable('tests', sources : test_sources, dependencies : [check_dep, pcre_dep])
  test('test', e)
endif
