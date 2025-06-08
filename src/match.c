#include "../include/match.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>

struct MatchState {
	Piece board[8][8];
	int turn;
};

bool match_create(MatchState **state) {
	assert(state != NULL);
	MatchState *b;
	b = calloc(1, sizeof(MatchState));
	if (!b) {
		return false;
	}
	b->turn = 0;
	*state = b;
	return true;
}

bool match_clone(MatchState **dst, const MatchState *src) {
	assert(dst != NULL);
	assert(src != NULL);
	MatchState *b;
	b = malloc(sizeof(MatchState));
	if (!b) {
		return false;
	}
	memcpy(b, src, sizeof(MatchState));
	*dst = b;
	return true;
}

void match_destroy(MatchState **state) {
	if (state && *state) {
		free(*state);
		*state = NULL;
	}
}

Piece match_get_piece(const MatchState *state, Position pos) {
	assert(state != NULL);
	if (!match_is_within_bounds(pos)) {
		return (Piece) {NONE, EMPTY};
	}
	return state->board[pos.y][pos.x];
}

bool match_set_piece(MatchState *state, Piece piece, Position pos) {
	assert(state != NULL);
	if (match_is_within_bounds(pos)) {
		state->board[pos.y][pos.x] = piece;
		return true;
	}
	return false;
}

void match_remove_piece(MatchState *state, Position pos) {
	assert(state != NULL);
	match_set_piece(state, (Piece) {NONE, EMPTY}, pos);
}

bool match_move_piece(MatchState *state, Position src, Position dst) {
	assert(state != NULL);
	if (!match_is_within_bounds(src) || !match_is_within_bounds(dst)) {
		return false;
	}
	Piece orig = match_get_piece(state, src);
	if (orig.type == EMPTY) {
		return false;
	}
	match_set_piece(state, orig, dst);
	match_remove_piece(state, src);
	return true;
}

Player match_get_player_turn(const MatchState *state) {
	assert(state != NULL);
	return state->turn % 2 == 0 ? WHITE_PLAYER : BLACK_PLAYER;
}

int match_get_turn(const MatchState *state) {
	assert(state != NULL);
	return state->turn;
}

int match_next_turn(MatchState *state) {
	assert(state != NULL);
	state->turn++;
	return state->turn;
}

bool match_is_empty(const MatchState *state, Position pos) {
	assert(state != NULL);
	return match_get_piece(state, pos).type == EMPTY;
}

bool match_is_enemy(const MatchState *state, Player player, Position pos) {
	assert(state != NULL);
	switch (player) {
		case WHITE_PLAYER:
			return match_get_piece(state, pos).player == BLACK_PLAYER;
		case BLACK_PLAYER:
			return match_get_piece(state, pos).player == WHITE_PLAYER;
		case NONE:
			return false;
	}
	return false;  // should never reach this point but is required by the compiler
}

bool match_is_within_bounds(Position pos) {
	return pos.x >= 0 && pos.x <= 7 && pos.y >= 0 && pos.y <= 7;
}

Position match_find_king_pos(const MatchState *state, Player player) {
	assert(state != NULL);
	for (int i = 0; i < 8; i++) {
		for (int j = 0; j < 8; j++) {
			Position pos = (Position) {i, j};
			Piece piece = match_get_piece(state, pos);
			if (piece.player == player && piece.type == KING) {
				return pos;
			}
		}
	}
	return (Position) {-1, -1};	 // should never reach this point but is required by the compiler
}
