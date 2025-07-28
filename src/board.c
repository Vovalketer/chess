#include "board.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "bits.h"
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

bool board_has_castling_rights(const Board *board, CastlingRights cr) {
	assert(board != NULL);
	return board->castling_rights & cr;
}

bool board_has_enemy(const Board *board, Square sqr, Player player) {
	assert(board != NULL);
	assert(is_within_bounds(sqr));
	Player p = utils_get_opponent(player);
	return bits_get(board->occupancies[p], sqr);
}

bool board_has_ally(const Board *board, Square sqr, Player player) {
	assert(board != NULL);
	assert(is_within_bounds(sqr));
	return bits_get(board->occupancies[player], sqr);
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

bool board_is_occupied(const Board *board, Square sqr) {
	assert(board != NULL);
	assert(is_within_bounds(sqr));
	return board_get_occupant(board, sqr) != PLAYER_NONE;
}

Board *board_create(void) {
	Board *b;
	b = calloc(1, sizeof(*b));
	if (!b) {
		// TODO: logging
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

void board_set_piece(Board *board, Player player, PieceType piece, Square sqr) {
	assert(board != NULL);
	assert(is_within_bounds(sqr));
	if (bits_get(board->occupancies[player], sqr)) {
		board_remove_piece(board, sqr);
	}

	bits_set(&board->pieces[player][piece], sqr);
	bits_set(&board->occupancies[player], sqr);
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

static Piece get_player_piece(const Board *board, Square sqr) {
	Player p = board_get_occupant(board, sqr);
	if (p == PLAYER_NONE) {
		return EMPTY;
	}
	PieceType piece = board_get_piece_type(board, sqr);
	if (p == PLAYER_W) {
		return W_PAWN + piece;
	} else {
		return B_PAWN + piece;
	}
}

void board_apply_history(Board *board, History hist) {
	board_set_piece(board, hist.side, hist.moving, hist.from);
	if (hist.captured != EMPTY) {
		Square target = hist.mv_type == MV_EN_PASSANT ? hist.ep_target : hist.to;
		board_set_piece(board, board_get_opponent(hist.side), hist.captured, target);
	} else {
		board_remove_piece(board, hist.to);
	}
	board->side				= hist.side;
	board->ep_target		= hist.ep_target;
	board->castling_rights	= hist.castling_rights;
	board->halfmove_clock	= hist.halfmove_clock;
	board->fullmove_counter = hist.fullmove_counter;
}

void board_print(const Board *board) {
	for (int row = 7; row >= 0; row--) {
		printf("\n %d ", row + 1);
		for (int col = 0; col < 8; col++) {
			Piece p = get_player_piece(board, row * 8 + col);
			printf("%2c", board_piece_to_char(p));
		}
	}
	printf("\n    a b c d e f g h \n");
}
