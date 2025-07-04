#include "../include/engine.h"

#include <assert.h>
#include <stdlib.h>

#include "../include/board.h"
#include "../include/match.h"
#include "../include/movelist.h"
#include "../include/rules.h"
#include "log.h"

static bool _update_match_status(MatchState *state);
static void _update_turn_moves(MatchState *state);

bool engine_create_match(MatchState **state) {
	MatchState *b;
	bool result = match_create(&b);
	if (!result) {
		return false;
	}
	*state = b;
	TurnMoves *turn_moves = rules_generate_turn_moves(b, match_get_player_turn(b));
	match_set_legal_moves(b, turn_moves);

	return true;
}

MoveMask engine_get_valid_moves(MatchState *state, Position pos) {
	MoveMask mm = {0};
	if (match_get_piece(state, pos).player != match_get_player_turn(state)) {
		return mm;
	}
	TurnMoves *turn_moves = match_get_legal_moves(state);
	if (!turn_moves) {
		log_error("Failed to get legal moves for player %s",
				  match_get_player_turn(state) == WHITE_PLAYER ? "White" : "Black");
		return mm;
	}
	MoveList *moves = NULL;
	for (size_t i = 0; i < turn_moves_size(turn_moves); i++) {
		TurnPieceMoves *tpm = NULL;
		turn_moves_get(turn_moves, i, &tpm);
		if (position_eq(tpm->pos, pos)) {
			moves = tpm->moves;
			break;
		}
	}

	if (!moves) {
		log_error("Failed to get valid moves for piece at pos %d:%d", pos.x, pos.y);
		exit(1);
	}

	for (size_t i = 0; i < move_list_size(moves); i++) {
		Move *m = NULL;
		move_list_get(moves, i, &m);
		if (position_eq(m->src, pos)) {
			mm.mask[m->dst.y][m->dst.x] = true;
			mm.count++;
		}
	}
	return mm;
}

Piece engine_get_piece(MatchState *state, Position pos) {
	return board_get_piece(match_get_board(state), pos);
}

static void _next_turn(MatchState *state, TurnRecord record) {
	match_append_turn_record(state, record);
	match_next_turn(state);
	_update_match_status(state);
	_update_turn_moves(state);
}

bool engine_move_piece(MatchState *state, Position src, Position dst) {
	if (position_eq(src, dst) || match_get_status(state) != MATCH_IN_PROGRESS ||
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
			match_move_piece(state, move);
			break;
		case MOVE_CASTLING:
			match_move_castling(state, move);
			break;
		case MOVE_EN_PASSANT:
			bool is_en_passant = match_move_en_passant(state, move);
			assert(is_en_passant);
			break;
		case MOVE_PROMOTION:
			match_move_promotion(state, move);
		default:
			break;
	}

	_next_turn(state, record);
	return true;
}

bool _update_match_status(MatchState *state) {
	Player player = match_get_player_turn(state);
	bool is_checkmate = rules_is_checkmate(state, player);
	log_info("Checkmate: %s\n", is_checkmate ? "true" : "false");
	if (is_checkmate) {
		MatchStatus winner = player == WHITE_PLAYER ? MATCH_BLACK_WINS : MATCH_WHITE_WINS;
		match_set_status(state, winner);
		log_info("Winner: %s\n", match_get_status(state) == MATCH_BLACK_WINS ? "Black" : "White");
	}
	return is_checkmate;
}

void _update_turn_moves(MatchState *state) {
	TurnMoves *moves = match_get_legal_moves(state);
	turn_moves_destroy(&moves);
	match_set_legal_moves(state, rules_generate_turn_moves(state, match_get_player_turn(state)));
}

void engine_undo_move(MatchState *state) {
	bool undone = match_undo_move(state);
	if (undone) {
		match_previous_turn(state);
		_update_turn_moves(state);
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
