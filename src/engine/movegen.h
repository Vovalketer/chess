#ifndef MOVEGEN_H
#define MOVEGEN_H

#include "../include/types.h"
#include "movelist.h"

#define MOVEGEN_MAX_DEPTH 10
// generate all moves for a given board
MoveList *movegen_generate(const Board *board, Player p);
// generates only moves
MoveList *movegen_generate_moves(const Board *board, Player p);
// generates only captures
MoveList *movegen_generate_captures(const Board *board, Player p);

#endif
