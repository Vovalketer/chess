subdir('utils')
libarray_dep = declare_dependency(link_with: [libarray], include_directories: inc_utils)
liblog_dep = declare_dependency(link_with: [liblog], include_directories: inc_utils)

libboard = library(
	'board',
	[
		'board.c',
		'bits.c',
		'fen.c',
		'utils.c',
	],
	dependencies: [liblog_dep, libarray_dep],
	include_directories: [inc, inc_utils],
)
libboard_dep = declare_dependency(link_with: [libboard], dependencies: [libarray_dep, liblog_dep])

libmakemove = library(
	'makemove',
	['makemove.c', 'movegen.c', 'bitboards.c'],
	dependencies: [libboard_dep],
)

perft_sources = ['perft.c']
threads_dep = dependency('threads')
perft = executable(
	'perft',
	perft_sources,
	dependencies: [libboard_dep, threads_dep],
	link_with: [libmakemove],
	include_directories: [inc, inc_utils],
)
