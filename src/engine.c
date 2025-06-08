#include "../include/engine.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "../include/board.h"
#include "../include/match.h"
#include "../include/movegen.h"
#include "../include/movelist.h"
#include "../include/rules.h"

bool engine_create_match(MatchState **state) {
	MatchState *b;
	bool result = match_create(&b);
	if (!result) {
		return false;
	}
	*state = b;
	return true;
}

MoveMask engine_get_valid_moves(const MatchState *state, Position pos) {
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

Piece engine_get_piece(const MatchState *state, Position pos) {
	return board_get_piece(match_get_board(state), pos);
}

bool engine_move_piece(MatchState *state, Position src, Position dst) {
	Move move = (Move) {src, dst};
	if (!rules_is_valid_move(state, move)) {
		return false;
	}

	// clone board and calculate if the king iwll be in check after moving the piece
	MatchState *clone;
	bool cloned = match_clone(&clone, state);
	match_move_piece(clone, src, dst);
	assert(cloned != false);
	bool is_check = rules_is_check(clone, match_get_player_turn(state));
	match_destroy(&clone);
	if (is_check) {
		return false;
	}

	match_move_piece(state, src, dst);
	match_next_turn(state);
	return true;
}

bool engine_undo_move(MatchState *state) {
	// TODO: implement, possibly by adding a move list to the board state itself
	return false;
}

void engine_destroy_match(MatchState **state) {
	match_destroy(state);
}
