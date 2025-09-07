src_path = '../src/'
engine_path = src_path + 'engine/'
unity_path = '../external/unity/'
unity_files = [unity_path + 'unity.c', unity_path + 'unity.h']
unity_c_args = ['-DUNITY_OUTPUT_COLOR', '-DUNITY_OUTPUT_FOR_ECLIPSE']

engine_inc = include_directories(engine_path)

unity_dep = declare_dependency(sources: unity_files, compile_args: unity_c_args)

fen_test = executable(
    'fen_test',
    'fen_test.c',
    include_directories: [common_inc, engine_inc],
    dependencies: [libboard_dep, unity_dep],
)
test('fen_test', fen_test)

movegen_files = [engine_path + 'movegen.c', engine_path + 'movelist.c']
movegen_test = executable(
    'movegen_test',
    'movegen_test.c',
    movegen_files,
    include_directories: [common_inc, engine_inc],
    dependencies: [libboard_dep, unity_dep],
)
test('movegen_test', movegen_test)

makemove_files = [engine_path + 'makemove.c', engine_path + 'hash.c']
makemove_test = executable(
    'makemove_test',
    'makemove_test.c',
    makemove_files,
    include_directories: [common_inc, engine_inc],
    dependencies: [libboard_dep, unity_dep],
)
test('makemove_test', makemove_test)

hash_files = [
    engine_path + 'hash.c',
    engine_path + 'hash.c',
    engine_path + 'makemove.c',
]
hash_test = executable(
    'hash_test',
    'hash_test.c',
    hash_files,
    include_directories: [common_inc, engine_inc],
    dependencies: [libboard_dep, unity_dep],
)
test('hash_test', hash_test)
]
)
