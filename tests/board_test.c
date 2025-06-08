#include "../include/match.h"

#include "criterion/criterion.h"

MatchState *board = NULL;

void setup(void) {
	match_create(&board);
}

void teardown(void) {
	match_destroy(&board);
	board = NULL;
}

Test(board, create_returns_true_if_successful) {
	MatchState *_board = NULL;
	bool result = match_create(&_board);
	cr_assert(result);
	cr_assert_not_null(_board);
	match_destroy(&_board);
}

Test(board, destroy_destroys_board) {
	MatchState *_board = NULL;
	bool result = match_create(&_board);
	cr_assert(result);
	cr_assert_not_null(_board);
	match_destroy(&_board);
	cr_assert_null(_board);
}

Test(board, destroy_null_match_doesnt_segfault) {
	match_destroy(NULL);
}

Test(board, set_piece_returns_false_when_out_of_bounds, .init = setup, .fini = teardown) {
	Piece piece = (Piece) {WHITE_PLAYER, ROOK};
	bool result = match_set_piece(board, piece, (Position) {-1, -1});
	cr_assert_not(result);
}

Test(board, set_piece_returns_true_when_successful, .init = setup, .fini = teardown) {
	Piece piece = (Piece) {WHITE_PLAYER, ROOK};
	bool result = match_set_piece(board, piece, (Position) {0, 0});
	cr_assert(result);
}

Test(board, set_piece_sets_piece, .init = setup, .fini = teardown) {
	Piece piece = (Piece) {WHITE_PLAYER, ROOK};
	bool result = match_set_piece(board, piece, (Position) {0, 0});
	cr_assert(result);
	Piece p = match_get_piece(board, (Position) {0, 0});
	cr_assert_eq(p.type, ROOK, "Expected %d, got %d", ROOK, p.type);
}

Test(board, get_piece_returns_false_when_out_of_bounds, .init = setup, .fini = teardown) {
	Piece piece = match_get_piece(board, (Position) {-1, -1});
	cr_assert_eq(piece.type, EMPTY);
}

Test(board, get_piece_returns_piece, .init = setup, .fini = teardown) {
	Position pos = (Position) {0, 0};
	Piece piece = (Piece) {WHITE_PLAYER, ROOK};
	bool result = match_set_piece(board, piece, pos);
	cr_assert(result);
	Piece p = match_get_piece(board, pos);
	cr_assert_eq(p.type, ROOK, "Expected %d, got %d", ROOK, p.type);
}

Test(board, move_piece_returns_false_when_out_of_bounds, .init = setup, .fini = teardown) {
	Position src = (Position) {-10, -10};
	Position dst = (Position) {0, 0};
	bool result = match_move_piece(board, src, dst);
	cr_assert_not(result);
}

Test(board, remove_piece_does_nothing_when_out_of_bounds, .init = setup, .fini = teardown) {
	match_remove_piece(board, (Position) {-10, -10});
}

Test(board, remove_piece_is_successful, .init = setup, .fini = teardown) {
	Piece piece = (Piece) {WHITE_PLAYER, ROOK};
	Position pos = (Position) {0, 0};
	bool result = match_set_piece(board, piece, pos);
	cr_assert(result);
	Piece g = match_get_piece(board, pos);
	cr_assert_eq(g.type, ROOK, "Expected %d, got %d", ROOK, g.type);
	match_remove_piece(board, pos);
	Piece p = match_get_piece(board, pos);
	cr_assert_eq(p.type, EMPTY, "Expected %d, got %d", EMPTY, p.type);
}

Test(board, get_turn_returns_zero_when_turn_is_zero, .init = setup, .fini = teardown) {
	int t = match_get_turn(board);
	cr_assert_eq(t, 0);
}

Test(board, next_turn_returns_one_when_transitioning_from_zero_to_one, .init = setup, .fini = teardown) {
	int t = match_next_turn(board);
	cr_assert_eq(t, 1);
}

Test(board, get_turn_returns_one_when_turn_is_one, .init = setup, .fini = teardown) {
	match_next_turn(board);
	int t = match_get_turn(board);
	cr_assert_eq(t, 1);
}

Test(board, get_player_turn_returns_white_when_turn_is_zero, .init = setup, .fini = teardown) {
	Player p = match_get_player_turn(board);
	cr_assert_eq(p, WHITE_PLAYER);
}

Test(board, get_player_turn_returns_black_when_turn_is_one, .init = setup, .fini = teardown) {
	match_next_turn(board);
	Player p = match_get_player_turn(board);
	cr_assert_eq(p, BLACK_PLAYER);
}

Test(board, is_empty_returns_true_when_piece_is_empty, .init = setup, .fini = teardown) {
	bool result = match_is_empty(board, (Position) {0, 0});
	cr_assert(result);
}

Test(board, is_empty_returns_false_when_piece_is_not_empty, .init = setup, .fini = teardown) {
	Piece piece = (Piece) {WHITE_PLAYER, ROOK};
	Position pos = (Position) {0, 0};
	bool result = match_set_piece(board, piece, pos);
	cr_assert(result);
	bool empty = match_is_empty(board, pos);
	cr_assert_not(empty);
}

Test(board, is_within_bounds_returns_true_when_within_bounds, .init = setup, .fini = teardown) {
	bool result = match_is_within_bounds((Position) {0, 0});
	cr_assert(result);
}

Test(board, is_within_bounds_returns_false_when_out_of_bounds, .init = setup, .fini = teardown) {
	bool result = match_is_within_bounds((Position) {-10, -10});
	cr_assert_not(result);
}

Test(board, find_king_pos_returns_white_king_position, .init = setup, .fini = teardown) {
	bool result = match_set_piece(board, (Piece) {WHITE_PLAYER, KING}, (Position) {4, 7});
	cr_assert(result);
	Position pos = match_find_king_pos(board, WHITE_PLAYER);
	cr_assert_eq(pos.x, 4);
	cr_assert_eq(pos.y, 7);
}

Test(board, find_king_pos_returns_black_king_position, .init = setup, .fini = teardown) {
	bool result = match_set_piece(board, (Piece) {BLACK_PLAYER, KING}, (Position) {4, 0});
	cr_assert(result);
	Position pos = match_find_king_pos(board, BLACK_PLAYER);
	cr_assert_eq(pos.x, 4);
	cr_assert_eq(pos.y, 0);
}

Test(board, clone_returns_true_when_successful, .init = setup, .fini = teardown) {
	MatchState *dst;
	bool result = match_clone(&dst, board);
	cr_assert(result);
	match_destroy(&dst);
}

Test(board, clone_returns_match_with_the_same_state, .init = setup, .fini = teardown) {
	MatchState *clone;
	match_set_piece(board, (Piece) {WHITE_PLAYER, ROOK}, (Position) {0, 0});
	match_set_piece(board, (Piece) {WHITE_PLAYER, KNIGHT}, (Position) {1, 0});
	match_set_piece(board, (Piece) {WHITE_PLAYER, BISHOP}, (Position) {2, 0});
	match_next_turn(board);
	match_next_turn(board);
	match_next_turn(board);
	bool result = match_clone(&clone, board);
	cr_assert(result);

	cr_assert_eq(match_get_turn(board), match_get_turn(clone));

	Piece rook = match_get_piece(clone, (Position) {0, 0});
	Piece knight = match_get_piece(clone, (Position) {1, 0});
	Piece bishop = match_get_piece(clone, (Position) {2, 0});

	cr_assert_eq(rook.type, ROOK);
	cr_assert_eq(knight.type, KNIGHT);
	cr_assert_eq(bishop.type, BISHOP);

	match_destroy(&clone);
}

Test(board, append_turn_record_returns_true_when_successful, .init = setup, .fini = teardown) {
	Move move = (Move) {(Position) {0, 0}, (Position) {0, 1}};
	bool result = match_append_turn_record(board, move);
	cr_assert(result);
}

Test(board, append_turn_record_appends_move_to_history, .init = setup, .fini = teardown) {
	Move move = (Move) {(Position) {0, 0}, (Position) {0, 1}};
	bool result = match_append_turn_record(board, move);
	cr_assert(result);
	TurnRecord *record = NULL;
	match_get_turn_record(board, 0, &record);
	cr_assert(move_eq(move, record->move));
	cr_assert_eq(record->turn, 0);
	cr_assert_eq(record->captured_piece.type, EMPTY);
	cr_assert_eq(record->player, WHITE_PLAYER);
}
