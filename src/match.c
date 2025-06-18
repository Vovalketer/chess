#include "match.h"

#include <assert.h>
#include <stdlib.h>

#include "board.h"

static Piece match_get_promoted_piece(Player player, PromotionType type);

struct MatchState {
	Board *board;
	int turn;
	TurnHistory *history;
	PromotionType white_promotion;
	PromotionType black_promotion;
};

bool match_create(MatchState **state) {
	if (!match_create_empty(state)) {
		return false;
	}
	board_init_positions((*state)->board);
	return true;
}

bool match_create_empty(MatchState **state) {
	assert(state != NULL);
	MatchState *m = NULL;
	m = calloc(1, sizeof(**state));
	if (!m) {
		return false;
	}

	if (!board_create(&m->board)) {
		free(m);
		return false;
	}

	if (!history_create(&m->history)) {
		board_destroy(&m->board);
		free(m);
		return false;
	}

	m->white_promotion = PROMOTION_QUEEN;
	m->black_promotion = PROMOTION_QUEEN;
	m->turn = 0;
	*state = m;
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
	b->white_promotion = src->white_promotion;
	b->black_promotion = src->black_promotion;
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

Board *match_get_board(MatchState *state) {
	assert(state != NULL);
	return state->board;
}

bool match_move_piece(MatchState *state, Position src, Position dst) {
	assert(state != NULL);
	return board_move_piece(state->board, src, dst);
}

void match_set_next_promotion_type(MatchState *state, Player player, PromotionType type) {
	assert(state != NULL);
	if (player == WHITE_PLAYER) {
		state->white_promotion = type;
	} else {
		state->black_promotion = type;
	}
}

static Piece match_get_promoted_piece(Player player, PromotionType type) {
	PieceType piece_type = PAWN;
	switch (type) {
		case PROMOTION_QUEEN:
			piece_type = QUEEN;
			break;
		case PROMOTION_BISHOP:
			piece_type = BISHOP;
			break;
		case PROMOTION_KNIGHT:
			piece_type = KNIGHT;
			break;
		case PROMOTION_ROOK:
			piece_type = ROOK;
			break;
		case NO_PROMOTION:
			break;
	}
	return (Piece) {player, piece_type};
}

// promotes to the piece stored in the state
PromotionType match_promote_pawn(MatchState *state, Position pos) {
	assert(state != NULL);
	Piece piece = match_get_piece(state, pos);
	assert(piece.type == PAWN);
	PromotionType promotion_type =
		piece.player == WHITE_PLAYER ? state->white_promotion : state->black_promotion;
	Piece promoted = match_get_promoted_piece(piece.player, promotion_type);
	bool set_piece = board_set_piece(state->board, promoted, pos);
	assert(set_piece);
	return promotion_type;
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

TurnRecord match_create_turn_record(MatchState *state, Move move, PromotionType prom) {
	return (TurnRecord) {.move = move,
						 .turn = state->turn,
						 .src = match_get_piece(state, move.src),
						 .dst = match_get_piece(state, move.dst),
						 .promoted_type = prom};
}

bool match_append_turn_record(MatchState *state, TurnRecord record) {
	assert(state != NULL);
	return history_append(state->history, record);
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
	board_set_piece(state->board, r->dst, r->move.dst);
	free(r);
	return true;
}
