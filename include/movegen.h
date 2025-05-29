#ifndef MOVEGEN_H
#define MOVEGEN_H
#include "movelist.h"
#include "types.h"

bool movegen_generate(const BoardState *board, Position pos, struct MoveList *out_moves);
#endif
