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
	Board *board = match_get_board(match);

	board_set_piece(board, (Piece) {.player = WHITE_PLAYER, .type = KING}, (Position) {0, 0});
	board_set_piece(board, (Piece) {.player = BLACK_PLAYER, .type = QUEEN}, (Position) {1, 0});

	bool is_check = rules_is_check(match, WHITE_PLAYER);
	cr_assert_eq(is_check, true);
}

Test(rules, is_check_returns_false_when_king_is_not_endangered) {
	Board *board = match_get_board(match);

	board_set_piece(board, (Piece) {.player = WHITE_PLAYER, .type = KING}, (Position) {0, 0});
	board_set_piece(board, (Piece) {.player = WHITE_PLAYER, .type = ROOK}, (Position) {1, 0});

	bool is_check = rules_is_check(match, WHITE_PLAYER);
	cr_assert_eq(is_check, false);
}

Test(rules, is_check_after_move_returns_true_when_king_would_be_endangered) {
	Board *board = match_get_board(match);

	board_set_piece(board, (Piece) {.player = WHITE_PLAYER, .type = KING}, (Position) {0, 0});
	board_set_piece(board, (Piece) {.player = BLACK_PLAYER, .type = QUEEN}, (Position) {2, 1});

	bool is_check = rules_is_check_after_move(match, (Move) {(Position) {0, 0}, (Position) {1, 0}});
	cr_assert_eq(is_check, true);
}

Test(rules, is_check_after_move_returns_false_when_king_would_not_be_endangered) {
	Board *board = match_get_board(match);

	board_set_piece(board, (Piece) {.player = WHITE_PLAYER, .type = KING}, (Position) {0, 0});
	board_set_piece(board, (Piece) {.player = BLACK_PLAYER, .type = ROOK}, (Position) {3, 1});

	bool is_check = rules_is_check_after_move(match, (Move) {(Position) {0, 0}, (Position) {1, 0}});
	cr_assert_eq(is_check, false);
}

Test(rules, is_check_after_move_returns_false_when_attacker_is_captured) {
	Board *board = match_get_board(match);

	board_set_piece(board, (Piece) {.player = WHITE_PLAYER, .type = KING}, (Position) {0, 0});
	board_set_piece(board, (Piece) {.player = BLACK_PLAYER, .type = QUEEN}, (Position) {1, 0});

	bool is_check = rules_is_check_after_move(match, (Move) {(Position) {0, 0}, (Position) {1, 0}});
	cr_assert_eq(is_check, false);
}

Test(rules, is_check_after_move_returns_true_when_move_exposes_king) {
	Board *board = match_get_board(match);

	board_set_piece(board, (Piece) {.player = WHITE_PLAYER, .type = KING}, (Position) {0, 0});
	board_set_piece(board, (Piece) {.player = WHITE_PLAYER, .type = ROOK}, (Position) {1, 0});
	board_set_piece(board, (Piece) {.player = BLACK_PLAYER, .type = QUEEN}, (Position) {2, 0});

	bool is_check = rules_is_check_after_move(match, (Move) {(Position) {1, 0}, (Position) {1, 4}});
	cr_assert_eq(is_check, true);
}

Test(rules, is_check_after_move_returns_true_when_capturing_attacker_exposes_king) {
	Board *board = match_get_board(match);

	board_set_piece(board, (Piece) {.player = WHITE_PLAYER, .type = KING}, (Position) {0, 0});
	board_set_piece(board, (Piece) {.player = BLACK_PLAYER, .type = ROOK}, (Position) {1, 0});
	board_set_piece(board, (Piece) {.player = BLACK_PLAYER, .type = QUEEN}, (Position) {2, 0});

	bool is_check = rules_is_check_after_move(match, (Move) {(Position) {0, 0}, (Position) {1, 0}});
	cr_assert_eq(is_check, true);
}

Test(rules, is_checkmate_returns_true_when_king_cannot_escape) {
	Board *board = match_get_board(match);

	board_set_piece(board, (Piece) {.player = WHITE_PLAYER, .type = KING}, (Position) {0, 0});
	board_set_piece(board, (Piece) {.player = BLACK_PLAYER, .type = ROOK}, (Position) {1, 0});
	board_set_piece(board, (Piece) {.player = BLACK_PLAYER, .type = QUEEN}, (Position) {1, 1});

	bool is_checkmate = rules_is_checkmate(match, WHITE_PLAYER);
	cr_assert_eq(is_checkmate, true);
}

Test(rules, is_checkmate_returns_false_when_king_can_escape) {
	Board *board = match_get_board(match);

	board_set_piece(board, (Piece) {.player = WHITE_PLAYER, .type = KING}, (Position) {0, 0});
	board_set_piece(board, (Piece) {.player = BLACK_PLAYER, .type = ROOK}, (Position) {2, 0});
	board_set_piece(board, (Piece) {.player = BLACK_PLAYER, .type = QUEEN}, (Position) {5, 2});

	bool is_checkmate = rules_is_checkmate(match, WHITE_PLAYER);
	cr_assert_eq(is_checkmate, false);
}

Test(rules, is_promotion_returns_true_when_white_pawn_is_in_last_row) {
	Board *board = match_get_board(match);

	board_set_piece(board, (Piece) {.player = WHITE_PLAYER, .type = PAWN}, (Position) {0, 0});

	bool is_promotion = rules_is_promotion(match, (Position) {0, 0});
	cr_assert_eq(is_promotion, true);
}

Test(rules, is_promotion_returns_true_when_black_pawn_is_in_first_row) {
	Board *board = match_get_board(match);

	board_set_piece(board, (Piece) {.player = BLACK_PLAYER, .type = PAWN}, (Position) {0, 7});

	bool is_promotion = rules_is_promotion(match, (Position) {0, 7});
	cr_assert_eq(is_promotion, true);
}

Test(rules, is_promotion_returns_false_when_not_pawn) {
	Board *board = match_get_board(match);

	board_set_piece(board, (Piece) {.player = WHITE_PLAYER, .type = ROOK}, (Position) {0, 0});

	bool is_promotion = rules_is_promotion(match, (Position) {0, 0});
	cr_assert_eq(is_promotion, false);
}

Test(rules, white_can_kingside_castling) {
	Board *board = match_get_board(match);

	Piece king = (Piece) {WHITE_PLAYER, KING};
	Position king_pos = (Position) {4, 7};
	Piece rook = (Piece) {WHITE_PLAYER, ROOK};
	Position rook_pos = (Position) {7, 7};

	board_set_piece(board, king, king_pos);
	board_set_piece(board, rook, rook_pos);

	bool ks_castling = rules_can_castle_kingside(match, WHITE_PLAYER);

	cr_assert_eq(ks_castling, true);
}

Test(rules, white_kingside_castling_blocked_by_friendly) {
	Board *board = match_get_board(match);

	Piece king = (Piece) {WHITE_PLAYER, KING};
	Position king_pos = (Position) {4, 7};
	Piece rook = (Piece) {WHITE_PLAYER, ROOK};
	Position rook_pos = (Position) {7, 7};

	Piece blocking_pawn = (Piece) {WHITE_PLAYER, PAWN};
	Position blocking_pawn_pos = (Position) {6, 7};

	board_set_piece(board, king, king_pos);
	board_set_piece(board, rook, rook_pos);
	board_set_piece(board, blocking_pawn, blocking_pawn_pos);

	bool ks_castling = rules_can_castle_kingside(match, WHITE_PLAYER);

	cr_assert_eq(ks_castling, false);
}

Test(rules, white_kingside_castling_blocked_by_enemy) {
	Board *board = match_get_board(match);

	Piece king = (Piece) {WHITE_PLAYER, KING};
	Position king_pos = (Position) {4, 7};
	Piece rook = (Piece) {WHITE_PLAYER, ROOK};
	Position rook_pos = (Position) {7, 7};

	Piece blocking_enemy = (Piece) {BLACK_PLAYER, PAWN};
	Position blocking_enemy_pos = (Position) {6, 7};

	board_set_piece(board, king, king_pos);
	board_set_piece(board, rook, rook_pos);
	board_set_piece(board, blocking_enemy, blocking_enemy_pos);

	bool ks_castling = rules_can_castle_kingside(match, WHITE_PLAYER);

	cr_assert_eq(ks_castling, false);
}

Test(rules, white_kingside_castling_blocked_by_check) {
	Board *board = match_get_board(match);

	Piece king = (Piece) {WHITE_PLAYER, KING};
	Position king_pos = (Position) {4, 7};
	Piece rook = (Piece) {WHITE_PLAYER, ROOK};
	Position rook_pos = (Position) {7, 7};

	Piece blocking_enemy = (Piece) {BLACK_PLAYER, BISHOP};
	Position blocking_enemy_pos = (Position) {king_pos.x - 2, king_pos.y - 2};

	board_set_piece(board, king, king_pos);
	board_set_piece(board, rook, rook_pos);
	board_set_piece(board, blocking_enemy, blocking_enemy_pos);

	bool ks_castling = rules_can_castle_kingside(match, WHITE_PLAYER);

	cr_assert_eq(ks_castling, false);
}

Test(rules, white_kingside_castling_blocked_by_enemy_targeting_castling_tile_f1) {
	Board *board = match_get_board(match);

	Piece king = (Piece) {WHITE_PLAYER, KING};
	Position king_pos = (Position) {4, 7};
	Piece rook = (Piece) {WHITE_PLAYER, ROOK};
	Position rook_pos = (Position) {7, 7};

	Piece blocking_enemy = (Piece) {BLACK_PLAYER, BISHOP};
	Position blocking_enemy_pos = (Position) {3, 5};

	board_set_piece(board, king, king_pos);
	board_set_piece(board, rook, rook_pos);
	board_set_piece(board, blocking_enemy, blocking_enemy_pos);

	bool ks_castling = rules_can_castle_kingside(match, WHITE_PLAYER);

	cr_assert_eq(ks_castling, false);
}

Test(rules, white_kingside_castling_blocked_by_enemy_targeting_castling_tile_g1) {
	Board *board = match_get_board(match);

	Piece king = (Piece) {WHITE_PLAYER, KING};
	Position king_pos = (Position) {4, 7};
	Piece rook = (Piece) {WHITE_PLAYER, ROOK};
	Position rook_pos = (Position) {7, 7};

	Piece blocking_enemy = (Piece) {BLACK_PLAYER, BISHOP};
	Position blocking_enemy_pos = (Position) {4, 5};

	board_set_piece(board, king, king_pos);
	board_set_piece(board, rook, rook_pos);
	board_set_piece(board, blocking_enemy, blocking_enemy_pos);

	bool ks_castling = rules_can_castle_kingside(match, WHITE_PLAYER);

	cr_assert_eq(ks_castling, false);
}

Test(rules, black_can_kingside_castling) {
	Board *board = match_get_board(match);

	Piece king = (Piece) {BLACK_PLAYER, KING};
	Position king_pos = (Position) {4, 0};
	Piece rook = (Piece) {BLACK_PLAYER, ROOK};
	Position rook_pos = (Position) {7, 0};

	board_set_piece(board, king, king_pos);
	board_set_piece(board, rook, rook_pos);

	bool ks_castling = rules_can_castle_kingside(match, BLACK_PLAYER);

	cr_assert_eq(ks_castling, true);
}

Test(rules, black_kingside_castling_blocked_by_friendly) {
	Board *board = match_get_board(match);

	Piece king = (Piece) {BLACK_PLAYER, KING};
	Position king_pos = (Position) {4, 0};
	Piece rook = (Piece) {BLACK_PLAYER, ROOK};
	Position rook_pos = (Position) {7, 0};

	Piece blocking_friendly = (Piece) {BLACK_PLAYER, PAWN};
	Position blocking_friendly_pos = (Position) {6, 0};

	board_set_piece(board, king, king_pos);
	board_set_piece(board, rook, rook_pos);
	board_set_piece(board, blocking_friendly, blocking_friendly_pos);

	bool ks_castling = rules_can_castle_kingside(match, BLACK_PLAYER);

	cr_assert_eq(ks_castling, false);
}

Test(rules, black_kingside_castling_blocked_by_enemy) {
	Board *board = match_get_board(match);

	Piece king = (Piece) {BLACK_PLAYER, KING};
	Position king_pos = (Position) {4, 0};
	Piece rook = (Piece) {BLACK_PLAYER, ROOK};
	Position rook_pos = (Position) {7, 0};

	Piece blocking_pawn = (Piece) {WHITE_PLAYER, PAWN};
	Position blocking_pawn_pos = (Position) {6, 0};

	board_set_piece(board, king, king_pos);
	board_set_piece(board, rook, rook_pos);
	board_set_piece(board, blocking_pawn, blocking_pawn_pos);

	bool ks_castling = rules_can_castle_kingside(match, BLACK_PLAYER);

	cr_assert_eq(ks_castling, false);
}

Test(rules, black_kingside_castling_blocked_by_check) {
	Board *board = match_get_board(match);

	Piece king = (Piece) {BLACK_PLAYER, KING};
	Position king_pos = (Position) {4, 0};
	Piece rook = (Piece) {BLACK_PLAYER, ROOK};
	Position rook_pos = (Position) {7, 0};

	Piece blocking_enemy = (Piece) {WHITE_PLAYER, BISHOP};
	Position blocking_enemy_pos = (Position) {king_pos.x - 2, king_pos.y + 2};

	board_set_piece(board, king, king_pos);
	board_set_piece(board, rook, rook_pos);
	board_set_piece(board, blocking_enemy, blocking_enemy_pos);

	bool ks_castling = rules_can_castle_kingside(match, BLACK_PLAYER);

	cr_assert_eq(ks_castling, false);
}

Test(rules, black_kingside_castling_blocked_by_enemy_targeting_castling_tile_f8) {
	Board *board = match_get_board(match);

	Piece king = (Piece) {BLACK_PLAYER, KING};
	Position king_pos = (Position) {4, 0};
	Piece rook = (Piece) {BLACK_PLAYER, ROOK};
	Position rook_pos = (Position) {7, 0};

	Piece blocking_enemy = (Piece) {WHITE_PLAYER, BISHOP};
	Position blocking_enemy_pos = (Position) {3, 2};

	board_set_piece(board, king, king_pos);
	board_set_piece(board, rook, rook_pos);
	board_set_piece(board, blocking_enemy, blocking_enemy_pos);

	bool ks_castling = rules_can_castle_kingside(match, BLACK_PLAYER);

	cr_assert_eq(ks_castling, false);
}

Test(rules, black_kingside_castling_blocked_by_enemy_targeting_castling_tile_g8) {
	Board *board = match_get_board(match);

	Piece king = (Piece) {BLACK_PLAYER, KING};
	Position king_pos = (Position) {4, 0};
	Piece rook = (Piece) {BLACK_PLAYER, ROOK};
	Position rook_pos = (Position) {7, 0};

	Piece blocking_enemy = (Piece) {WHITE_PLAYER, BISHOP};
	Position blocking_enemy_pos = (Position) {4, 2};

	board_set_piece(board, king, king_pos);
	board_set_piece(board, rook, rook_pos);
	board_set_piece(board, blocking_enemy, blocking_enemy_pos);

	bool ks_castling = rules_can_castle_kingside(match, BLACK_PLAYER);

	cr_assert_eq(ks_castling, false);
}

Test(rules, white_is_castling_kingside) {
	Board *board = match_get_board(match);

	Piece king = (Piece) {WHITE_PLAYER, KING};
	Position king_pos = (Position) {4, 7};
	Piece rook = (Piece) {WHITE_PLAYER, ROOK};
	Position rook_pos = (Position) {7, 7};

	board_set_piece(board, king, king_pos);
	board_set_piece(board, rook, rook_pos);

	Move castling_move = (Move) {king_pos, rook_pos};
	bool ks_castling = rules_is_castling(match, castling_move);

	cr_assert_eq(ks_castling, true);
}

Test(rules, white_is_castling_queenside) {
	Board *board = match_get_board(match);

	Piece king = (Piece) {WHITE_PLAYER, KING};
	Position king_pos = (Position) {4, 7};
	Piece rook = (Piece) {WHITE_PLAYER, ROOK};
	Position rook_pos = (Position) {0, 7};

	board_set_piece(board, king, king_pos);
	board_set_piece(board, rook, rook_pos);

	Move castling_move = (Move) {king_pos, rook_pos};
	bool qs_castling = rules_is_castling(match, castling_move);

	cr_assert_eq(qs_castling, true);
}

Test(rules, black_is_castling_kingside) {
	Board *board = match_get_board(match);

	Piece king = (Piece) {BLACK_PLAYER, KING};
	Position king_pos = (Position) {4, 0};
	Piece rook = (Piece) {BLACK_PLAYER, ROOK};
	Position rook_pos = (Position) {7, 0};

	board_set_piece(board, king, king_pos);
	board_set_piece(board, rook, rook_pos);

	Move castling_move = (Move) {king_pos, rook_pos};
	bool ks_castling = rules_is_castling(match, castling_move);

	cr_assert_eq(ks_castling, true);
}

Test(rules, black_is_castling_queenside) {
	Board *board = match_get_board(match);

	Piece king = (Piece) {BLACK_PLAYER, KING};
	Position king_pos = (Position) {4, 0};
	Piece rook = (Piece) {BLACK_PLAYER, ROOK};
	Position rook_pos = (Position) {0, 0};

	board_set_piece(board, king, king_pos);
	board_set_piece(board, rook, rook_pos);

	Move castling_move = (Move) {king_pos, rook_pos};
	bool qs_castling = rules_is_castling(match, castling_move);

	cr_assert_eq(qs_castling, true);
}
