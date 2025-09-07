libboard = library(
	'board',
	[
		'board.c',
		'bits.c',
		'bitboards.c',
		'fen.c',
		'hash.c',
		'utils.c',
		'move.c',
	],
	dependencies: [liblog_dep],
	include_directories: [common_inc],
)
libboard_dep = declare_dependency(link_with: [libboard], dependencies: [ liblog_dep])

libmakemove = library(
	'makemove',
	['makemove.c', 'movegen.c', 'hash.c'],
	dependencies: [libboard_dep],
)

libmakemove_dep = declare_dependency(link_with: [libmakemove], dependencies: [libboard_dep])

libenginegui = library(
	'engine',
	['enginegui.c', 'search.c', 'transposition.c', 'eval.c'],
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
