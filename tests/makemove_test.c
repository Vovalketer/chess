#include "../src/engine/makemove.h"

#include "../external/unity/unity.h"
#include "../src/engine/board.h"
#include "../src/engine/utils.h"

Board *board = NULL;

void setUp(void) {
	board = board_create();
}

void tearDown(void) {
	board_destroy(&board);
}

void test_quiet_move_removes_from_original_square_and_sets_at_new_square(void) {
	Piece  pawn = (Piece) {.player = PLAYER_W, .type = PAWN};
	Square from = SQ_B2;
	Square to	= SQ_B3;
	Move   move = (Move) {.piece = pawn, .from = from, .to = to, .mv_type = MV_QUIET};
	board_set_piece(board, pawn, from);
	TEST_ASSERT_TRUE(board_get_piece(board, from).type == pawn.type);
	TEST_ASSERT_TRUE(board_get_piece(board, to).type == EMPTY);

	TEST_ASSERT_TRUE(make_move(board, move));

	TEST_ASSERT_TRUE(board_get_piece(board, from).type == EMPTY);
	TEST_ASSERT_TRUE(board_get_piece(board, to).type == pawn.type);
}

void test_w_pawn_double_push_sets_ep_target(void) {
	Piece  pawn = (Piece) {.player = PLAYER_W, .type = PAWN};
	Square from = SQ_B2;
	Square to	= SQ_B4;
	Move   move = (Move) {.piece = pawn, .from = from, .to = to, .mv_type = MV_PAWN_DOUBLE};
	board_set_piece(board, pawn, from);
	TEST_ASSERT_TRUE(board->ep_target == SQ_NONE);
	TEST_ASSERT_TRUE(make_move(board, move));
	TEST_ASSERT_EQUAL(SQ_B3, board->ep_target);
}

void test_b_pawn_double_push_sets_ep_target(void) {
	board->side = PLAYER_B;
	Piece  pawn = (Piece) {.player = PLAYER_B, .type = PAWN};
	Square from = SQ_B7;
	Square to	= SQ_B5;
	Move   move = (Move) {.piece = pawn, .from = from, .to = to, .mv_type = MV_PAWN_DOUBLE};
	board_set_piece(board, pawn, from);
	TEST_ASSERT_TRUE(board->ep_target == SQ_NONE);
	TEST_ASSERT_TRUE(make_move(board, move));
	TEST_ASSERT_EQUAL(SQ_B6, board->ep_target);
}

void test_non_double_pawn_pushes_reset_ep_target(void) {
	board->ep_target = SQ_H3;
	Piece  pawn		 = (Piece) {.player = PLAYER_W, .type = PAWN};
	Square from		 = SQ_B2;
	Square to		 = SQ_B3;
	Move   move		 = (Move) {.piece = pawn, .from = from, .to = to, .mv_type = MV_QUIET};
	board_set_piece(board, pawn, from);
	TEST_ASSERT_TRUE(board->ep_target == SQ_H3);
	TEST_ASSERT_TRUE(make_move(board, move));
	TEST_ASSERT_EQUAL(SQ_NONE, board->ep_target);
}

void test_ks_w_rook_move_removes_ks_castling_rights(void) {
	Piece  rook = (Piece) {.player = PLAYER_W, .type = ROOK};
	Square from = SQ_H1;
	Square to	= SQ_F1;
	Move   move = (Move) {.piece = rook, .from = from, .to = to, .mv_type = MV_QUIET};
	board_set_piece(board, rook, from);
	TEST_ASSERT_TRUE(board_has_castling_rights(board, CASTLING_WHITE_KS));

	TEST_ASSERT_TRUE(make_move(board, move));

	TEST_ASSERT_FALSE(board_has_castling_rights(board, CASTLING_WHITE_KS));
}

void test_qs_w_rook_move_removes_qs_castling_rights(void) {
	Piece  rook = (Piece) {.player = PLAYER_W, .type = ROOK};
	Square from = SQ_A1;
	Square to	= SQ_D1;
	Move   move = (Move) {.piece = rook, .from = from, .to = to, .mv_type = MV_QUIET};
	board_set_piece(board, rook, from);
	TEST_ASSERT_TRUE(board_has_castling_rights(board, CASTLING_WHITE_QS));

	TEST_ASSERT_TRUE(make_move(board, move));

	TEST_ASSERT_FALSE(board_has_castling_rights(board, CASTLING_WHITE_QS));
}

void test_ks_b_rook_move_removes_ks_castling_rights(void) {
	board->side = PLAYER_B;
	Piece  rook = (Piece) {.player = PLAYER_B, .type = ROOK};
	Square from = SQ_H8;
	Square to	= SQ_F8;
	Move   move = (Move) {.piece = rook, .from = from, .to = to, .mv_type = MV_QUIET};
	board_set_piece(board, rook, from);
	TEST_ASSERT_TRUE(board_has_castling_rights(board, CASTLING_BLACK_KS));

	TEST_ASSERT_TRUE(make_move(board, move));

	TEST_ASSERT_FALSE(board_has_castling_rights(board, CASTLING_BLACK_KS));
}

void test_qs_b_rook_move_removes_qs_castling_rights(void) {
	board->side = PLAYER_B;
	Piece  rook = (Piece) {.player = PLAYER_B, .type = ROOK};
	Square from = SQ_A8;
	Square to	= SQ_D8;
	Move   move = (Move) {.piece = rook, .from = from, .to = to, .mv_type = MV_QUIET};
	board_set_piece(board, rook, from);
	TEST_ASSERT_TRUE(board_has_castling_rights(board, CASTLING_BLACK_QS));

	TEST_ASSERT_TRUE(make_move(board, move));

	TEST_ASSERT_FALSE(board_has_castling_rights(board, CASTLING_BLACK_QS));
}

void test_w_king_move_removes_w_castling_rights(void) {
	board->side = PLAYER_W;
	Piece  king = (Piece) {.player = PLAYER_W, .type = KING};
	Square from = SQ_E1;
	Square to	= SQ_E2;
	Move   move = (Move) {.piece = king, .from = from, .to = to, .mv_type = MV_QUIET};
	board_set_piece(board, king, from);
	TEST_ASSERT_TRUE(board_has_castling_rights(board, CASTLING_WHITE_KS));
	TEST_ASSERT_TRUE(board_has_castling_rights(board, CASTLING_WHITE_QS));

	TEST_ASSERT_TRUE(make_move(board, move));

	TEST_ASSERT_FALSE(board_has_castling_rights(board, CASTLING_WHITE_KS));
	TEST_ASSERT_FALSE(board_has_castling_rights(board, CASTLING_WHITE_QS));
}

void test_b_king_move_removes_b_castling_rights(void) {
	board->side = PLAYER_B;
	Piece  king = (Piece) {.player = PLAYER_B, .type = KING};
	Square from = SQ_E8;
	Square to	= SQ_E7;
	Move   move = (Move) {.piece = king, .from = from, .to = to, .mv_type = MV_QUIET};
	board_set_piece(board, king, from);
	TEST_ASSERT_TRUE(board_has_castling_rights(board, CASTLING_BLACK_KS));
	TEST_ASSERT_TRUE(board_has_castling_rights(board, CASTLING_BLACK_QS));

	TEST_ASSERT_TRUE(make_move(board, move));

	TEST_ASSERT_FALSE(board_has_castling_rights(board, CASTLING_BLACK_KS));
	TEST_ASSERT_FALSE(board_has_castling_rights(board, CASTLING_BLACK_QS));
}

void test_w_ks_castling_moves_rook_and_king(void) {
	board->side		 = PLAYER_W;
	Piece  king		 = (Piece) {.player = PLAYER_W, .type = KING};
	Piece  rook		 = (Piece) {.player = PLAYER_W, .type = ROOK};
	Square rook_from = SQ_H1;
	Square rook_to	 = SQ_F1;
	Square from		 = SQ_E1;
	Square to		 = SQ_G1;
	Move   move		 = (Move) {.piece = king, .from = from, .to = to, .mv_type = MV_KS_CASTLE};
	board_set_piece(board, king, from);
	board_set_piece(board, rook, rook_from);

	TEST_ASSERT_TRUE(make_move(board, move));

	TEST_ASSERT_TRUE(board_get_piece(board, rook_from).type == EMPTY);
	TEST_ASSERT_TRUE(board_get_piece(board, from).type == EMPTY);

	TEST_ASSERT_TRUE(board_get_piece(board, to).type == king.type);
	TEST_ASSERT_TRUE(board_get_piece(board, rook_to).type == rook.type);
}

void test_b_ks_castling_moves_rook_and_king(void) {
	board->side		 = PLAYER_B;
	Piece  king		 = (Piece) {.player = PLAYER_B, .type = KING};
	Piece  rook		 = (Piece) {.player = PLAYER_B, .type = ROOK};
	Square rook_from = SQ_H8;
	Square rook_to	 = SQ_F8;
	Square from		 = SQ_E8;
	Square to		 = SQ_G8;
	Move   move		 = (Move) {.piece = king, .from = from, .to = to, .mv_type = MV_KS_CASTLE};
	board_set_piece(board, king, from);
	board_set_piece(board, rook, rook_from);

	TEST_ASSERT_TRUE(make_move(board, move));

	TEST_ASSERT_TRUE(board_get_piece(board, rook_from).type == EMPTY);
	TEST_ASSERT_TRUE(board_get_piece(board, from).type == EMPTY);

	TEST_ASSERT_TRUE(board_get_piece(board, to).type == king.type);
	TEST_ASSERT_TRUE(board_get_piece(board, rook_to).type == rook.type);
}

void test_w_qs_castling_moves_rook_and_king(void) {
	board->side		 = PLAYER_W;
	Piece  king		 = (Piece) {.player = PLAYER_W, .type = KING};
	Piece  rook		 = (Piece) {.player = PLAYER_W, .type = ROOK};
	Square rook_from = SQ_A1;
	Square rook_to	 = SQ_D1;
	Square from		 = SQ_E1;
	Square to		 = SQ_C1;
	Move   move		 = (Move) {.piece = king, .from = from, .to = to, .mv_type = MV_QS_CASTLE};
	board_set_piece(board, king, from);
	board_set_piece(board, rook, rook_from);

	TEST_ASSERT_TRUE(make_move(board, move));

	TEST_ASSERT_TRUE(board_get_piece(board, rook_from).type == EMPTY);
	TEST_ASSERT_TRUE(board_get_piece(board, from).type == EMPTY);

	TEST_ASSERT_TRUE(board_get_piece(board, to).type == king.type);
	TEST_ASSERT_TRUE(board_get_piece(board, rook_to).type == rook.type);
}

void test_b_qs_castling_moves_rook_and_king(void) {
	board->side		 = PLAYER_B;
	Piece  king		 = (Piece) {.player = PLAYER_B, .type = KING};
	Piece  rook		 = (Piece) {.player = PLAYER_B, .type = ROOK};
	Square rook_from = SQ_A8;
	Square rook_to	 = SQ_D8;
	Square from		 = SQ_E8;
	Square to		 = SQ_C8;
	Move   move		 = (Move) {.piece = king, .from = from, .to = to, .mv_type = MV_QS_CASTLE};
	board_set_piece(board, king, from);
	board_set_piece(board, rook, rook_from);

	TEST_ASSERT_TRUE(make_move(board, move));

	TEST_ASSERT_TRUE(board_get_piece(board, rook_from).type == EMPTY);
	TEST_ASSERT_TRUE(board_get_piece(board, from).type == EMPTY);

	TEST_ASSERT_TRUE(board_get_piece(board, to).type == king.type);
	TEST_ASSERT_TRUE(board_get_piece(board, rook_to).type == rook.type);
}

void test_w_ks_castling_cannot_be_done_if_king_is_in_check(void) {
	board->side		 = PLAYER_W;
	Piece  king		 = (Piece) {.player = PLAYER_W, .type = KING};
	Piece  rook		 = (Piece) {.player = PLAYER_W, .type = ROOK};
	Square rook_from = SQ_H1;
	Square rook_to	 = SQ_F1;
	Square from		 = SQ_E1;
	Square to		 = SQ_G1;
	Move   move		 = (Move) {.piece = king, .from = from, .to = to, .mv_type = MV_KS_CASTLE};
	board_set_piece(board, king, from);
	board_set_piece(board, rook, rook_from);

	Piece  threat	   = (Piece) {.player = PLAYER_B, .type = ROOK};
	Square threat_from = SQ_E3;
	board_set_piece(board, threat, threat_from);

	TEST_ASSERT_FALSE(make_move(board, move));

	TEST_ASSERT_TRUE(board_get_piece(board, rook_from).type == rook.type);
	TEST_ASSERT_TRUE(board_get_piece(board, from).type == king.type);

	TEST_ASSERT_TRUE(board_get_piece(board, to).type == EMPTY);
	TEST_ASSERT_TRUE(board_get_piece(board, rook_to).type == EMPTY);
}

void test_b_ks_castling_cannot_be_done_if_king_is_in_check(void) {
	board->side		 = PLAYER_B;
	Piece  king		 = (Piece) {.player = PLAYER_B, .type = KING};
	Piece  rook		 = (Piece) {.player = PLAYER_B, .type = ROOK};
	Square rook_from = SQ_H8;
	Square rook_to	 = SQ_F8;
	Square from		 = SQ_E8;
	Square to		 = SQ_G8;
	Move   move		 = (Move) {.piece = king, .from = from, .to = to, .mv_type = MV_KS_CASTLE};
	board_set_piece(board, king, from);
	board_set_piece(board, rook, rook_from);

	Piece  threat	   = (Piece) {.player = PLAYER_W, .type = ROOK};
	Square threat_from = SQ_E6;
	board_set_piece(board, threat, threat_from);

	TEST_ASSERT_FALSE(make_move(board, move));

	TEST_ASSERT_TRUE(board_get_piece(board, rook_from).type == rook.type);
	TEST_ASSERT_TRUE(board_get_piece(board, from).type == king.type);

	TEST_ASSERT_TRUE(board_get_piece(board, to).type == EMPTY);
	TEST_ASSERT_TRUE(board_get_piece(board, rook_to).type == EMPTY);
}

void test_w_qs_castling_cannot_be_done_if_king_is_in_check(void) {
	board->side		 = PLAYER_W;
	Piece  king		 = (Piece) {.player = PLAYER_W, .type = KING};
	Piece  rook		 = (Piece) {.player = PLAYER_W, .type = ROOK};
	Square rook_from = SQ_A1;
	Square rook_to	 = SQ_D1;
	Square from		 = SQ_E1;
	Square to		 = SQ_C1;
	Move   move		 = (Move) {.piece = king, .from = from, .to = to, .mv_type = MV_QS_CASTLE};
	board_set_piece(board, king, from);
	board_set_piece(board, rook, rook_from);

	Piece  threat	   = (Piece) {.player = PLAYER_B, .type = ROOK};
	Square threat_from = SQ_E3;
	board_set_piece(board, threat, threat_from);

	TEST_ASSERT_FALSE(make_move(board, move));

	TEST_ASSERT_TRUE(board_get_piece(board, rook_from).type == rook.type);
	TEST_ASSERT_TRUE(board_get_piece(board, from).type == king.type);

	TEST_ASSERT_TRUE(board_get_piece(board, to).type == EMPTY);
	TEST_ASSERT_TRUE(board_get_piece(board, rook_to).type == EMPTY);
}

void test_b_qs_castling_cannot_be_done_if_king_is_in_check(void) {
	board->side		 = PLAYER_B;
	Piece  king		 = (Piece) {.player = PLAYER_B, .type = KING};
	Piece  rook		 = (Piece) {.player = PLAYER_B, .type = ROOK};
	Square rook_from = SQ_A8;
	Square rook_to	 = SQ_D8;
	Square from		 = SQ_E8;
	Square to		 = SQ_C8;
	Move   move		 = (Move) {.piece = king, .from = from, .to = to, .mv_type = MV_QS_CASTLE};
	board_set_piece(board, king, from);
	board_set_piece(board, rook, rook_from);

	Piece  threat	   = (Piece) {.player = PLAYER_W, .type = ROOK};
	Square threat_from = SQ_E6;
	board_set_piece(board, threat, threat_from);

	TEST_ASSERT_FALSE(make_move(board, move));

	TEST_ASSERT_TRUE(board_get_piece(board, rook_from).type == rook.type);
	TEST_ASSERT_TRUE(board_get_piece(board, from).type == king.type);

	TEST_ASSERT_TRUE(board_get_piece(board, to).type == EMPTY);
	TEST_ASSERT_TRUE(board_get_piece(board, rook_to).type == EMPTY);
}

void test_w_ks_castling_cannot_be_done_if_path_is_blocked(Square blocker_sqr) {
	board->side		 = PLAYER_W;
	Piece  king		 = (Piece) {.player = PLAYER_W, .type = KING};
	Piece  rook		 = (Piece) {.player = PLAYER_W, .type = ROOK};
	Square rook_from = SQ_H1;
	Square from		 = SQ_E1;
	Square to		 = SQ_G1;
	Move   move		 = (Move) {.piece = king, .from = from, .to = to, .mv_type = MV_KS_CASTLE};
	board_set_piece(board, king, from);
	board_set_piece(board, rook, rook_from);

	Piece blocker = (Piece) {.player = PLAYER_W, .type = PAWN};
	board_set_piece(board, blocker, blocker_sqr);

	TEST_ASSERT_FALSE(make_move(board, move));

	TEST_ASSERT_TRUE_MESSAGE(board_get_piece(board, rook_from).type == rook.type,
							 utils_square_contents(board, rook_from).str);
	TEST_ASSERT_TRUE_MESSAGE(board_get_piece(board, from).type == king.type,
							 utils_square_contents(board, from).str);

	TEST_ASSERT_TRUE_MESSAGE(board_get_piece(board, to).type != king.type,
							 utils_square_contents(board, to).str);
	TEST_ASSERT_TRUE_MESSAGE(board_get_piece(board, blocker_sqr).type == blocker.type,
							 utils_square_contents(board, blocker_sqr).str);
}

void test_w_ks_castling_cannot_be_done_if_path_is_blocked_parametric(void) {
	test_w_ks_castling_cannot_be_done_if_path_is_blocked(SQ_F1);
	test_w_ks_castling_cannot_be_done_if_path_is_blocked(SQ_G1);
}

void test_b_ks_castling_cannot_be_done_if_path_is_blocked(Square blocker_sqr) {
	board->side		 = PLAYER_B;
	Piece  king		 = (Piece) {.player = PLAYER_B, .type = KING};
	Piece  rook		 = (Piece) {.player = PLAYER_B, .type = ROOK};
	Square rook_from = SQ_H8;
	Square from		 = SQ_E8;
	Square to		 = SQ_G8;
	Move   move		 = (Move) {.piece = king, .from = from, .to = to, .mv_type = MV_KS_CASTLE};
	board_set_piece(board, king, from);
	board_set_piece(board, rook, rook_from);

	Piece blocker = (Piece) {.player = PLAYER_B, .type = PAWN};
	board_set_piece(board, blocker, blocker_sqr);

	TEST_ASSERT_FALSE(make_move(board, move));

	TEST_ASSERT_TRUE_MESSAGE(board_get_piece(board, rook_from).type == rook.type,
							 utils_square_contents(board, rook_from).str);
	TEST_ASSERT_TRUE_MESSAGE(board_get_piece(board, from).type == king.type,
							 utils_square_contents(board, from).str);

	TEST_ASSERT_TRUE_MESSAGE(board_get_piece(board, to).type != king.type,
							 utils_square_contents(board, to).str);
	TEST_ASSERT_TRUE_MESSAGE(board_get_piece(board, blocker_sqr).type == blocker.type,
							 utils_square_contents(board, blocker_sqr).str);
}

void test_b_ks_castling_cannot_be_done_if_path_is_blocked_parametric(void) {
	test_b_ks_castling_cannot_be_done_if_path_is_blocked(SQ_F8);
	test_b_ks_castling_cannot_be_done_if_path_is_blocked(SQ_G8);
}

void test_w_qs_castling_cannot_be_done_if_path_is_blocked(Square blocker_sqr) {
	board->side		 = PLAYER_W;
	Piece  king		 = (Piece) {.player = PLAYER_W, .type = KING};
	Piece  rook		 = (Piece) {.player = PLAYER_W, .type = ROOK};
	Square rook_from = SQ_A1;
	Square from		 = SQ_E1;
	Square to		 = SQ_C1;
	Move   move		 = (Move) {.piece = king, .from = from, .to = to, .mv_type = MV_QS_CASTLE};
	board_set_piece(board, king, from);
	board_set_piece(board, rook, rook_from);

	Piece blocker = (Piece) {.player = PLAYER_W, .type = PAWN};
	board_set_piece(board, blocker, blocker_sqr);

	TEST_ASSERT_FALSE(make_move(board, move));

	TEST_ASSERT_TRUE_MESSAGE(board_get_piece(board, rook_from).type == rook.type,
							 utils_square_contents(board, rook_from).str);
	TEST_ASSERT_TRUE_MESSAGE(board_get_piece(board, from).type == king.type,
							 utils_square_contents(board, from).str);

	TEST_ASSERT_TRUE_MESSAGE(board_get_piece(board, to).type != king.type,
							 utils_square_contents(board, to).str);
	TEST_ASSERT_TRUE_MESSAGE(board_get_piece(board, blocker_sqr).type == blocker.type,
							 utils_square_contents(board, blocker_sqr).str);
}

void test_w_qs_castling_cannot_be_done_if_path_is_blocked_parametric(void) {
	test_w_qs_castling_cannot_be_done_if_path_is_blocked(SQ_D1);
	test_w_qs_castling_cannot_be_done_if_path_is_blocked(SQ_C1);
	test_w_qs_castling_cannot_be_done_if_path_is_blocked(SQ_B1);
}

void test_b_qs_castling_cannot_be_done_if_path_is_blocked(Square blocker_sqr) {
	board->side		 = PLAYER_B;
	Piece  king		 = (Piece) {.player = PLAYER_B, .type = KING};
	Piece  rook		 = (Piece) {.player = PLAYER_B, .type = ROOK};
	Square rook_from = SQ_A8;
	// Square rook_to	 = SQ_D8;
	Square from = SQ_E8;
	Square to	= SQ_C8;
	Move   move = (Move) {.piece = king, .from = from, .to = to, .mv_type = MV_QS_CASTLE};
	board_set_piece(board, king, from);
	board_set_piece(board, rook, rook_from);

	Piece blocker = (Piece) {.player = PLAYER_B, .type = PAWN};
	board_set_piece(board, blocker, blocker_sqr);

	TEST_ASSERT_FALSE(make_move(board, move));

	TEST_ASSERT_TRUE_MESSAGE(board_get_piece(board, rook_from).type == rook.type,
							 utils_square_contents(board, rook_from).str);
	TEST_ASSERT_TRUE_MESSAGE(board_get_piece(board, from).type == king.type,
							 utils_square_contents(board, from).str);

	TEST_ASSERT_TRUE_MESSAGE(board_get_piece(board, to).type != king.type,
							 utils_square_contents(board, to).str);
	TEST_ASSERT_TRUE_MESSAGE(board_get_piece(board, blocker_sqr).type == blocker.type,
							 utils_square_contents(board, blocker_sqr).str);
}

void test_b_qs_castling_cannot_be_done_if_path_is_blocked_parametric(void) {
	test_b_qs_castling_cannot_be_done_if_path_is_blocked(SQ_D8);
	test_b_qs_castling_cannot_be_done_if_path_is_blocked(SQ_C8);
	test_b_qs_castling_cannot_be_done_if_path_is_blocked(SQ_B8);
}

void test_ks_castling_cannot_be_done_if_squares_are_threatened(Square threatened_sqr, Player p) {
	board->side		 = p;
	Piece  king		 = (Piece) {.player = p, .type = KING};
	Square from		 = p == PLAYER_W ? SQ_E1 : SQ_E8;
	Square to		 = p == PLAYER_W ? SQ_G1 : SQ_G8;
	Piece  rook		 = (Piece) {.player = p, .type = ROOK};
	Square rook_from = p == PLAYER_W ? SQ_H1 : SQ_H8;
	Square rook_to	 = p == PLAYER_W ? SQ_F1 : SQ_F8;
	board_set_piece(board, king, from);
	board_set_piece(board, rook, rook_from);

	Piece	  threat	  = (Piece) {.player = utils_get_opponent(p), .type = ROOK};
	Direction offset	  = p == PLAYER_W ? DIR_N : DIR_S;
	Square	  threat_from = threatened_sqr + offset;
	board_set_piece(board, threat, threat_from);

	Move move = (Move) {.piece = king, .from = from, .to = to, .mv_type = MV_KS_CASTLE};
	TEST_ASSERT_FALSE(make_move(board, move));

	TEST_ASSERT_TRUE_MESSAGE(board_get_piece(board, rook_from).type == rook.type,
							 utils_square_contents(board, rook_from).str);
	TEST_ASSERT_TRUE_MESSAGE(board_get_piece(board, from).type == king.type,
							 utils_square_contents(board, from).str);

	TEST_ASSERT_TRUE_MESSAGE(board_get_piece(board, to).type == EMPTY,
							 utils_square_contents(board, to).str);
	TEST_ASSERT_TRUE_MESSAGE(board_get_piece(board, rook_to).type == EMPTY,
							 utils_square_contents(board, rook_to).str);
}

void test_ks_castling_cannot_be_done_if_squares_are_threatened_parametric(void) {
	test_ks_castling_cannot_be_done_if_squares_are_threatened(SQ_F1, PLAYER_W);
	test_ks_castling_cannot_be_done_if_squares_are_threatened(SQ_G1, PLAYER_W);
	test_ks_castling_cannot_be_done_if_squares_are_threatened(SQ_F8, PLAYER_B);
	test_ks_castling_cannot_be_done_if_squares_are_threatened(SQ_G8, PLAYER_B);
}

void test_qs_castling_cannot_be_done_if_squares_are_threatened(Square threatened_sqr, Player p) {
	board->side		 = p;
	Piece  king		 = (Piece) {.player = p, .type = KING};
	Square from		 = p == PLAYER_W ? SQ_E1 : SQ_E8;
	Square to		 = p == PLAYER_W ? SQ_C1 : SQ_C8;
	Piece  rook		 = (Piece) {.player = p, .type = ROOK};
	Square rook_from = p == PLAYER_W ? SQ_A1 : SQ_A8;
	Square rook_to	 = p == PLAYER_W ? SQ_D1 : SQ_D8;
	board_set_piece(board, king, from);
	board_set_piece(board, rook, rook_from);

	Piece	  threat	  = (Piece) {.player = utils_get_opponent(p), .type = ROOK};
	Direction offset	  = p == PLAYER_W ? DIR_N : DIR_S;
	Square	  threat_from = threatened_sqr + offset;
	board_set_piece(board, threat, threat_from);

	Move move = (Move) {.piece = king, .from = from, .to = to, .mv_type = MV_QS_CASTLE};
	TEST_ASSERT_FALSE(make_move(board, move));

	TEST_ASSERT_TRUE_MESSAGE(board_get_piece(board, rook_from).type == rook.type,
							 utils_square_contents(board, rook_from).str);
	TEST_ASSERT_TRUE_MESSAGE(board_get_piece(board, from).type == king.type,
							 utils_square_contents(board, from).str);

	TEST_ASSERT_TRUE_MESSAGE(board_get_piece(board, to).type == EMPTY,
							 utils_square_contents(board, to).str);
	TEST_ASSERT_TRUE_MESSAGE(board_get_piece(board, rook_to).type == EMPTY,
							 utils_square_contents(board, rook_to).str);
}

void test_qs_castling_cannot_be_done_if_squares_are_threatened_parametric(void) {
	test_qs_castling_cannot_be_done_if_squares_are_threatened(SQ_C1, PLAYER_W);
	test_qs_castling_cannot_be_done_if_squares_are_threatened(SQ_D1, PLAYER_W);
	test_qs_castling_cannot_be_done_if_squares_are_threatened(SQ_C8, PLAYER_B);
	test_qs_castling_cannot_be_done_if_squares_are_threatened(SQ_D8, PLAYER_B);
}

void test_capture_move_removes_from_original_square_and_sets_at_new_square(void) {
	Piece  pawn		= (Piece) {.player = PLAYER_W, .type = PAWN};
	Piece  captured = (Piece) {.player = PLAYER_B, .type = ROOK};
	Square from		= SQ_B2;
	Square to		= SQ_C3;
	board_set_piece(board, pawn, from);
	board_set_piece(board, captured, to);

	Move move = (Move) {.piece = pawn, .from = from, .to = to, .mv_type = MV_CAPTURE};
	TEST_ASSERT_TRUE(make_move(board, move));

	TEST_ASSERT_TRUE(board_get_piece(board, from).type == EMPTY);
	TEST_ASSERT_TRUE(board_get_piece(board, to).type == pawn.type);
}

void test_w_en_passant_move_removes_opponent_pawn_and_sets_ep_target(void) {
	board->side			= PLAYER_W;
	Piece  pawn			= (Piece) {.player = PLAYER_W, .type = PAWN};
	Piece  captured		= (Piece) {.player = PLAYER_B, .type = PAWN};
	Square from			= SQ_B5;
	Square to			= SQ_C6;
	board->ep_target	= to;
	Square captured_sqr = SQ_C5;
	board_set_piece(board, pawn, from);
	board_set_piece(board, captured, captured_sqr);

	Move move = (Move) {.piece = pawn, .from = from, .to = to, .mv_type = MV_EN_PASSANT};
	TEST_ASSERT_TRUE(make_move(board, move));

	TEST_ASSERT_TRUE(board_get_piece(board, from).type == EMPTY);
	TEST_ASSERT_TRUE(board_get_piece(board, to).type == pawn.type &&
					 board_get_piece(board, to).player == pawn.player);
	TEST_ASSERT_TRUE(board_get_piece(board, captured_sqr).type == EMPTY);
	TEST_ASSERT_TRUE(board->ep_target == SQ_NONE);
}

void test_b_en_passant_move_removes_opponent_pawn_and_sets_ep_target(void) {
	board->side			= PLAYER_B;
	Piece  pawn			= (Piece) {.player = PLAYER_B, .type = PAWN};
	Piece  captured		= (Piece) {.player = PLAYER_W, .type = PAWN};
	Square from			= SQ_B4;
	Square to			= SQ_C3;
	board->ep_target	= to;
	Square captured_sqr = SQ_C4;
	board_set_piece(board, pawn, from);
	board_set_piece(board, captured, captured_sqr);

	Move move = (Move) {.piece = pawn, .from = from, .to = to, .mv_type = MV_EN_PASSANT};
	TEST_ASSERT_TRUE(make_move(board, move));

	TEST_ASSERT_TRUE(board_get_piece(board, from).type == EMPTY);
	TEST_ASSERT_TRUE(board_get_piece(board, to).player == pawn.player &&
					 board_get_piece(board, to).type == pawn.type);
	TEST_ASSERT_TRUE(board_get_piece(board, captured_sqr).type == EMPTY);
	TEST_ASSERT_TRUE(board->ep_target == SQ_NONE);
}

int main(void) {
	UNITY_BEGIN();
	RUN_TEST(test_quiet_move_removes_from_original_square_and_sets_at_new_square);
	RUN_TEST(test_w_pawn_double_push_sets_ep_target);
	RUN_TEST(test_b_pawn_double_push_sets_ep_target);
	RUN_TEST(test_non_double_pawn_pushes_reset_ep_target);
	RUN_TEST(test_ks_w_rook_move_removes_ks_castling_rights);
	RUN_TEST(test_qs_w_rook_move_removes_qs_castling_rights);
	RUN_TEST(test_ks_b_rook_move_removes_ks_castling_rights);
	RUN_TEST(test_qs_b_rook_move_removes_qs_castling_rights);
	RUN_TEST(test_w_king_move_removes_w_castling_rights);
	RUN_TEST(test_b_king_move_removes_b_castling_rights);
	RUN_TEST(test_w_ks_castling_moves_rook_and_king);
	RUN_TEST(test_b_ks_castling_moves_rook_and_king);
	RUN_TEST(test_w_qs_castling_moves_rook_and_king);
	RUN_TEST(test_b_qs_castling_moves_rook_and_king);
	RUN_TEST(test_w_ks_castling_cannot_be_done_if_king_is_in_check);
	RUN_TEST(test_b_ks_castling_cannot_be_done_if_king_is_in_check);
	RUN_TEST(test_w_qs_castling_cannot_be_done_if_king_is_in_check);
	RUN_TEST(test_b_qs_castling_cannot_be_done_if_king_is_in_check);
	RUN_TEST(test_w_ks_castling_cannot_be_done_if_path_is_blocked_parametric);
	RUN_TEST(test_b_ks_castling_cannot_be_done_if_path_is_blocked_parametric);
	RUN_TEST(test_w_qs_castling_cannot_be_done_if_path_is_blocked_parametric);
	RUN_TEST(test_b_qs_castling_cannot_be_done_if_path_is_blocked_parametric);
	RUN_TEST(test_ks_castling_cannot_be_done_if_squares_are_threatened_parametric);
	RUN_TEST(test_qs_castling_cannot_be_done_if_squares_are_threatened_parametric);
	RUN_TEST(test_capture_move_removes_from_original_square_and_sets_at_new_square);
	RUN_TEST(test_w_en_passant_move_removes_opponent_pawn_and_sets_ep_target);
	RUN_TEST(test_b_en_passant_move_removes_opponent_pawn_and_sets_ep_target);

	return UNITY_END();
}
