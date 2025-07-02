#ifndef TURN_MOVES_H
#define TURN_MOVES_H
#include <stdbool.h>
#include <stddef.h>

#include "array_tpl.h"
#include "movelist.h"

typedef struct {
	Position pos;
	MoveList *moves;
} TurnPieceMoves;

ARRAY_DEFINE_TYPE(TurnPieceMoves, TurnMoves, turn_moves)

#endif
