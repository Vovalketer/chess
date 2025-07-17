#include "board.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "bits.h"
#include "stdint.h"
#include "types.h"

#define TOTAL_PIECE_TYPES 6

bool   board_is_within_bounds(Square sqr);
Player board_get_piece_player(PieceType piece);

static char board_piece_to_char(Piece piece) {
	switch (piece) {
		case W_PAWN:
			return 'P';
		case W_ROOK:
			return 'R';
		case W_KNIGHT:
			return 'N';
		case W_BISHOP:
			return 'B';
		case W_QUEEN:
			return 'Q';
		case W_KING:
			return 'K';
		case B_PAWN:
			return 'p';
		case B_ROOK:
			return 'r';
		case B_KNIGHT:
			return 'n';
		case B_BISHOP:
			return 'b';
		case B_QUEEN:
			return 'q';
		case B_KING:
			return 'k';
		case EMPTY:
			return '.';
	}
}

static Piece to_piece(Player player, PieceType piece) {
	switch (player) {
		case PLAYER_W:
			return W_PAWN + piece;
		case PLAYER_B:
			return B_PAWN + piece;
		default:
			return EMPTY;
	}
}

static void set_castling_rights(Board *board, CastlingRights cr) {
	assert(board != NULL);
	board->castling_rights |= cr;
}

static void remove_castling_rights(Board *board, CastlingRights cr) {
	assert(board != NULL);
	board->castling_rights &= ~cr;
}

static Player get_opponent(Player player) {
	return player == PLAYER_W ? PLAYER_B : PLAYER_W;
}

static Player get_occupant(const Board *board, Square sqr) {
	assert(board != NULL);
	assert(board_is_within_bounds(sqr));
	if (bits_get(board->occupancies[PLAYER_W], sqr)) {
		return PLAYER_W;
	} else if (bits_get(board->occupancies[PLAYER_B], sqr)) {
		return PLAYER_B;
	} else {
		return PLAYER_NONE;
	}
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

bool board_is_within_bounds(Square sqr) {
	return sqr > SQ_NONE && sqr < SQ_CNT;
}

bool board_set_piece(Board *board, Player player, PieceType piece, Square sqr) {
	assert(board != NULL);
	if (!board_is_within_bounds(sqr)) {
		return false;
	}
	bits_set(&board->pieces[piece], sqr);
	bits_set(&board->occupancies[player], sqr);
	return true;
}

void board_remove_piece(Board *board, Square sqr) {
	assert(board != NULL);
	assert(board_is_within_bounds(sqr));
	Player p = get_occupant(board, sqr);
	if (p == PLAYER_NONE) {
		return;
	};

	bits_clear(&board->occupancies[p], sqr);
	// bulldoze over the arrays to reduce the branching
	for (int i = 0; i < TOTAL_PIECE_TYPES; i++) {
		bits_clear(&board->pieces[i], sqr);
	}
}

PieceType board_get_piece(const Board *board, Square sqr) {
	assert(board != NULL);
	assert(board_is_within_bounds(sqr));
	for (int i = 0; i < TOTAL_PIECE_TYPES; i++) {
		if (bits_get(board->pieces[i], sqr)) {
			return i;
		}
	}
}

Piece get_player_piece(const Board *board, Square sqr) {
	Player p = get_occupant(board, sqr);
	if (p == PLAYER_NONE) {
		return EMPTY;
	}
	PieceType piece = board_get_piece(board, sqr);
	if (p == PLAYER_W) {
		return W_PAWN + piece;
	} else {
		return B_PAWN + piece;
	}
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
