#include "board.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
	return b;
}

void board_destroy(Board **board) {
	if (board && *board) {
		free(*board);
		*board = NULL;
	}
}

static bool is_within_bounds(Square sqr) {
	return sqr > SQ_NONE && sqr < SQ_CNT;
}

void board_set_piece(Board *board, Piece piece, Square sqr) {
	assert(board != NULL);
	printf("Setting piece %d at %d\n", piece.type, sqr);
	assert(is_within_bounds(sqr));
	if (bits_get(board->occupancies[piece.player], sqr)) {
		board_remove_piece(board, sqr);
	}

	bits_set(&board->pieces[piece.player][piece.type], sqr);
	bits_set(&board->occupancies[piece.player], sqr);
}

void board_remove_piece(Board *board, Square sqr) {
	assert(board != NULL);
	assert(is_within_bounds(sqr));
	Player p = board_get_occupant(board, sqr);
	if (p == PLAYER_NONE) {
		return;
	};

	bits_clear(&board->occupancies[p], sqr);
	// bulldoze over the arrays to reduce the branching
	for (int i = 0; i < TOTAL_PIECE_TYPES; i++) {
		bits_clear(&board->pieces[p][i], sqr);
	}
}

void board_move_piece(Board *board, Square from, Square to, PieceType piece) {
	assert(board != NULL);
	assert(is_within_bounds(from));
	assert(is_within_bounds(to));
	Player p = board_get_occupant(board, from);
	assert(p != PLAYER_NONE);

	bits_clear(&board->occupancies[p], from);
	bits_set(&board->occupancies[p], to);
	bits_clear(&board->pieces[p][piece], from);
	bits_set(&board->pieces[p][piece], to);
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
	board_set_piece(board, (Piece) {.player = hist.side, .type = hist.moving}, hist.from);
	if (hist.captured != EMPTY) {
		Square target = hist.mv_type == MV_EN_PASSANT ? hist.ep_target : hist.to;
		board_set_piece(board,
						(Piece) {.player = utils_get_opponent(hist.side), .type = hist.captured},
						target);
	} else {
		board_remove_piece(board, hist.to);
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
	return true;
}

void board_print(const Board *board) {
	for (int row = 7; row >= 0; row--) {
		printf("\n %d ", row + 1);
		for (int col = 0; col < 8; col++) {
			Piece p = board_get_piece(board, row * 8 + col);
			printf("%2c", utils_piece_to_char(p));
		}
	}
	printf("\n    a b c d e f g h \n");
}
