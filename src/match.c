#include "../include/match.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "board.h"

struct MatchState {
	Board *board;
	int turn;
};

bool match_create(MatchState **state) {
	assert(state != NULL);
	MatchState *b;
	b = calloc(1, sizeof(MatchState));
	if (!b) {
		return false;
	}

	if (!board_create(&b->board)) {
		free(b);
		return false;
	}

	board_init_positions(b->board);
	b->turn = 0;
	*state = b;
	return true;
}

bool match_clone(MatchState **dst, const MatchState *src) {
	assert(dst != NULL);
	assert(src != NULL);
	MatchState *b;
	b = malloc(sizeof(**dst));
	if (!b) {
		return false;
	}
	if (!board_clone(&b->board, src->board)) {
		free(b);
		return false;
	}
	b->turn = src->turn;
	*dst = b;
	return true;
}

void match_destroy(MatchState **state) {
	if (state && *state) {
		board_destroy(&(*state)->board);
		free(*state);
		*state = NULL;
	}
}

const Board *match_get_board(const MatchState *state) {
	assert(state != NULL);
	return state->board;
}

bool match_move_piece(MatchState *state, Position src, Position dst) {
	assert(state != NULL);
	return board_move_piece(state->board, src, dst);
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


