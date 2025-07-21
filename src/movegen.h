#ifndef MOVEGEN_H
#define MOVEGEN_H

#include "movelist.h"
#include "types.h"

#define MOVEGEN_MAX_DEPTH 10

MoveList *movegen_generate(const Board *board, Player p);

#endif
