fen_file = files('fen.c')
hash_file = files('hash.c')
bits_file = files('bits.c')
board_file = files('board.c')
bitboards_file = files('bitboards.c')
makemove_file = files('makemove.c')
movegen_file = files('movegen.c')
movelist_file = files('movelist.c')
utils_file = files('utils.c')
move_file = files('move.c')
uci_file = files('uci.c')
transposition_file = files('transposition.c')
eval_file = files('eval.c')
search_file = files('search.c')
msg_queue_file = files('msg_queue.c')
engine_file = files('engine.c')
engine_mq = files('engine_mq.c')

board_sources = [
  bitboards_file,
  bits_file,
  board_file,
  hash_file,
  move_file,
  utils_file,
  fen_file,
]
libboard = library(
  'board',
  board_sources,
  dependencies: [liblog_dep],
  include_directories: [common_inc],
)
libboard_dep = declare_dependency(link_with: [libboard], dependencies: [liblog_dep])

libmakemove_sources = [makemove_file, movegen_file, hash_file]
libmakemove = library(
  'makemove',
  libmakemove_sources,
  dependencies: [libboard_dep],
)

libmakemove_dep = declare_dependency(link_with: [libmakemove], dependencies: [libboard_dep])

perft_sources = ['perft.c']
perft = executable(
  'perft',
  perft_sources,
  dependencies: [libboard_dep, threads_dep],
  link_with: [libmakemove],
  include_directories: [common_inc],
)

engine_sources = [engine_file, uci_file, eval_file, search_file, transposition_file, engine_mq]
engine = executable(
  'engine',
  engine_sources,
  dependencies: [libboard_dep, libmakemove_dep, liblog_dep],
  include_directories: [common_inc],
)
