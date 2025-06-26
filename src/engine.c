#include "../include/engine.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "../include/board.h"
#include "../include/match.h"
#include "../include/movegen.h"
#include "../include/movelist.h"
#include "../include/rules.h"

static bool _update_match_status(MatchState *state);
bool engine_create_match(MatchState **state) {
	MatchState *b;
	bool result = match_create(&b);
	if (!result) {
		return false;
	}
	*state = b;
	return true;
}

MoveMask engine_get_valid_moves(MatchState *state, Position pos) {
	MoveMask mm = {0};
	MoveList *moves = NULL;
	move_list_create(&moves);
	movegen_generate(match_get_board(state), pos, moves);

	for (size_t i = 0; i < move_list_size(moves); i++) {
		Move *m = NULL;
		move_list_get(moves, i, &m);
		if (position_eq(m->src, pos)) {
			mm.mask[m->dst.y][m->dst.x] = true;
			mm.count++;
		}
	}
	move_list_destroy(&moves);
	return mm;
}

Piece engine_get_piece(MatchState *state, Position pos) {
	return board_get_piece(match_get_board(state), pos);
}

bool engine_move_piece(MatchState *state, Position src, Position dst) {
	if (match_get_status(state) != MATCH_IN_PROGRESS ||
		match_get_player_turn(state) != match_get_piece(state, src).player) {
		return false;
	}

	Move move = (Move) {src, dst};
	PromotionType promotion_type = NO_PROMOTION;

	MoveType move_type = rules_get_move_type(state, move);
	if (move_type == MOVE_INVALID) {
		return false;
	}

	TurnRecord record = match_create_turn_record(state, move, move_type, promotion_type);

	switch (move_type) {
		case MOVE_REGULAR:
			match_move_piece(state, src, dst);
			break;
		case MOVE_CASTLING:
			match_move_castling(state, src, dst);
			break;
		// case MOVE_EN_PASSANT:
		// 	match_move_en_passant(state, src, dst);
		// 	break;
		case MOVE_PROMOTION:
			match_promote_pawn(state, src);
		default:
			break;
	}

	match_append_turn_record(state, record);
	match_next_turn(state);
	_update_match_status(state);
	return true;
}

bool _update_match_status(MatchState *state) {
	Player player = match_get_enemy_player(match_get_player_turn(state));
	bool is_checkmate = rules_is_checkmate(state, player);
	if (is_checkmate) {
		MatchStatus winner = player == WHITE_PLAYER ? MATCH_BLACK_WINS : MATCH_WHITE_WINS;
		match_set_status(state, winner);
	}
	return is_checkmate;
}

void engine_undo_move(MatchState *state) {
	bool undone = match_undo_move(state);
	if (undone) {
		match_previous_turn(state);
	}
}

void engine_set_next_promotion_type(MatchState *state, Player player, PromotionType type) {
	assert(state != NULL);
	if (type == NO_PROMOTION) {
		return;
	}
	match_set_next_promotion_type(state, player, type);
}

void engine_destroy_match(MatchState **state) {
	match_destroy(state);
}
