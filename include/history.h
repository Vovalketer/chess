#ifndef HISTORY_H
#define HISTORY_H
#include "array_tpl.h"
#include "move.h"
#include "types.h"

typedef union {
	Position captured_pawn_pos;	 // only used during en passant
	Move rook_move;				 // only used during castling
	PromotionType promotion;	 // only used during promotions
} SpecialMoveInfo;

typedef struct {
	Move move;
	Piece moving_piece;
	Piece captured_piece;

	MoveType move_type;
	SpecialMoveInfo special_move_info;

	int turn;
	int halfmove_clock;

	Position en_passant_target;
	bool en_passant_available;

	CastlingRights castling;
} TurnRecord;

ARRAY_DEFINE_TYPE(TurnRecord, TurnHistory, history)

#endif	// HISTORY_H
