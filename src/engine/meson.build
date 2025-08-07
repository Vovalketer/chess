libboard = library(
	'board',
	[
		'board.c',
		'bits.c',
		'bitboards.c',
		'fen.c',
		'utils.c',
	],
	dependencies: [liblog_dep, libarray_dep],
	include_directories: [common_inc],
)
libboard_dep = declare_dependency(link_with: [libboard], dependencies: [libarray_dep, liblog_dep])

libmakemove = library(
	'makemove',
	['makemove.c', 'movegen.c', 'bitboards.c'],
	dependencies: [libboard_dep],
)

libmakemove_dep = declare_dependency(link_with: [libmakemove], dependencies: [libboard_dep])

libengine = library(
	'engine',
	['engine.c', 'search.c', 'eval.c'],
	dependencies: [libmakemove_dep],
	include_directories: [common_inc],
)

perft_sources = ['perft.c']
threads_dep = dependency('threads')
perft = executable(
	'perft',
	perft_sources,
	dependencies: [libboard_dep, threads_dep],
	link_with: [libmakemove],
	include_directories: [common_inc],
)
