#include "../include/engine.h"

#include <assert.h>
#include <stdlib.h>

#include "../include/board.h"
#include "../include/game_state.h"
#include "../include/movelist.h"
#include "../include/rules.h"
#include "log.h"

static bool _update_gstate_status(GameState *state);
static void _update_turn_moves(GameState *state);

bool engine_create_match(GameState **state) {
	GameState *b;
	bool result = gstate_create(&b);
	if (!result) {
		return false;
	}
	*state = b;
	TurnMoves *turn_moves = rules_generate_turn_moves(b, gstate_get_player_turn(b));
	gstate_set_legal_moves(b, turn_moves);

	return true;
}

MoveMask engine_get_valid_moves(GameState *state, Position pos) {
	MoveMask mm = {0};
	if (gstate_get_piece(state, pos).player != gstate_get_player_turn(state)) {
		return mm;
	}
	TurnMoves *turn_moves = gstate_get_legal_moves(state);
	if (!turn_moves) {
		log_error("Failed to get legal moves for player %s",
				  gstate_get_player_turn(state) == WHITE_PLAYER ? "White" : "Black");
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

Piece engine_get_piece(GameState *state, Position pos) {
	return board_get_piece(gstate_get_board(state), pos);
}

static void _next_turn(GameState *state) {
	gstate_next_turn(state);
	_update_gstate_status(state);
	_update_turn_moves(state);
}

bool engine_move_piece(GameState *state, Position src, Position dst) {
	if (position_eq(src, dst) || gstate_get_status(state) != MATCH_IN_PROGRESS ||
		gstate_get_player_turn(state) != gstate_get_piece(state, src).player) {
		return false;
	}

	Move move = (Move) {src, dst};

	MoveType move_type = rules_get_move_type(state, move);
	if (move_type == MOVE_INVALID) {
		return false;
	}
	bool applied = gstate_apply_move(state, move, move_type);
	if(!applied){
		log_error("Failed to apply move");
		exit(1);
	}

	_next_turn(state);
	return true;
}

bool _update_gstate_status(GameState *state) {
	Player player = gstate_get_player_turn(state);
	bool is_checkmate = rules_is_checkmate(state, player);
	if (is_checkmate) {
		MatchStatus winner = player == WHITE_PLAYER ? MATCH_BLACK_WINS : MATCH_WHITE_WINS;
		gstate_set_status(state, winner);
		log_info("Winner: %s\n", gstate_get_status(state) == MATCH_BLACK_WINS ? "Black" : "White");
	}
	bool _is_fifty_moves_draw = rules_is_fifty_moves_draw(state);
	if (_is_fifty_moves_draw) {
		gstate_set_status(state, MATCH_DRAW);
		log_info("Fifty move draw");
	}
	return is_checkmate;
}

void _update_turn_moves(GameState *state) {
	TurnMoves *moves = gstate_get_legal_moves(state);
	turn_moves_destroy(&moves);
	gstate_set_legal_moves(state, rules_generate_turn_moves(state, gstate_get_player_turn(state)));
}

void engine_undo_move(GameState *state) {
	bool undone = gstate_undo_move(state);
	if (undone) {
		gstate_previous_turn(state);
		_update_turn_moves(state);
	}
}

void engine_set_next_promotion_type(GameState *state, Player player, PromotionType type) {
	assert(state != NULL);
	if (type == NO_PROMOTION) {
		return;
	}
	gstate_set_next_promotion_type(state, player, type);
}

void engine_destroy_match(GameState **state) {
	gstate_destroy(state);
}
