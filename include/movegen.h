#ifndef MOVEGEN_H
#define MOVEGEN_H
#include "movelist.h"

struct Board;

bool movegen_generate(const struct Board *board, Position pos, MoveList *out_moves);
#endif
