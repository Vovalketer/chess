#include "rules.h"

#include "board.h"
#include "criterion/criterion.h"
#include "criterion/new/assert.h"
#include "match.h"
#include "types.h"
MatchState *match = NULL;

void setup(void) {
	match_create_empty(&match);
}

void teardown(void) {
	match_destroy(&match);
	match = NULL;
}

TestSuite(rules, .init = setup, .fini = teardown);

Test(rules, is_check_returns_true_when_king_is_endangered) {
	struct Board *board = match_get_board(match);

	board_set_piece(board, (Piece) {.player = WHITE_PLAYER, .type = KING}, (Position) {0, 0});
	board_set_piece(board, (Piece) {.player = BLACK_PLAYER, .type = QUEEN}, (Position) {1, 0});

	bool is_check = rules_is_check(match, WHITE_PLAYER);
	cr_assert_eq(is_check, true);
}

Test(rules, is_check_returns_false_when_king_is_not_endangered) {
	struct Board *board = match_get_board(match);

	board_set_piece(board, (Piece) {.player = WHITE_PLAYER, .type = KING}, (Position) {0, 0});
	board_set_piece(board, (Piece) {.player = WHITE_PLAYER, .type = ROOK}, (Position) {1, 0});

	bool is_check = rules_is_check(match, WHITE_PLAYER);
	cr_assert_eq(is_check, false);
}

Test(rules, is_check_after_move_returns_true_when_king_would_be_endangered) {
	struct Board *board = match_get_board(match);

	board_set_piece(board, (Piece) {.player = WHITE_PLAYER, .type = KING}, (Position) {0, 0});
	board_set_piece(board, (Piece) {.player = BLACK_PLAYER, .type = QUEEN}, (Position) {2, 1});

	bool is_check = rules_is_check_after_move(match, (Move) {(Position) {0, 0}, (Position) {1, 0}});
	cr_assert_eq(is_check, true);
}

Test(rules, is_check_after_move_returns_false_when_king_would_not_be_endangered) {
	struct Board *board = match_get_board(match);

	board_set_piece(board, (Piece) {.player = WHITE_PLAYER, .type = KING}, (Position) {0, 0});
	board_set_piece(board, (Piece) {.player = BLACK_PLAYER, .type = ROOK}, (Position) {3, 1});

	bool is_check = rules_is_check_after_move(match, (Move) {(Position) {0, 0}, (Position) {1, 0}});
	cr_assert_eq(is_check, false);
}

Test(rules, is_check_after_move_returns_false_when_attacker_is_captured) {
	struct Board *board = match_get_board(match);

	board_set_piece(board, (Piece) {.player = WHITE_PLAYER, .type = KING}, (Position) {0, 0});
	board_set_piece(board, (Piece) {.player = BLACK_PLAYER, .type = QUEEN}, (Position) {1, 0});

	bool is_check = rules_is_check_after_move(match, (Move) {(Position) {0, 0}, (Position) {1, 0}});
	cr_assert_eq(is_check, false);
}

Test(rules, is_check_after_move_returns_true_when_move_exposes_king) {
	struct Board *board = match_get_board(match);

	board_set_piece(board, (Piece) {.player = WHITE_PLAYER, .type = KING}, (Position) {0, 0});
	board_set_piece(board, (Piece) {.player = WHITE_PLAYER, .type = ROOK}, (Position) {1, 0});
	board_set_piece(board, (Piece) {.player = BLACK_PLAYER, .type = QUEEN}, (Position) {2, 0});

	bool is_check = rules_is_check_after_move(match, (Move) {(Position) {1, 0}, (Position) {1, 4}});
	cr_assert_eq(is_check, true);
}

Test(rules, is_check_after_move_returns_true_when_capturing_attacker_exposes_king) {
	struct Board *board = match_get_board(match);

	board_set_piece(board, (Piece) {.player = WHITE_PLAYER, .type = KING}, (Position) {0, 0});
	board_set_piece(board, (Piece) {.player = BLACK_PLAYER, .type = ROOK}, (Position) {1, 0});
	board_set_piece(board, (Piece) {.player = BLACK_PLAYER, .type = QUEEN}, (Position) {2, 0});

	bool is_check = rules_is_check_after_move(match, (Move) {(Position) {0, 0}, (Position) {1, 0}});
	cr_assert_eq(is_check, true);
}

Test(rules, is_checkmate_returns_true_when_king_cannot_escape) {
	struct Board *board = match_get_board(match);

	board_set_piece(board, (Piece) {.player = WHITE_PLAYER, .type = KING}, (Position) {0, 0});
	board_set_piece(board, (Piece) {.player = BLACK_PLAYER, .type = ROOK}, (Position) {1, 0});
	board_set_piece(board, (Piece) {.player = BLACK_PLAYER, .type = QUEEN}, (Position) {1, 1});

	bool is_checkmate = rules_is_checkmate(match, WHITE_PLAYER);
	cr_assert_eq(is_checkmate, true);
}

Test(rules, is_checkmate_returns_false_when_king_can_escape) {
	struct Board *board = match_get_board(match);

	board_set_piece(board, (Piece) {.player = WHITE_PLAYER, .type = KING}, (Position) {0, 0});
	board_set_piece(board, (Piece) {.player = BLACK_PLAYER, .type = ROOK}, (Position) {2, 0});
	board_set_piece(board, (Piece) {.player = BLACK_PLAYER, .type = QUEEN}, (Position) {5, 2});

	bool is_checkmate = rules_is_checkmate(match, WHITE_PLAYER);
	cr_assert_eq(is_checkmate, false);
}
