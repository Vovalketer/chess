#include "match.h"

#include <assert.h>
#include <stdlib.h>

#include "board.h"

struct MatchState {
	Board *board;
	int turn;
	TurnHistory *history;
};

bool match_create(MatchState **state) {
	assert(state != NULL);
	MatchState *b = NULL;
	b = calloc(1, sizeof(MatchState));
	if (!b) {
		return false;
	}

	if (!board_create(&b->board)) {
		free(b);
		return false;
	}

	if (!history_create(&b->history)) {
		board_destroy(&b->board);
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
	MatchState *b = NULL;
	b = malloc(sizeof(**dst));
	if (!b) {
		return false;
	}
	if (!board_clone(&b->board, src->board)) {
		free(b);
		return false;
	}

	if (!history_clone(&b->history, src->history)) {
		board_destroy(&b->board);
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
		history_destroy(&(*state)->history);
		free(*state);
		*state = NULL;
	}
}

Board *match_get_board(const MatchState *state) {
	assert(state != NULL);
	return state->board;
}

bool match_move_piece(MatchState *state, Position src, Position dst) {
	assert(state != NULL);
	match_append_turn_record(state, (Move) {src, dst});
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

int match_previous_turn(MatchState *state) {
	assert(state != NULL);
	state->turn--;
	return state->turn;
}

Piece match_get_piece(const MatchState *state, Position pos) {
	assert(state != NULL);
	return board_get_piece(state->board, pos);
}

bool match_append_turn_record(MatchState *state, Move move) {
	assert(state != NULL);
	TurnRecord r = (TurnRecord) {.move = move,
								 .turn = state->turn,
								 .player = board_get_piece(state->board, move.src).player,
								 .captured_piece = board_get_piece(state->board, move.dst)};
	return history_append(state->history, r);
}

bool match_get_turn_record(MatchState *state, size_t turn, TurnRecord **out_record) {
	assert(state != NULL);
	assert(out_record != NULL);
	return history_get(state->history, turn, out_record);
}

// Returns a clone of the history. User is in charge of freeing the memory
bool match_get_history_clone(MatchState *state, TurnHistory **out_history) {
	TurnHistory *clone = NULL;
	if (!history_clone(&clone, state->history)) {
		return false;
	}
	*out_history = clone;
	return true;
}

TurnHistory *match_get_history(MatchState *state) {
	assert(state != NULL);
	return state->history;
}

bool match_undo_move(MatchState *state) {
	assert(state != NULL);
	TurnRecord *r = NULL;
	bool removed = history_pop_last(state->history, &r);
	if (!removed) {
		return false;
	}
	board_move_piece(state->board, r->move.dst, r->move.src);
	// if no piece was captured then it'll just set the tile to NONE
	board_set_piece(state->board, r->captured_piece, r->move.dst);
	free(r);
	return true;
}
