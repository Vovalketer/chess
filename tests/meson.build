src_path = '../src/'
unity_path = '../external/unity/'
unity_files = [unity_path + 'unity.c', unity_path + 'unity.h']
unity_c_args = ['-DUNITY_OUTPUT_COLOR', '-DUNITY_OUTPUT_FOR_ECLIPSE']

unity_dep = declare_dependency(
	sources: unity_files,
	compile_args: unity_c_args,
)

fen_test = executable(
	'fen_test',
	'fen_test.c',
	include_directories: [inc, inc_utils],
	dependencies: [libboard_dep, unity_dep],
)
test('fen_test', fen_test)

movegen_files = [
	src_path + 'movegen.c',
	src_path + 'bitboards.c',
	src_path + 'movelist.c',
]
movegen_test = executable(
	'movegen_test',
	'movegen_test.c',
	movegen_files,
	include_directories: [inc, inc_utils],
	dependencies: [libboard_dep, unity_dep],
)
test('movegen_test', movegen_test)
