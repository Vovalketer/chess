subdir('utils')
libarray_dep = declare_dependency(link_with: [libarray])
liblog_dep = declare_dependency(link_with: [liblog])

game_state_sources = [
    'game_state.c',
    'board.c',
    'game_state.c',
    'move.c',
    'movelist.c',
    'turn_moves.c',
    'position.c',
]

fen_sources = ['fen.c', game_state_sources]
libfen = library(
    'fen',
    fen_sources,
    dependencies: [libarray_dep, liblog_dep],
    include_directories: [inc, inc_utils],
)

libfen_dep = declare_dependency(link_with: [libfen], dependencies: [liblog_dep])
engine_sources = ['engine.c', 'rules.c', 'movegen.c', game_state_sources]
libengine = library(
    'engine',
    engine_sources,
    dependencies: [libarray_dep, liblog_dep, libfen_dep],
    include_directories: [inc, inc_utils],
)

libengine_dep = declare_dependency(
    link_with: [libengine],
    dependencies: [libarray_dep, liblog_dep],
)

perft_sources = ['perft.c']
perft = executable(
    'perft',
    perft_sources,
    dependencies: [libengine_dep],
    include_directories: [inc, inc_utils],
)

raylib_dep = dependency('raylib')
chess_exec = executable(
    'chess',
    ['main.c', 'ui.c'],
    dependencies: [
        raylib_dep,
        libarray_dep,
        liblog_dep,
        libfen_dep,
        libengine_dep,
    ],
    include_directories: [inc, inc_utils],
)
