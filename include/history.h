#ifndef HISTORY_H
#define HISTORY_H
#include "array_tpl.h"
#include "move.h"
#include "types.h"

typedef struct {
	Move move;
	Piece src;
	Piece dst;
	MoveType move_type;
	PromotionType promoted_type;
	int turn;
	CastlingRights castling;
} TurnRecord;

ARRAY_DEFINE_TYPE(TurnRecord, TurnHistory, history)

#endif	// HISTORY_H
