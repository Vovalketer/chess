#include "game_state.h"

#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>

#include "board.h"
#include "log.h"

struct GameState {
	Board *board;
	int turn;
	int halfmove_clock;
	PromotionType w_prom;
	PromotionType b_prom;
	TurnHistory *history;
	MatchStatus status;
	CastlingRights castling;
	TurnMoves *legal_moves;
	Position en_passant_target;
	bool en_passant_available;
};

static void _remove_ks_castling_rights(GameState *state, Player player);
static void _remove_qs_castling_rights(GameState *state, Player player);
static void _remove_all_castling_rights(GameState *state, Player player);
static bool _is_castling_rights_available(GameState *state, Player player);
static Piece _get_promoted_piece(PromotionType type, Player player);
static PromotionType _get_promotion_type(GameState *state, Player player);

bool gstate_create(GameState **state) {
	if (!gstate_create_empty(state)) {
		return false;
	}
	board_init_positions((*state)->board);
	return true;
}

bool gstate_create_empty(GameState **state) {
	assert(state != NULL);
	GameState *m = NULL;
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

bool gstate_clone(GameState **dst, const GameState *src) {
	assert(dst != NULL);
	assert(src != NULL);
	GameState *b = NULL;
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

void gstate_destroy(GameState **state) {
	if (state && *state) {
		board_destroy(&(*state)->board);
		history_destroy(&(*state)->history);
		free(*state);
		*state = NULL;
	}
}

Board *gstate_get_board(GameState *state) {
	assert(state != NULL);
	return state->board;
}

MatchStatus gstate_get_status(GameState *state) {
	assert(state != NULL);
	return state->status;
}

void gstate_set_status(GameState *state, MatchStatus status) {
	assert(state != NULL);
	state->status = status;
}

void gstate_set_next_promotion_type(GameState *state, Player player, PromotionType type) {
	assert(state != NULL);
	if (player == WHITE_PLAYER) {
		state->w_prom = type;
	} else {
		state->b_prom = type;
	}
}

static PromotionType _get_promotion_type(GameState *state, Player player) {
	return player == WHITE_PLAYER ? state->w_prom : state->b_prom;
}

static Piece _get_promoted_piece(PromotionType type, Player player) {
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

Player gstate_get_player_turn(const GameState *state) {
	assert(state != NULL);
	return state->turn % 2 == 0 ? WHITE_PLAYER : BLACK_PLAYER;
}

int gstate_get_turn(const GameState *state) {
	assert(state != NULL);
	return state->turn;
}

void gstate_set_turn(GameState *state, int turn) {
	assert(state != NULL);
	state->turn = turn;
}

int gstate_get_fullmove_counter(GameState *state) {
	return gstate_get_turn(state) * 0.5;
}

int gstate_next_turn(GameState *state) {
	assert(state != NULL);
	state->turn++;
	return state->turn;
}

int gstate_previous_turn(GameState *state) {
	assert(state != NULL);
	state->turn--;
	return state->turn;
}

Piece gstate_get_piece(GameState *state, Position pos) {
	assert(state != NULL);
	return board_get_piece(state->board, pos);
}

bool gstate_set_piece(GameState *state, Piece piece, Position pos) {
	return board_set_piece(state->board, piece, pos);
}

bool gstate_get_turn_record(GameState *state, size_t turn, TurnRecord **out_record) {
	assert(state != NULL);
	assert(out_record != NULL);
	return history_get(state->history, turn, out_record);
}

bool gstate_get_last_turn_record(GameState *state, TurnRecord **out_record) {
	assert(state != NULL);
	assert(out_record != NULL);
	return history_get_last(state->history, out_record);
}

// Returns a clone of the history. User is in charge of freeing the memory
bool gstate_get_history_clone(GameState *state, TurnHistory **out_history) {
	TurnHistory *clone = NULL;
	if (!history_clone(&clone, state->history)) {
		return false;
	}
	*out_history = clone;
	return true;
}

TurnHistory *gstate_get_history(GameState *state) {
	assert(state != NULL);
	return state->history;
}

bool gstate_undo_move(GameState *state) {
	assert(state != NULL);
	if (history_size(state->history) == 0) {
		return false;
	}
	TurnRecord *tr = NULL;
	bool removed = history_pop_last(state->history, &tr);
	if (!removed) {
		return false;
	}
	state->castling = tr->castling;
	state->turn = tr->turn;
	switch (tr->move_type) {
		case MOVE_CASTLING:
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

bool gstate_has_castling_rights_kingside(GameState *state, Player player) {
	return player == WHITE_PLAYER ? state->castling.w_ks : state->castling.b_ks;
}

bool gstate_has_castling_rights_queenside(GameState *state, Player player) {
	return player == WHITE_PLAYER ? state->castling.w_qs : state->castling.b_qs;
}

void gstate_set_castling_rights_kingside(GameState *state, Player player, bool available) {
	assert(state != NULL);
	assert(player != NONE);
	if (player == WHITE_PLAYER) {
		state->castling.w_ks = available;
	} else {
		state->castling.b_ks = available;
	}
}

void gstate_set_castling_rights_queenside(GameState *state, Player player, bool available) {
	assert(state != NULL);
	assert(player != NONE);
	if (player == WHITE_PLAYER) {
		state->castling.w_qs = available;
	} else {
		state->castling.b_qs = available;
	}
}

void gstate_set_en_passant_target(GameState *state, Position pos) {
	assert(state != NULL);
	state->en_passant_target = pos;
}

Position gstate_get_en_passant_target(GameState *state) {
	assert(state != NULL);
	return state->en_passant_target;
}

TurnMoves *gstate_get_legal_moves(GameState *state) {
	assert(state != NULL);
	return state->legal_moves;
}

void gstate_set_legal_moves(GameState *state, TurnMoves *moves) {
	assert(state != NULL);
	if (state->legal_moves != NULL) {
		turn_moves_destroy(&state->legal_moves);
	}
	state->legal_moves = moves;
}

int gstate_get_halfmove_clock(GameState *state) {
	assert(state != NULL);
	return state->halfmove_clock;
}

void gstate_set_halfmove_clock(GameState *state, int halfmove_clock) {
	assert(state != NULL);
	state->halfmove_clock = halfmove_clock;
}

static void _remove_qs_castling_rights(GameState *state, Player player) {
	assert(state != NULL);
	assert(player != NONE);

	if (player == WHITE_PLAYER) {
		state->castling.w_qs = false;
	} else {
		state->castling.b_qs = false;
	}
}

static void _remove_ks_castling_rights(GameState *state, Player player) {
	assert(state != NULL);
	assert(player != NONE);

	if (player == WHITE_PLAYER) {
		state->castling.w_ks = false;
	} else {
		state->castling.b_ks = false;
	}
}

static void _remove_all_castling_rights(GameState *state, Player player) {
	assert(state != NULL);
	assert(player != NONE);

	_remove_ks_castling_rights(state, player);
	_remove_qs_castling_rights(state, player);
}

static bool _is_castling_rights_available(GameState *state, Player player) {
	assert(state != NULL);
	assert(player != NONE);
	return gstate_has_castling_rights_kingside(state, player) ||
		   gstate_has_castling_rights_queenside(state, player);
}

static bool _is_quiet_move(TurnRecord record) {
	return record.moving_piece.type != PAWN && record.captured_piece.player == NONE;
}

static void _apply_record_to_board(GameState *state, TurnRecord record) {
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
			_remove_all_castling_rights(state, record.moving_piece.player);
			break;

		case MOVE_PROMOTION:
			board_set_piece(
				state->board,
				_get_promoted_piece(record.special_move_info.promotion, record.moving_piece.player),
				record.move.dst);
			break;

		case MOVE_EN_PASSANT:
			board_remove_piece(state->board, state->en_passant_target);
			break;

		case MOVE_REGULAR:
			switch (record.moving_piece.type) {
				case KING:
					if (_is_castling_rights_available(state, record.moving_piece.player)) {
						_remove_all_castling_rights(state, record.moving_piece.player);
					}
					break;

				case ROOK:
					if (record.move.src.x == 0 &&
						gstate_has_castling_rights_queenside(state, record.moving_piece.player)) {
						_remove_qs_castling_rights(state, record.moving_piece.player);
					} else if (record.move.src.x == 7 &&
							   gstate_has_castling_rights_kingside(state, record.moving_piece.player)) {
						_remove_ks_castling_rights(state, record.moving_piece.player);
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

static TurnRecord _create_turn_record(GameState *state, Move move, MoveType move_type) {
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
			tr.special_move_info.promotion = _get_promotion_type(state, moving_piece.player);
			break;

		case MOVE_INVALID:
			log_error("Attempted to apply an invalid move type");
			exit(1);
	}
	return tr;
}

bool gstate_apply_move(GameState *state, Move move, MoveType move_type) {
	TurnRecord tr = _create_turn_record(state, move, move_type);

	_apply_record_to_board(state, tr);
	history_append(state->history, tr);

	if (_is_quiet_move(tr)) {
		state->halfmove_clock++;
	} else {
		state->halfmove_clock = 0;
	}

	return true;
}
