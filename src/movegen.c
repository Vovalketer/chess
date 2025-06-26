#include "../include/movegen.h"

// pseudo valid moves, doesnt check for rules

#include <assert.h>
#include <stdio.h>

#include "board.h"
#define KNIGHT_OFFSET_ROWS 8
#define DIAG_OFFSET_ROWS 4
#define CROSS_OFFSET_ROWS 8
#define MATRIX_COLS 2

static void _gen_offset_matrix(const Board *board, Piece piece, Position pos, const int *offset_matrix,
							   int offset_matrix_rows, MoveList *moves);
static void _gen_pawn(const Board *board, Piece piece, Position pos, MoveList *moves);
static void _gen_rook(const Board *board, Piece piece, Position pos, MoveList *moves);
static void _gen_knight(const Board *board, Piece piece, Position pos, MoveList *moves);
static void _gen_bishop(const Board *board, Piece piece, Position pos, MoveList *moves);
static void _gen_queen(const Board *board, Piece piece, Position pos, MoveList *moves);
static void _gen_king(const Board *board, Piece piece, Position pos, MoveList *moves);

const int KNIGHT_MOVES[KNIGHT_OFFSET_ROWS][MATRIX_COLS] = {
	{-2, -1}, {-2, 1}, {-1, -2}, {-1, 2}, {1, -2}, {1, 2}, {2, -1}, {2, 1}};
const int CROSS_MOVES[CROSS_OFFSET_ROWS][MATRIX_COLS] = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}};
const int DIAG_MOVES[DIAG_OFFSET_ROWS][MATRIX_COLS] = {{-1, -1}, {-1, 1}, {1, -1}, {1, 1}};

bool movegen_generate(const Board *board, Position pos, MoveList *moves) {
	if (!board_is_within_bounds(pos)) {
		return false;
	}

	Piece piece = board_get_piece(board, pos);
	switch (piece.type) {
		case PAWN:
			_gen_pawn(board, piece, pos, moves);
			break;
		case ROOK:
			_gen_rook(board, piece, pos, moves);
			break;
		case KNIGHT:
			_gen_knight(board, piece, pos, moves);
			break;
		case BISHOP:
			_gen_bishop(board, piece, pos, moves);
			break;
		case QUEEN:
			_gen_queen(board, piece, pos, moves);
			break;
		case KING:
			_gen_king(board, piece, pos, moves);
			break;
		default:
			break;
	}

	return true;
}

bool movegen_contains(Board *board, Move move) {
	assert(board != NULL);
	MoveList *ml = NULL;
	move_list_create(&ml);
	bool gen = movegen_generate(board, move.src, ml);
	assert(gen);
	bool contains = move_list_contains(ml, move);

	move_list_destroy(&ml);

	return contains;
}

static void _gen_rook(const Board *board, Piece piece, Position pos, MoveList *moves) {
	_gen_offset_matrix(board, piece, pos, *CROSS_MOVES, CROSS_OFFSET_ROWS, moves);
}

static void _gen_knight(const Board *board, Piece piece, Position pos, MoveList *moves) {
	for (int row = 0; row < 8; row++) {
		int dx = KNIGHT_MOVES[row][0];
		int dy = KNIGHT_MOVES[row][1];
		int target_col = pos.x + dx;
		int target_row = pos.y + dy;
		Position target_pos = {target_col, target_row};
		if (board_is_within_bounds(target_pos)) {
			if (board_is_empty(board, target_pos)) {
				move_list_append(moves, move_create(pos, target_pos));
			} else if (board_is_enemy(board, piece.player, target_pos)) {
				move_list_append(moves, move_create(pos, target_pos));
			}
		}
	}
}

static void _gen_bishop(const Board *board, Piece piece, Position pos, MoveList *moves) {
	_gen_offset_matrix(board, piece, pos, *DIAG_MOVES, DIAG_OFFSET_ROWS, moves);
}

static void _gen_queen(const Board *board, Piece piece, Position pos, MoveList *moves) {
	_gen_offset_matrix(board, piece, pos, *CROSS_MOVES, CROSS_OFFSET_ROWS, moves);
	_gen_offset_matrix(board, piece, pos, *DIAG_MOVES, DIAG_OFFSET_ROWS, moves);
}

static void _gen_king(const Board *board, Piece piece, Position pos, MoveList *moves) {
	for (int row = -1; row < 2; row++) {
		for (int col = -1; col < 2; col++) {
			if (row == pos.y && col == pos.x) {
				continue;
			}
			int target_col = pos.x + col;
			int target_row = pos.y + row;
			Position target_pos = {target_col, target_row};
			if (board_is_within_bounds(target_pos)) {
				if (board_is_empty(board, target_pos)) {
					bool added = move_list_append(moves, move_create(pos, target_pos));
					if (!added) {
						printf("failed to add move\n");
					}
				} else if (board_is_enemy(board, piece.player, target_pos)) {
					bool added = move_list_append(moves, move_create(pos, target_pos));
					if (!added) {
						printf("failed to add move\n");
					}
				}
			}
		}
	}
}

static void _gen_offset_matrix(const Board *board, Piece piece, Position pos, const int *offset_matrix,
							   int offset_matrix_rows, MoveList *moves) {
	for (int row = 0; row < offset_matrix_rows; row++) {
		int dx = offset_matrix[row * 2];
		int dy = offset_matrix[row * 2 + 1];
		int target_col = pos.x + dx;
		int target_row = pos.y + dy;
		Position target_pos = {target_col, target_row};
		while (board_is_within_bounds(target_pos)) {
			if (board_is_empty(board, target_pos)) {
				bool added = move_list_append(moves, move_create(pos, target_pos));
				if (!added) {
					printf("failed to add move\n");
				}
			} else if (board_is_enemy(board, piece.player, target_pos)) {
				bool added = move_list_append(moves, move_create(pos, target_pos));
				if (!added) {
					printf("failed to add move\n");
				}
				break;
			} else {
				// allied piece
				break;
			}

			target_pos.x += dx;
			target_pos.y += dy;
		}
	}
}

static void _gen_pawn(const Board *board, Piece piece, Position pos, MoveList *moves) {
	int starting_row;
	Position forward;
	Position forward2;
	Position forward_left;
	Position forward_right;

	if (piece.player == WHITE_PLAYER) {
		starting_row = 6;
		forward = (Position) {pos.x, pos.y - 1};
		forward2 = (Position) {pos.x, pos.y - 2};
		forward_left = (Position) {pos.x - 1, pos.y - 1};
		forward_right = (Position) {pos.x + 1, pos.y - 1};
	} else {
		starting_row = 1;
		forward = (Position) {pos.x, pos.y + 1};
		forward2 = (Position) {pos.x, pos.y + 2};
		forward_left = (Position) {pos.x - 1, pos.y + 1};
		forward_right = (Position) {pos.x + 1, pos.y + 1};
	}
	if (pos.y != starting_row) {
		if (board_is_within_bounds(forward) && board_is_empty(board, forward)) {
			move_list_append(moves, move_create(pos, forward));
		}
	} else {
		if (board_is_empty(board, forward) && board_is_empty(board, forward2)) {
			move_list_append(moves, move_create(pos, forward2));
			move_list_append(moves, move_create(pos, forward));
		}
	}
	// check for possible captures
	if (board_is_enemy(board, piece.player, forward_left)) {
		move_list_append(moves, move_create(pos, forward_left));
	}
	if (board_is_enemy(board, piece.player, forward_right)) {
		move_list_append(moves, move_create(pos, forward_right));
	}
}
