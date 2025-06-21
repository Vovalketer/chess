#ifndef MOVE_H
#define MOVE_H

#include <stddef.h>

#include "../include/position.h"

typedef struct {
	Position src;
	Position dst;
} Move;

typedef enum { MOVE_INVALID, MOVE_REGULAR, MOVE_CASTLING, MOVE_PROMOTION, MOVE_EN_PASSANT } MoveType;

// returns a move by value, no need to free it
Move move_create(Position src, Position dst);
bool move_eq(Move m1, Move m2);

#endif
