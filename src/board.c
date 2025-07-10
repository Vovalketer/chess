#include "board.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct Board {
	Piece board[8][8];
};

void board_print(Board *board) {
	for (int i = 0; i < 8; i++) {
		for (int j = 0; j < 8; j++) {
			switch (board->board[i][j].type) {
				case PAWN:
					board->board[i][j].player == WHITE_PLAYER ? printf(" P") : printf(" p");
					break;
				case ROOK:
					board->board[i][j].player == WHITE_PLAYER ? printf(" R") : printf(" r");
					break;
				case KNIGHT:
					board->board[i][j].player == WHITE_PLAYER ? printf(" N") : printf(" n");
					break;
				case BISHOP:
					board->board[i][j].player == WHITE_PLAYER ? printf(" B") : printf(" b");
					break;
				case QUEEN:
					board->board[i][j].player == WHITE_PLAYER ? printf(" Q") : printf(" q");
					break;
				case KING:
					board->board[i][j].player == WHITE_PLAYER ? printf(" K") : printf(" k");
					break;
				case EMPTY:
					printf(" .");
					break;
				default:
					printf("?");
					break;
			}
		}
		printf("\n");
	}
}

bool board_create(Board **board) {
	assert(board != NULL);
	Board *b;
	b = malloc(sizeof(Board));
	if (!b) {
		return false;
	}
	*board = b;
	return true;
}

bool board_destroy(Board **board) {
	if (board && *board) {
		free(*board);
		*board = NULL;
	}
	return true;
}

bool board_clone(Board **clone, const Board *board) {
	assert(clone != NULL);
	assert(board != NULL);
	bool result = board_create(clone);
	if (!result) {
		return false;
	}
	memcpy(*clone, board, sizeof(Board));
	return true;
}

bool board_is_within_bounds(Position pos) {
	return pos.x >= 0 && pos.x <= 7 && pos.y >= 0 && pos.y <= 7;
}

bool board_set_piece(Board *board, Piece piece, Position pos) {
	assert(board != NULL);
	if (!board_is_within_bounds(pos)) {
		return false;
	}
	board->board[pos.y][pos.x] = piece;
	return true;
}

void board_remove_piece(Board *board, Position pos) {
	assert(board != NULL);
	board_set_piece(board, (Piece) {NONE, EMPTY}, pos);
}

Piece board_get_piece(const Board *board, Position pos) {
	assert(board != NULL);
	if (!board_is_within_bounds(pos)) {
		return (Piece) {NONE, EMPTY};
	}
	return board->board[pos.y][pos.x];
}

bool board_move_piece(Board *board, Position src, Position dst) {
	assert(board != NULL);
	if (!board_is_within_bounds(src) || !board_is_within_bounds(dst)) {
		return false;
	}
	Piece orig = board_get_piece(board, src);
	if (orig.type == EMPTY) {
		return false;
	}
	board_set_piece(board, orig, dst);
	board_remove_piece(board, src);
	return true;
}

void board_init_positions(Board *board) {
	for (int i = 0; i < 8; i++) {
		board_set_piece(board, (Piece) {WHITE_PLAYER, PAWN}, (Position) {i, 6});
	}

	board_set_piece(board, (Piece) {WHITE_PLAYER, ROOK}, (Position) {0, 7});
	board_set_piece(board, (Piece) {WHITE_PLAYER, KNIGHT}, (Position) {1, 7});
	board_set_piece(board, (Piece) {WHITE_PLAYER, BISHOP}, (Position) {2, 7});
	board_set_piece(board, (Piece) {WHITE_PLAYER, QUEEN}, (Position) {3, 7});
	board_set_piece(board, (Piece) {WHITE_PLAYER, KING}, (Position) {4, 7});
	board_set_piece(board, (Piece) {WHITE_PLAYER, BISHOP}, (Position) {5, 7});
	board_set_piece(board, (Piece) {WHITE_PLAYER, KNIGHT}, (Position) {6, 7});
	board_set_piece(board, (Piece) {WHITE_PLAYER, ROOK}, (Position) {7, 7});

	for (int i = 0; i < 8; i++) {
		board_set_piece(board, (Piece) {BLACK_PLAYER, PAWN}, (Position) {i, 1});
	}

	board_set_piece(board, (Piece) {BLACK_PLAYER, ROOK}, (Position) {0, 0});
	board_set_piece(board, (Piece) {BLACK_PLAYER, KNIGHT}, (Position) {1, 0});
	board_set_piece(board, (Piece) {BLACK_PLAYER, BISHOP}, (Position) {2, 0});
	board_set_piece(board, (Piece) {BLACK_PLAYER, QUEEN}, (Position) {3, 0});
	board_set_piece(board, (Piece) {BLACK_PLAYER, KING}, (Position) {4, 0});
	board_set_piece(board, (Piece) {BLACK_PLAYER, BISHOP}, (Position) {5, 0});
	board_set_piece(board, (Piece) {BLACK_PLAYER, KNIGHT}, (Position) {6, 0});
	board_set_piece(board, (Piece) {BLACK_PLAYER, ROOK}, (Position) {7, 0});
}

bool board_is_empty(const Board *board, Position pos) {
	assert(board != NULL);
	return board_get_piece(board, pos).type == EMPTY;
}

bool board_is_enemy(const Board *board, Player player, Position pos) {
	assert(board != NULL);
	switch (player) {
		case WHITE_PLAYER:
			return board_get_piece(board, pos).player == BLACK_PLAYER;
		case BLACK_PLAYER:
			return board_get_piece(board, pos).player == WHITE_PLAYER;
		case NONE:
			return false;
	}
	return false;  // should never reach this point but is required by the compiler
}

bool board_is_friendly(const Board *board, Player player, Position pos) {
	assert(board != NULL);
	Piece piece = board_get_piece(board, pos);
	return piece.player == player;
}

Position board_find_king_pos(const Board *board, Player player) {
	assert(board != NULL);
	assert(player != NONE);
	for (int col = 0; col < 8; col++) {
		for (int row = 0; row < 8; row++) {
			Position pos = (Position) {col, row};
			Piece piece = board_get_piece(board, pos);
			if (piece.player == player && piece.type == KING) {
				return pos;
			}
		}
	}
	return (Position) {-1, -1};	 // should never reach this point but is required by the compiler
}
