#ifndef MOVEGEN_H
#define MOVEGEN_H
#include "movelist.h"
#include "types.h"

bool movegen_generate(const Board *board, Position pos, MoveList *out_moves);
#endif
