unity_path = '../external/unity/'
unity_files = [unity_path + 'unity.c', unity_path + 'unity.h']
unity_c_args = ['-DUNITY_OUTPUT_COLOR', '-DUNITY_OUTPUT_FOR_ECLIPSE']

engine_inc = include_directories('../src/engine')

unity_dep = declare_dependency(sources: unity_files, compile_args: unity_c_args)

fen_test = executable(
  'fen_test',
  'fen_test.c',
  include_directories: [common_inc, engine_inc],
  dependencies: [libboard_dep, unity_dep],
)
test('fen_test', fen_test)

movegen_test_files = [movegen_file, movelist_file]
movegen_test = executable(
  'movegen_test',
  'movegen_test.c',
  movegen_test_files,
  include_directories: [common_inc, engine_inc],
  dependencies: [libboard_dep, unity_dep],
)
test('movegen_test', movegen_test)

makemove_test = executable(
  'makemove_test',
  'makemove_test.c',
  include_directories: [common_inc, engine_inc],
  dependencies: [libmakemove_dep, libboard_dep, unity_dep],
)
test('makemove_test', makemove_test)

hash_test_files = [hash_file, makemove_file]
hash_test = executable(
  'hash_test',
  'hash_test.c',
  hash_test_files,
  include_directories: [common_inc, engine_inc],
  dependencies: [libboard_dep, unity_dep],
)
test('hash_test', hash_test)

search_test_files = [search_file, makemove_file, movegen_file, eval_file, transposition_file]
search_test = executable(
  'search_test',
  'search_test.c',
  search_test_files,
  include_directories: [common_inc, engine_inc],
  dependencies: [libboard_dep, unity_dep],
)
# test('search_test', search_test)

# tests for non engine dependent files such as data structures
subdir('common')
