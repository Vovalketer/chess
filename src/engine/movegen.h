#ifndef MOVEGEN_H
#define MOVEGEN_H

#include "../include/types.h"
#include "movelist.h"

#define MOVEGEN_MAX_DEPTH 10

MoveList *movegen_generate(const Board *board, Player p);

#endif
