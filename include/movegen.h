#ifndef MOVEGEN_H
#define MOVEGEN_H
#include "move.h"
#include "types.h"

bool movegen_generate(const BoardState *board, int x, int y, MoveList *moves);
#endif
