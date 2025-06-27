#include "match.h"

#include <assert.h>
#include <stdlib.h>

#include "board.h"

static Piece match_get_promoted_piece(Player player, PromotionType type);
static void match_remove_ks_castling_rights(MatchState *state, Player player);
static void match_remove_qs_castling_rights(MatchState *state, Player player);
static void match_remove_all_castling_rights(MatchState *state, Player player);
static bool match_is_castling_rights_available(MatchState *state, Player player);
static void match_queenside_castling(MatchState *state, Player player);
static void match_kingside_castling(MatchState *state, Player player);

struct MatchState {
	Board *board;
	int turn;
	PromotionType white_promotion;
	PromotionType black_promotion;
	TurnHistory *history;
	MatchStatus status;
	bool w_ks_castling;
	bool w_qs_castling;
	bool b_ks_castling;
	bool b_qs_castling;
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
	m->status = MATCH_IN_PROGRESS;
	m->w_ks_castling = true;
	m->w_qs_castling = true;
	m->b_ks_castling = true;
	m->b_qs_castling = true;
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
	b->status = src->status;
	b->w_ks_castling = src->w_ks_castling;
	b->w_qs_castling = src->w_qs_castling;
	b->b_ks_castling = src->b_ks_castling;
	b->b_qs_castling = src->b_qs_castling;
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

bool match_move_piece(MatchState *state, Position src, Position dst) {
	assert(state != NULL);
	Piece piece = board_get_piece(state->board, src);
	Player player = piece.player;
	if (board_move_piece(state->board, src, dst)) {
		if (match_is_castling_rights_available(state, player)) {
			if (piece.type == KING) {
				match_remove_all_castling_rights(state, player);
			} else if (piece.type == ROOK) {
				if (src.x == 0) {
					match_remove_qs_castling_rights(state, player);
				} else if (src.x == 7) {
					match_remove_ks_castling_rights(state, player);
				}
			}
		}
	}
	return false;
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

TurnRecord match_create_turn_record(MatchState *state, Move move, MoveType type, PromotionType prom) {
	return (TurnRecord) {.move = move,
						 .turn = state->turn,
						 .src = match_get_piece(state, move.src),
						 .dst = match_get_piece(state, move.dst),
						 .move_type = type,
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

bool match_is_kingside_castling_available(MatchState *state, Player player) {
	return player == WHITE_PLAYER ? state->w_ks_castling : state->b_ks_castling;
}

bool match_is_queenside_castling_available(MatchState *state, Player player) {
	return player == WHITE_PLAYER ? state->w_qs_castling : state->b_qs_castling;
}

static void match_kingside_castling(MatchState *state, Player player) {
	assert(player != NONE);
	int row = player == WHITE_PLAYER ? 7 : 0;
	int king_col = 4;
	int rook_col = 7;
	Position king_pos = (Position) {king_col, row};
	Position rook_pos = (Position) {rook_col, row};
	// this should be checked by rules
	assert(board_get_piece(state->board, king_pos).type == KING);
	assert(board_get_piece(state->board, rook_pos).type == ROOK);
	Position king_castled_pos = (Position) {6, row};
	Position rook_castled_pos = (Position) {5, row};
	board_move_piece(state->board, king_pos, king_castled_pos);
	board_move_piece(state->board, rook_pos, rook_castled_pos);
}

static void match_queenside_castling(MatchState *state, Player player) {
	assert(player != NONE);
	int row = player == WHITE_PLAYER ? 7 : 0;
	int king_col = 4;
	int rook_col = 0;
	Position king_pos = (Position) {king_col, row};
	Position rook_pos = (Position) {rook_col, row};
	// this should be checked by rules
	assert(board_get_piece(state->board, king_pos).type == KING);
	assert(board_get_piece(state->board, rook_pos).type == ROOK);
	Position king_castled_pos = (Position) {2, row};
	Position rook_castled_pos = (Position) {3, row};
	board_move_piece(state->board, king_pos, king_castled_pos);
	board_move_piece(state->board, rook_pos, rook_castled_pos);
}

bool match_move_castling(MatchState *state, Position src, Position dst) {
	assert(state != NULL);
	Player p_src = board_get_piece(state->board, src).player;
	Player p_dst = board_get_piece(state->board, dst).player;
	Piece king = board_get_piece(state->board, src);
	Piece rook = board_get_piece(state->board, dst);
	if (king.type != KING || rook.type != ROOK || p_src != p_dst) {
		return false;
	}
	if (dst.x == 0) {
		match_queenside_castling(state, p_src);
	} else {
		match_kingside_castling(state, p_src);
	}
	match_remove_all_castling_rights(state, p_src);
	return true;
}

bool match_move_en_passant(MatchState *state, Position src, Position dst) {
	assert(state != NULL);
	Piece src_piece = board_get_piece(state->board, src);
	if (src_piece.type != PAWN) {
		return false;
	}
	int step = src_piece.player == WHITE_PLAYER ? 1 : -1;
	Position target_pos = (Position) {dst.x, dst.y + step};
	Piece target = board_get_piece(state->board, target_pos);

	if (target.type != PAWN || target.player == src_piece.player) {
		return false;
	}

	bool pawn_move = board_move_piece(state->board, src, dst);
	assert(pawn_move);
	board_remove_piece(state->board, target_pos);

	return true;
}

static void match_remove_qs_castling_rights(MatchState *state, Player player) {
	assert(state != NULL);
	assert(player != NONE);

	if (player == WHITE_PLAYER) {
		state->w_qs_castling = false;
	} else {
		state->b_qs_castling = false;
	}
}

static void match_remove_ks_castling_rights(MatchState *state, Player player) {
	assert(state != NULL);
	assert(player != NONE);

	if (player == WHITE_PLAYER) {
		state->w_ks_castling = false;
	} else {
		state->b_ks_castling = false;
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

Player match_get_enemy_player(Player player) {
	return player == WHITE_PLAYER ? BLACK_PLAYER : WHITE_PLAYER;
}
