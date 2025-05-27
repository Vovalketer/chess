#include "../include/board.h"

#include "criterion/criterion.h"

BoardState *board = NULL;

void setup(void) {
	board_create(&board);
}

void teardown(void) {
	board_destroy(&board);
	board = NULL;
}

Test(board, create_returns_true_if_successful) {
	BoardState *_board = NULL;
	bool result = board_create(&_board);
	cr_assert(result);
	cr_assert_not_null(_board);
}

Test(board, destroy_destroys_board) {
	BoardState *_board = NULL;
	bool result = board_create(&_board);
	cr_assert(result);
	cr_assert_not_null(_board);
	board_destroy(&_board);
	cr_assert_null(_board);
}

Test(board, destroy_null_board_doesnt_segfault) {
	board_destroy(NULL);
}

Test(board, set_piece_returns_false_when_out_of_bounds, .init = setup, .fini = teardown) {
	Piece piece = (Piece) {WHITE_PLAYER, ROOK};
	bool result = board_set_piece(board, piece, (Position) {-1, -1});
	cr_assert_not(result);
}

Test(board, set_piece_returns_true_when_successful, .init = setup, .fini = teardown) {
	Piece piece = (Piece) {WHITE_PLAYER, ROOK};
	bool result = board_set_piece(board, piece, (Position) {0, 0});
	cr_assert(result);
}

Test(board, set_piece_sets_piece, .init = setup, .fini = teardown) {
	Piece piece = (Piece) {WHITE_PLAYER, ROOK};
	bool result = board_set_piece(board, piece, (Position) {0, 0});
	cr_assert(result);
	Piece p = board_get_piece(board, (Position) {0, 0});
	cr_assert_eq(p.type, ROOK, "Expected %d, got %d", ROOK, p.type);
}

Test(board, get_piece_returns_false_when_out_of_bounds, .init = setup, .fini = teardown) {
	Piece piece = board_get_piece(board, (Position) {-1, -1});
	cr_assert_eq(piece.type, EMPTY);
}

Test(board, get_piece_returns_piece, .init = setup, .fini = teardown) {
	Position pos = (Position) {0, 0};
	Piece piece = (Piece) {WHITE_PLAYER, ROOK};
	bool result = board_set_piece(board, piece, pos);
	cr_assert(result);
	Piece p = board_get_piece(board, pos);
	cr_assert_eq(p.type, ROOK, "Expected %d, got %d", ROOK, p.type);
}

Test(board, move_piece_returns_false_when_out_of_bounds, .init = setup, .fini = teardown) {
	Position src = (Position) {-10, -10};
	Position dst = (Position) {0, 0};
	bool result = board_move_piece(board, src, dst);
	cr_assert_not(result);
}

Test(board, remove_piece_does_nothing_when_out_of_bounds, .init = setup, .fini = teardown) {
	board_remove_piece(board, (Position) {-10, -10});
}

Test(board, remove_piece_is_successful, .init = setup, .fini = teardown) {
	Piece piece = (Piece) {WHITE_PLAYER, ROOK};
	Position pos = (Position) {0, 0};
	bool result = board_set_piece(board, piece, pos);
	cr_assert(result);
	Piece g = board_get_piece(board, pos);
	cr_assert_eq(g.type, ROOK, "Expected %d, got %d", ROOK, g.type);
	board_remove_piece(board, pos);
	Piece p = board_get_piece(board, pos);
	cr_assert_eq(p.type, EMPTY, "Expected %d, got %d", EMPTY, p.type);
}

Test(board, get_turn_returns_zero_when_turn_is_zero, .init = setup, .fini = teardown) {
	int t = board_get_turn(board);
	cr_assert_eq(t, 0);
}

Test(board, next_turn_returns_one_when_transitioning_from_zero_to_one, .init = setup, .fini = teardown) {
	int t = board_next_turn(board);
	cr_assert_eq(t, 1);
}

Test(board, get_turn_returns_one_when_turn_is_one, .init = setup, .fini = teardown) {
	board_next_turn(board);
	int t = board_get_turn(board);
	cr_assert_eq(t, 1);
}

Test(board, get_player_turn_returns_white_when_turn_is_zero, .init = setup, .fini = teardown) {
	Player p = board_get_player_turn(board);
	cr_assert_eq(p, WHITE_PLAYER);
}

Test(board, get_player_turn_returns_black_when_turn_is_one, .init = setup, .fini = teardown) {
	board_next_turn(board);
	Player p = board_get_player_turn(board);
	cr_assert_eq(p, BLACK_PLAYER);
}

Test(board, is_empty_returns_true_when_piece_is_empty, .init = setup, .fini = teardown) {
	bool result = board_is_empty(board, (Position) {0, 0});
	cr_assert(result);
}

Test(board, is_empty_returns_false_when_piece_is_not_empty, .init = setup, .fini = teardown) {
	Piece piece = (Piece) {WHITE_PLAYER, ROOK};
	Position pos = (Position) {0, 0};
	bool result = board_set_piece(board, piece, pos);
	cr_assert(result);
	bool empty = board_is_empty(board, pos);
	cr_assert_not(empty);
}

Test(board, is_within_bounds_returns_true_when_within_bounds, .init = setup, .fini = teardown) {
	bool result = board_is_within_bounds((Position) {0, 0});
	cr_assert(result);
}

Test(board, is_within_bounds_returns_false_when_out_of_bounds, .init = setup, .fini = teardown) {
	bool result = board_is_within_bounds((Position) {-10, -10});
	cr_assert_not(result);
}
