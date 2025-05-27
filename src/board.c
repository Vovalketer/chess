#include "../include/board.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>

struct BoardState {
	Piece board[8][8];
	int turn;
};

bool board_create(BoardState **state) {
	assert(state != NULL);
	BoardState *b;
	b = calloc(1, sizeof(BoardState));
	if (!b) {
		return false;
	}
	b->turn = 0;
	*state = b;
	return true;
}

bool board_clone(BoardState **dst, const BoardState *src) {
	assert(dst != NULL);
	assert(src != NULL);
	BoardState *b;
	b = malloc(sizeof(BoardState));
	if (!b) {
		return false;
	}
	memcpy(b, src, sizeof(BoardState));
	*dst = b;
	return true;
}

void board_destroy(BoardState **state) {
	if (state && *state) {
		free(*state);
		*state = NULL;
	}
}

Piece board_get_piece(const BoardState *state, Position pos) {
	assert(state != NULL);
	if (!board_is_within_bounds(pos)) {
		return (Piece) {NONE, EMPTY};
	}
	return state->board[pos.y][pos.x];
}

bool board_set_piece(BoardState *state, Piece piece, Position pos) {
	assert(state != NULL);
	if (board_is_within_bounds(pos)) {
		state->board[pos.y][pos.x] = piece;
		return true;
	}
	return false;
}

void board_remove_piece(BoardState *state, Position pos) {
	assert(state != NULL);
	board_set_piece(state, (Piece) {NONE, EMPTY}, pos);
}

bool board_move_piece(BoardState *state, Position src, Position dst) {
	assert(state != NULL);
	if (!board_is_within_bounds(src) || !board_is_within_bounds(dst)) {
		return false;
	}
	Piece orig = board_get_piece(state, src);
	if (orig.type == EMPTY) {
		return false;
	}
	board_set_piece(state, orig, dst);
	board_remove_piece(state, src);
	return true;
}

Player board_get_player_turn(const BoardState *state) {
	assert(state != NULL);
	return state->turn % 2 == 0 ? WHITE_PLAYER : BLACK_PLAYER;
}

int board_get_turn(const BoardState *state) {
	assert(state != NULL);
	return state->turn;
}

int board_next_turn(BoardState *state) {
	assert(state != NULL);
	state->turn++;
	return state->turn;
}

bool board_is_empty(const BoardState *state, Position pos) {
	assert(state != NULL);
	return board_get_piece(state, pos).type == EMPTY;
}

bool board_is_enemy(const BoardState *state, Player player, Position pos) {
	assert(state != NULL);
	switch (player) {
		case WHITE_PLAYER:
			return board_get_piece(state, pos).player == BLACK_PLAYER;
		case BLACK_PLAYER:
			return board_get_piece(state, pos).player == WHITE_PLAYER;
		case NONE:
			return false;
	}
	return false;  // should never reach this point but is required by the compiler
}

bool board_is_within_bounds(Position pos) {
	return pos.x >= 0 && pos.x <= 7 && pos.y >= 0 && pos.y <= 7;
}

Position board_find_king_pos(const BoardState *state, Player player) {
	assert(state != NULL);
	for (int i = 0; i < 8; i++) {
		for (int j = 0; j < 8; j++) {
			Position pos = (Position) {i, j};
			Piece piece = board_get_piece(state, pos);
			if (piece.player == player && piece.type == KING) {
				return pos;
			}
		}
	}
	return (Position) {-1, -1};	 // should never reach this point but is required by the compiler
}
