#include "../include/board.h"

#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>

struct BoardState {
	Piece board[8][8];
	int turn;
};

bool board_create(BoardState **state) {
	BoardState *b;
	assert(state != NULL);
	b = malloc(sizeof(BoardState));
	if (!b) {
		return false;
	}
	b->turn = 0;
	*state = b;
	return true;
}

void board_destroy(BoardState **state) {
	if (state && *state) {
		free(*state);
		*state = NULL;
	}
}

Piece board_get_piece(const BoardState *state, int x, int y) {
	assert(state != NULL);
	if (!board_is_within_bounds(x, y)) {
		return (Piece) {NONE, EMPTY};
	}
	return state->board[y][x];
}

bool board_set_piece(BoardState *state, Piece piece, int x, int y) {
	assert(state != NULL);
	if (board_is_within_bounds(x, y)) {
		state->board[y][x] = piece;
		return true;
	}
	return false;
}

void board_remove_piece(BoardState *state, int x, int y) {
	assert(state != NULL);
	board_set_piece(state, (Piece) {NONE, EMPTY}, x, y);
}

bool board_move_piece(BoardState *state, int x_src, int y_src, int x_dest, int y_dest) {
	assert(state != NULL);
	if (!board_is_within_bounds(x_src, y_src) || !board_is_within_bounds(x_dest, y_dest)) {
		return false;
	}
	Piece orig = board_get_piece(state, x_src, y_src);
	if (orig.type == EMPTY) {
		return false;
	}
	board_set_piece(state, orig, x_dest, y_dest);
	board_remove_piece(state, x_src, y_src);
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

bool board_is_empty(const BoardState *state, int x, int y) {
	assert(state != NULL);
	return board_get_piece(state, x, y).type == EMPTY;
}

bool board_is_enemy(const BoardState *state, Player player, int x, int y) {
	assert(state != NULL);
	switch (player) {
		case WHITE_PLAYER:
			return board_get_piece(state, x, y).player == BLACK_PLAYER;
		case BLACK_PLAYER:
			return board_get_piece(state, x, y).player == WHITE_PLAYER;
		case NONE:
			return false;
	}
	return false;  // should never reach this point but is required by the compiler
}

bool board_is_within_bounds(int x, int y) {
	return x >= 0 && x <= 7 && y >= 0 && y <= 7;
}
