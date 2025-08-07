#include "board.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "bitboards.h"
#include "bits.h"
#include "fen.h"
#include "log.h"
#include "types.h"
#include "utils.h"

#define TOTAL_PIECE_TYPES 6

static bool is_within_bounds(Square sqr);
Player		board_get_piece_player(PieceType piece);

void board_set_castling_rights(Board *board, CastlingRights cr) {
	assert(board != NULL);
	board->castling_rights |= cr;
}

void board_remove_castling_rights(Board *board, CastlingRights cr) {
	assert(board != NULL);
	board->castling_rights &= ~cr;
}

CastlingRights board_get_castling_rights(const Board *board, Player player) {
	if (player == PLAYER_W) {
		return board->castling_rights & CASTLING_WHITE_ALL;
	} else {
		return board->castling_rights & CASTLING_BLACK_ALL;
	}
}

bool board_has_castling_rights(const Board *board, CastlingRights cr) {
	assert(board != NULL);
	return (board->castling_rights & cr) == cr;
}

Player board_get_occupant(const Board *board, Square sqr) {
	assert(board != NULL);
	assert(is_within_bounds(sqr));
	if (bits_get(board->occupancies[PLAYER_W], sqr)) {
		return PLAYER_W;
	} else if (bits_get(board->occupancies[PLAYER_B], sqr)) {
		return PLAYER_B;
	} else {
		return PLAYER_NONE;
	}
}

Player board_get_player_turn(const Board *board) {
	return board->side;
}

Board *board_create(void) {
	Board *b;
	b = calloc(1, sizeof(*b));
	if (!b) {
		log_error("Failed to allocate board");
		return NULL;
	}
	board_set_castling_rights(b, CASTLING_ALL_RIGHTS);
	b->side		 = PLAYER_W;
	b->ep_target = SQ_NONE;
	if (!history_create(&b->history)) {
		log_error("Failed to allocate history");
		free(b);
		return NULL;
	}
	return b;
}

void board_destroy(Board **board) {
	if (board && *board) {
		free(*board);
		*board = NULL;
	}
}

Board *board_clone(const Board *board) {
	Board *b = board_create();
	memcpy(b, board, sizeof(Board));
	HistoryList *hl = NULL;
	history_create(&hl);
	for (size_t i = 0; i < history_size(board->history); i++) {
		History h;
		history_get(board->history, i, &h);
		history_append(hl, h);
	}
	b->history = hl;
	return b;
}

static bool is_within_bounds(Square sqr) {
	return sqr > SQ_NONE && sqr < SQ_CNT;
}

void board_set_piece(Board *board, Piece piece, Square sqr) {
	assert(board != NULL);
	assert(is_within_bounds(sqr));
	if (bits_get(
			board->occupancies[piece.player] | board->occupancies[utils_get_opponent(piece.player)],
			sqr)) {
		board_remove_piece(board, sqr);
	}
	bits_set(&board->pieces[piece.player][piece.type], sqr);
	bits_set(&board->occupancies[piece.player], sqr);
}

void board_remove_piece(Board *board, Square sqr) {
	assert(board != NULL);
	assert(is_within_bounds(sqr));

	bits_clear(&board->occupancies[PLAYER_W], sqr);
	bits_clear(&board->occupancies[PLAYER_B], sqr);
	for (int i = 0; i < TOTAL_PIECE_TYPES; i++) {
		bits_clear(&board->pieces[PLAYER_W][i], sqr);
		bits_clear(&board->pieces[PLAYER_B][i], sqr);
	}
}

void board_move_piece(Board *board, Square from, Square to, PieceType piece) {
	assert(board != NULL);
	assert(is_within_bounds(from));
	assert(is_within_bounds(to));
	Player p = board_get_occupant(board, from);
	assert(p != PLAYER_NONE);
	board_remove_piece(board, from);
	board_set_piece(board, (Piece) {.player = p, .type = piece}, to);
}

PieceType board_get_piece_type(const Board *board, Square sqr) {
	assert(board != NULL);
	assert(is_within_bounds(sqr));
	Player p = board_get_occupant(board, sqr);
	if (p == PLAYER_NONE) {
		return EMPTY;
	}
	for (int i = 0; i < TOTAL_PIECE_TYPES; i++) {
		if (bits_get(board->pieces[p][i], sqr)) {
			return i;
		}
	}
	return EMPTY;
}

Piece board_get_piece(const Board *board, Square sqr) {
	Player p = board_get_occupant(board, sqr);
	if (p == PLAYER_NONE) {
		return (Piece) {.type = EMPTY, .player = PLAYER_NONE};
	}
	PieceType piece = board_get_piece_type(board, sqr);
	return (Piece) {.type = piece, .player = p};
}

void board_apply_history(Board *board, History hist) {
	board_move_piece(board, hist.to, hist.from, hist.moving);
	if (hist.captured != EMPTY) {
		Square target = hist.mv_type == MV_EN_PASSANT
							? utils_ep_capture_pos(hist.ep_target, hist.side)
							: hist.to;
		board_set_piece(board,
						(Piece) {.player = utils_get_opponent(hist.side), .type = hist.captured},
						target);
	}
	if (hist.mv_type == MV_KS_CASTLE) {
		board_remove_piece(board, hist.side == PLAYER_W ? SQ_F1 : SQ_F8);
		board_set_piece(board,
						(Piece) {.player = hist.side, .type = ROOK},
						hist.side == PLAYER_W ? SQ_H1 : SQ_H8);
	}
	if (hist.mv_type == MV_QS_CASTLE) {
		board_remove_piece(board, hist.side == PLAYER_W ? SQ_D1 : SQ_D8);
		board_set_piece(board,
						(Piece) {.player = hist.side, .type = ROOK},
						hist.side == PLAYER_W ? SQ_A1 : SQ_A8);
	}
	board->side				= hist.side;
	board->ep_target		= hist.ep_target;
	board->castling_rights	= hist.castling_rights;
	board->halfmove_clock	= hist.halfmove_clock;
	board->fullmove_counter = hist.fullmove_counter;
}

bool board_from_fen(Board *board, const char *fen) {
	if (!fen_parse(fen, board)) {
		log_error("Error parsing FEN");
		return false;
	};
	board_print(board);
	return true;
}

void board_print(const Board *board) {
	for (int row = 7; row >= 0; row--) {
		printf("\n %d ", row + 1);
		for (int col = 0; col < 8; col++) {
			Piece p = board_get_piece(board, row * 8 + col);
			printf(" %s", utils_piece_to_unicode_str(p));
		}
	}
	printf("\n    a b c d e f g h \n");
}

bool board_is_square_threatened(const Board *board, Square sqr, Player player) {
	Player	 opponent	 = utils_get_opponent(player);
	uint64_t occupancies = board->occupancies[player] | board->occupancies[opponent];

	uint64_t pawn_bb =
		bitboards_get_pawn_attacks(sqr, player);  // get the attacks for our own pawns to reflect
												  // the position where the enemy pawns could be
	uint64_t knight_bb = bitboards_get_knight_attacks(sqr);
	uint64_t bishop_bb = bitboards_get_bishop_attacks(sqr, occupancies);
	uint64_t rook_bb   = bitboards_get_rook_attacks(sqr, occupancies);
	uint64_t queen_bb  = bitboards_get_queen_attacks(sqr, occupancies);
	uint64_t king_bb   = bitboards_get_king_attacks(sqr);

	// check if any of these pieces exist in the bitboards from the opponent
	// and if so, the square is threatened
	return pawn_bb & board->pieces[opponent][PAWN] || knight_bb & board->pieces[opponent][KNIGHT] ||
		   bishop_bb & board->pieces[opponent][BISHOP] || rook_bb & board->pieces[opponent][ROOK] ||
		   queen_bb & board->pieces[opponent][QUEEN] || king_bb & board->pieces[opponent][KING];
}

bool board_is_check(const Board *board, Player player) {
	Square king_sqr = bits_get_lsb(board->pieces[player][KING]);
	return board_is_square_threatened(board, king_sqr, player);
}
