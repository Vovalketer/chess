#include "match.h"

#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>

#include "board.h"
#include "log.h"

struct MatchState {
	Board *board;
	int turn;
	PromotionType w_prom;
	PromotionType b_prom;
	TurnHistory *history;
	MatchStatus status;
	CastlingRights castling;
	TurnMoves *legal_moves;
	Position en_passant_target;
	bool en_passant_available;
};

static Piece match_get_promoted_piece(PromotionType type, Player player);
static void match_remove_ks_castling_rights(MatchState *state, Player player);
static void match_remove_qs_castling_rights(MatchState *state, Player player);
static void match_remove_all_castling_rights(MatchState *state, Player player);
static bool match_is_castling_rights_available(MatchState *state, Player player);
static PromotionType match_get_promotion_type(MatchState *state, Player player);
bool match_append_turn_record(MatchState *state, TurnRecord record);

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

	m->w_prom = PROMOTION_QUEEN;
	m->b_prom = PROMOTION_QUEEN;
	m->status = MATCH_IN_PROGRESS;
	m->castling.w_ks = true;
	m->castling.w_qs = true;
	m->castling.b_ks = true;
	m->castling.b_qs = true;
	m->legal_moves = NULL;
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
	b->w_prom = src->w_prom;
	b->b_prom = src->b_prom;
	b->status = src->status;
	b->castling.w_ks = src->castling.w_ks;
	b->castling.w_qs = src->castling.w_qs;
	b->castling.b_ks = src->castling.b_ks;
	b->castling.b_qs = src->castling.b_qs;
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

MatchStatus match_get_status(MatchState *state) {
	assert(state != NULL);
	return state->status;
}

void match_set_status(MatchState *state, MatchStatus status) {
	assert(state != NULL);
	state->status = status;
}

void match_set_next_promotion_type(MatchState *state, Player player, PromotionType type) {
	assert(state != NULL);
	if (player == WHITE_PLAYER) {
		state->w_prom = type;
	} else {
		state->b_prom = type;
	}
}

static PromotionType match_get_promotion_type(MatchState *state, Player player) {
	return player == WHITE_PLAYER ? state->w_prom : state->b_prom;
}

static Piece match_get_promoted_piece(PromotionType type, Player player) {
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

bool match_append_turn_record(MatchState *state, TurnRecord record) {
	assert(state != NULL);
	return history_append(state->history, record);
}

bool match_get_turn_record(MatchState *state, size_t turn, TurnRecord **out_record) {
	assert(state != NULL);
	assert(out_record != NULL);
	return history_get(state->history, turn, out_record);
}

bool match_get_last_turn_record(MatchState *state, TurnRecord **out_record) {
	assert(state != NULL);
	assert(out_record != NULL);
	return history_get_last(state->history, out_record);
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
	if (history_size(state->history) == 0) {
		return false;
	}
	TurnRecord *tr = NULL;
	bool removed = history_pop_last(state->history, &tr);
	if (!removed) {
		return false;
	}
	switch (tr->move_type) {
		case MOVE_CASTLING:
			state->castling = tr->castling;
			board_set_piece(state->board, tr->moving_piece, tr->move.src);
			board_set_piece(state->board, tr->captured_piece, tr->move.dst);

			board_set_piece(state->board,
							(Piece) {.player = tr->moving_piece.player, .type = ROOK},
							tr->special_move_info.rook_move.src);
			board_remove_piece(state->board, tr->special_move_info.rook_move.dst);
			break;
		case MOVE_PROMOTION:
			board_set_piece(state->board, tr->moving_piece, tr->move.src);
			board_set_piece(state->board, tr->captured_piece, tr->move.dst);
			break;
		case MOVE_EN_PASSANT:
			board_set_piece(state->board, tr->moving_piece, tr->move.src);
			board_remove_piece(state->board, tr->move.dst);

			board_set_piece(state->board, tr->captured_piece, tr->special_move_info.captured_pawn_pos);
			break;
		case MOVE_REGULAR:
			board_set_piece(state->board, tr->moving_piece, tr->move.src);
			board_set_piece(state->board, tr->captured_piece, tr->move.dst);
			break;
		case MOVE_INVALID:
			log_error("Attempted to undo an invalid move type");
			exit(1);
			break;
	}
	free(tr);
	return true;
}

bool match_is_kingside_castling_available(MatchState *state, Player player) {
	return player == WHITE_PLAYER ? state->castling.w_ks : state->castling.b_ks;
}

bool match_is_queenside_castling_available(MatchState *state, Player player) {
	return player == WHITE_PLAYER ? state->castling.w_qs : state->castling.b_qs;
}

TurnMoves *match_get_legal_moves(MatchState *state) {
	assert(state != NULL);
	return state->legal_moves;
}

void match_set_legal_moves(MatchState *state, TurnMoves *moves) {
	assert(state != NULL);
	state->legal_moves = moves;
}

static void match_remove_qs_castling_rights(MatchState *state, Player player) {
	assert(state != NULL);
	assert(player != NONE);

	if (player == WHITE_PLAYER) {
		state->castling.w_qs = false;
	} else {
		state->castling.b_qs = false;
	}
}

static void match_remove_ks_castling_rights(MatchState *state, Player player) {
	assert(state != NULL);
	assert(player != NONE);

	if (player == WHITE_PLAYER) {
		state->castling.w_ks = false;
	} else {
		state->castling.b_ks = false;
	}
}

static void match_remove_all_castling_rights(MatchState *state, Player player) {
	assert(state != NULL);
	assert(player != NONE);

	match_remove_ks_castling_rights(state, player);
	match_remove_qs_castling_rights(state, player);
}

static bool match_is_castling_rights_available(MatchState *state, Player player) {
	assert(state != NULL);
	assert(player != NONE);
	return match_is_kingside_castling_available(state, player) ||
		   match_is_queenside_castling_available(state, player);
}

static void match_apply_record_to_board(MatchState *state, TurnRecord record) {
	bool moved = board_move_piece(state->board, record.move.src, record.move.dst);
	if (!moved) {
		log_error("Failed to move piece from %d:%d to %d:%d",
				  record.move.src.x,
				  record.move.src.y,
				  record.move.dst.x,
				  record.move.dst.y);
		exit(1);
	}

	switch (record.move_type) {
		case MOVE_CASTLING:
			bool moved = board_move_piece(
				state->board, record.special_move_info.rook_move.src, record.special_move_info.rook_move.dst);
			if (!moved) {
				log_error("Failed to move rook during castling move from %d:%d to %d:%d",
						  record.special_move_info.rook_move.src.x,
						  record.special_move_info.rook_move.src.y,
						  record.special_move_info.rook_move.dst.x,
						  record.special_move_info.rook_move.dst.y);
				exit(1);
			}
			match_remove_all_castling_rights(state, record.moving_piece.player);
			break;

		case MOVE_PROMOTION:
			board_set_piece(
				state->board,
				match_get_promoted_piece(record.special_move_info.promotion, record.moving_piece.player),
				record.move.dst);
			break;

		case MOVE_EN_PASSANT:
			board_remove_piece(state->board, state->en_passant_target);
			break;

		case MOVE_REGULAR:
			switch (record.moving_piece.type) {
				case KING:
					if (match_is_castling_rights_available(state, record.moving_piece.player)) {
						match_remove_all_castling_rights(state, record.moving_piece.player);
					}
					break;

				case ROOK:
					if (record.move.src.x == 0 &&
						match_is_queenside_castling_available(state, record.moving_piece.player)) {
						match_remove_qs_castling_rights(state, record.moving_piece.player);
					} else if (record.move.src.x == 7 &&
							   match_is_kingside_castling_available(state, record.moving_piece.player)) {
						match_remove_ks_castling_rights(state, record.moving_piece.player);
					}
					break;

				case PAWN:
					if (abs(record.move.dst.y - record.move.src.y) == 2) {
						state->en_passant_target = record.move.dst;
						state->en_passant_available = true;
					}
					break;

				default:
					break;
			}
			state->en_passant_available = false;
			break;

		case MOVE_INVALID:
			log_error("Attempted to apply an invalid move type");
			exit(1);
			break;
	}
}

static TurnRecord _create_turn_record(MatchState *state, Move move, MoveType move_type) {
	Piece moving_piece = board_get_piece(state->board, move.src);

	TurnRecord tr = (TurnRecord) {.move = move,
								  .turn = state->turn,
								  .moving_piece = moving_piece,
								  .move_type = move_type,
								  .castling = state->castling};

	switch (move_type) {
		case MOVE_REGULAR:
			tr.captured_piece = board_get_piece(state->board, move.dst);
			break;
		case MOVE_CASTLING:
			tr.captured_piece = (Piece) {NONE, EMPTY};

			Move rook_move;
			if (moving_piece.player == WHITE_PLAYER) {
				if (move.dst.x == 6) {
					// kingside castling
					rook_move.src = (Position) {7, 7};
					rook_move.dst = (Position) {5, 7};
				} else if (move.dst.x == 2) {
					// queenside castling
					rook_move.src = (Position) {0, 7};
					rook_move.dst = (Position) {3, 7};
				} else {
					log_error("Invalid castling move");
					exit(1);
				}
			}

			else if (moving_piece.player == BLACK_PLAYER) {
				if (move.dst.x == 6) {
					// kingside castling
					rook_move.src = (Position) {7, 0};
					rook_move.dst = (Position) {5, 0};
				} else if (move.dst.x == 2) {
					// queenside castling
					rook_move.src = (Position) {0, 0};
					rook_move.dst = (Position) {3, 0};
				} else {
					log_error("Invalid castling move");
					exit(1);
				}
			} else {
				log_error("Invalid player %d", moving_piece.player);
				exit(1);
			}

			tr.special_move_info.rook_move = rook_move;
			break;

		case MOVE_EN_PASSANT:
			tr.captured_piece = board_get_piece(state->board, state->en_passant_target);
			tr.special_move_info.captured_pawn_pos = state->en_passant_target;
			break;

		case MOVE_PROMOTION:
			tr.captured_piece = board_get_piece(state->board, move.dst);
			tr.special_move_info.promotion = match_get_promotion_type(state, moving_piece.player);
			break;

		case MOVE_INVALID:
			log_error("Attempted to apply an invalid move type");
			exit(1);
	}
	return tr;
}

bool match_apply_move(MatchState *state, Move move, MoveType move_type) {
	TurnRecord tr = _create_turn_record(state, move, move_type);

	match_apply_record_to_board(state, tr);
	history_append(state->history, tr);
	return true;
}
