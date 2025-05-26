#include "../include/movegen.h"
// pseudo valid moves, doesnt check for rules

#include <stdbool.h>
#include <stdio.h>

#include "../include/board.h"
#define KNIGHT_OFFSET_ROWS 8
#define DIAG_OFFSET_ROWS 4
#define CROSS_OFFSET_ROWS 8
#define MATRIX_COLS 2

static void _gen_offset_matrix(const BoardState *board, Piece piece, int x, int y, const int *offset_matrix,
							   int offset_matrix_rows, MoveList *moves);
static void _gen_pawn(const BoardState *board, Piece piece, int x, int y, MoveList *moves);
static void _gen_rook(const BoardState *board, Piece piece, int x, int y, MoveList *moves);
static void _gen_knight(const BoardState *board, Piece piece, int x, int y, MoveList *moves);
static void _gen_bishop(const BoardState *board, Piece piece, int x, int y, MoveList *moves);
static void _gen_queen(const BoardState *board, Piece piece, int x, int y, MoveList *moves);
static void _gen_king(const BoardState *board, Piece piece, int x, int y, MoveList *moves);

const int KNIGHT_MOVES[KNIGHT_OFFSET_ROWS][MATRIX_COLS] = {
	{-2, -1}, {-2, 1}, {-1, -2}, {-1, 2}, {1, -2}, {1, 2}, {2, -1}, {2, 1}};
const int CROSS_MOVES[CROSS_OFFSET_ROWS][MATRIX_COLS] = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}};
const int DIAG_MOVES[DIAG_OFFSET_ROWS][MATRIX_COLS] = {{-1, -1}, {-1, 1}, {1, -1}, {1, 1}};

bool movegen_generate(const BoardState *board, int x, int y, MoveList *moves) {
	if (!board_is_within_bounds(x, y)) {
		return false;
	}

	Piece piece = board_get_piece(board, x, y);
	switch (piece.type) {
		case PAWN:
			_gen_pawn(board, piece, x, y, moves);
			break;
		case ROOK:
			_gen_rook(board, piece, x, y, moves);
			break;
		case KNIGHT:
			_gen_knight(board, piece, x, y, moves);
			break;
		case BISHOP:
			_gen_bishop(board, piece, x, y, moves);
			break;
		case QUEEN:
			_gen_queen(board, piece, x, y, moves);
			break;
		case KING:
			_gen_king(board, piece, x, y, moves);
			break;
		default:
			break;
	}

	return true;
}

static void _gen_rook(const BoardState *board, Piece piece, int x, int y, MoveList *moves) {
	_gen_offset_matrix(board, piece, x, y, *CROSS_MOVES, CROSS_OFFSET_ROWS, moves);
}

static void _gen_knight(const BoardState *board, Piece piece, int x, int y, MoveList *moves) {
	for (int row = 0; row < 8; row++) {
		int dx = KNIGHT_MOVES[row][0];
		int dy = KNIGHT_MOVES[row][1];
		int target_col = x + dx;
		int target_row = y + dy;
		if (board_is_within_bounds(target_col, target_row)) {
			if (board_is_empty(board, target_col, target_row)) {
				move_list_add(moves, move_create(x, y, target_col, target_row));
			} else if (board_is_enemy(board, piece.player, target_col, target_row)) {
				move_list_add(moves, move_create(x, y, target_col, target_row));
			}
		}
	}
}

static void _gen_bishop(const BoardState *board, Piece piece, int x, int y, MoveList *moves) {
	_gen_offset_matrix(board, piece, x, y, *DIAG_MOVES, DIAG_OFFSET_ROWS, moves);
}

static void _gen_queen(const BoardState *board, Piece piece, int x, int y, MoveList *moves) {
	_gen_offset_matrix(board, piece, x, y, *CROSS_MOVES, CROSS_OFFSET_ROWS, moves);
	_gen_offset_matrix(board, piece, x, y, *DIAG_MOVES, DIAG_OFFSET_ROWS, moves);
}

static void _gen_king(const BoardState *board, Piece piece, int x, int y, MoveList *moves) {
	for (int row = -1; row < 2; row++) {
		for (int col = -1; col < 2; col++) {
			if (row == y && col == x) {
				continue;
			}
			int target_col = x + col;
			int target_row = y + row;
			if (board_is_within_bounds(target_col, target_row)) {
				if (board_is_empty(board, target_col, target_row)) {
					bool added = move_list_add(moves, move_create(x, y, target_col, target_row));
					if (!added) {
						printf("failed to add move\n");
					}
				} else if (board_is_enemy(board, piece.player, target_col, target_row)) {
					bool added = move_list_add(moves, move_create(x, y, target_col, target_row));
					if (!added) {
						printf("failed to add move\n");
					}
				}
			}
		}
	}
}

static void _gen_offset_matrix(const BoardState *board, Piece piece, int x, int y, const int *offset_matrix,
							   int offset_matrix_rows, MoveList *moves) {
	for (int row = 0; row < offset_matrix_rows; row++) {
		int dx = offset_matrix[row * 2];
		int dy = offset_matrix[row * 2 + 1];
		int target_col = x + dx;
		int target_row = y + dy;
		while (board_is_within_bounds(target_col, target_row)) {
			if (board_is_empty(board, target_col, target_row)) {
				bool added = move_list_add(moves, move_create(x, y, target_col, target_row));
				if (!added) {
					printf("failed to add move\n");
				}
			} else if (board_is_enemy(board, piece.player, target_col, target_row)) {
				bool added = move_list_add(moves, move_create(x, y, target_col, target_row));
				if (!added) {
					printf("failed to add move\n");
				}
				break;
			} else {
				// allied piece
				break;
			}

			target_col += dx;
			target_row += dy;
		}
	}
}

static void _gen_pawn(const BoardState *board, Piece piece, int x, int y, MoveList *moves) {
	const int left = x - 1;
	const int right = x + 1;
	int starting_row;
	int forward;
	int forward2;

	if (piece.player == WHITE_PLAYER) {
		starting_row = 6;
		forward = y - 1;
		forward2 = y - 2;
	} else {
		starting_row = 1;
		forward = y + 1;
		forward2 = y + 2;
	}
	if (y != starting_row) {
		if (board_is_within_bounds(x, forward) && board_is_empty(board, x, forward)) {
			move_list_add(moves, move_create(x, y, x, forward));
		}
	} else {
		if (board_is_empty(board, x, forward) && board_is_empty(board, x, forward2)) {
			move_list_add(moves, move_create(x, y, x, forward2));
			move_list_add(moves, move_create(x, y, x, forward));
		}
	}
	// check for possible captures
	if (board_is_enemy(board, piece.player, left, forward)) {
		move_list_add(moves, move_create(x, y, left, forward));
	}
	if (board_is_enemy(board, piece.player, right, forward)) {
		move_list_add(moves, move_create(x, y, right, forward));
	}
}
