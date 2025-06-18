#ifndef HISTORY_H
#define HISTORY_H
#include "array_tpl.h"
#include "move.h"
#include "types.h"

// TODO: castling rights
typedef struct {
	Move move;
	Piece src;
	Piece dst;
	int turn;
} TurnRecord;

ARRAY_DEFINE_TYPE(TurnRecord, TurnHistory, history)

#endif	// HISTORY_H
