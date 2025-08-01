#include "../src/movegen.h"

#include <stddef.h>
#include <stdio.h>

#include "../external/unity/unity.h"
#include "../src/bitboards.h"
#include "../src/board.h"
#include "../src/movelist.h"
#include "../src/utils.h"
#include "log.h"

Board *board = NULL;
char   err_msg[128];

void setUp(void) {
	board			 = board_create();
	board->ep_target = SQ_NONE;
	bitboards_init();
	log_set_level(LOG_INFO);
}

void tearDown(void) {
	board_destroy(&board);
}

void test_white_pawns_have_two_moves_at_starting_row(void) {
	for (Square sqr = SQ_A2; sqr <= SQ_H2; sqr++) {
		Piece pawn = (Piece) {.player = PLAYER_W, .type = PAWN};
		board_set_piece(board, pawn, sqr);
	}

	MoveList *ml = movegen_generate(board, PLAYER_W);
	TEST_ASSERT_EQUAL_size_t(16, move_list_size(ml));

	TEST_ASSERT_TRUE(move_list_contains(
		ml, (Move) {.from = SQ_A2, .to = SQ_A3, .mv_type = MV_QUIET, .piece = PAWN}));
	TEST_ASSERT_TRUE(move_list_contains(
		ml, (Move) {.from = SQ_A2, .to = SQ_A4, .mv_type = MV_PAWN_DOUBLE, .piece = PAWN}));
	TEST_ASSERT_TRUE(move_list_contains(
		ml, (Move) {.from = SQ_B2, .to = SQ_B3, .mv_type = MV_QUIET, .piece = PAWN}));
	TEST_ASSERT_TRUE(move_list_contains(
		ml, (Move) {.from = SQ_B2, .to = SQ_B4, .mv_type = MV_PAWN_DOUBLE, .piece = PAWN}));
	TEST_ASSERT_TRUE(move_list_contains(
		ml, (Move) {.from = SQ_C2, .to = SQ_C3, .mv_type = MV_QUIET, .piece = PAWN}));
	TEST_ASSERT_TRUE(move_list_contains(
		ml, (Move) {.from = SQ_C2, .to = SQ_C4, .mv_type = MV_PAWN_DOUBLE, .piece = PAWN}));
	TEST_ASSERT_TRUE(move_list_contains(
		ml, (Move) {.from = SQ_D2, .to = SQ_D3, .mv_type = MV_QUIET, .piece = PAWN}));
	TEST_ASSERT_TRUE(move_list_contains(
		ml, (Move) {.from = SQ_D2, .to = SQ_D4, .mv_type = MV_PAWN_DOUBLE, .piece = PAWN}));
	TEST_ASSERT_TRUE(move_list_contains(
		ml, (Move) {.from = SQ_E2, .to = SQ_E3, .mv_type = MV_QUIET, .piece = PAWN}));
	TEST_ASSERT_TRUE(move_list_contains(
		ml, (Move) {.from = SQ_E2, .to = SQ_E4, .mv_type = MV_PAWN_DOUBLE, .piece = PAWN}));
	TEST_ASSERT_TRUE(move_list_contains(
		ml, (Move) {.from = SQ_F2, .to = SQ_F3, .mv_type = MV_QUIET, .piece = PAWN}));
	TEST_ASSERT_TRUE(move_list_contains(
		ml, (Move) {.from = SQ_F2, .to = SQ_F4, .mv_type = MV_PAWN_DOUBLE, .piece = PAWN}));
	TEST_ASSERT_TRUE(move_list_contains(
		ml, (Move) {.from = SQ_G2, .to = SQ_G3, .mv_type = MV_QUIET, .piece = PAWN}));
	TEST_ASSERT_TRUE(move_list_contains(
		ml, (Move) {.from = SQ_G2, .to = SQ_G4, .mv_type = MV_PAWN_DOUBLE, .piece = PAWN}));
	TEST_ASSERT_TRUE(move_list_contains(
		ml, (Move) {.from = SQ_H2, .to = SQ_H3, .mv_type = MV_QUIET, .piece = PAWN}));
	TEST_ASSERT_TRUE(move_list_contains(
		ml, (Move) {.from = SQ_H2, .to = SQ_H4, .mv_type = MV_PAWN_DOUBLE, .piece = PAWN}));
}

void test_black_pawns_have_two_moves_at_starting_row(void) {
	for (Square sqr = SQ_A7; sqr <= SQ_H7; sqr++) {
		Piece pawn = (Piece) {.player = PLAYER_B, .type = PAWN};
		board_set_piece(board, pawn, sqr);
	}

	MoveList *ml = movegen_generate(board, PLAYER_B);
	TEST_ASSERT_EQUAL_size_t(16, move_list_size(ml));

	TEST_ASSERT_TRUE(move_list_contains(
		ml, (Move) {.from = SQ_A7, .to = SQ_A6, .mv_type = MV_QUIET, .piece = PAWN}));
	TEST_ASSERT_TRUE(move_list_contains(
		ml, (Move) {.from = SQ_A7, .to = SQ_A5, .mv_type = MV_PAWN_DOUBLE, .piece = PAWN}));
	TEST_ASSERT_TRUE(move_list_contains(
		ml, (Move) {.from = SQ_B7, .to = SQ_B6, .mv_type = MV_QUIET, .piece = PAWN}));
	TEST_ASSERT_TRUE(move_list_contains(
		ml, (Move) {.from = SQ_B7, .to = SQ_B5, .mv_type = MV_PAWN_DOUBLE, .piece = PAWN}));
	TEST_ASSERT_TRUE(move_list_contains(
		ml, (Move) {.from = SQ_C7, .to = SQ_C6, .mv_type = MV_QUIET, .piece = PAWN}));
	TEST_ASSERT_TRUE(move_list_contains(
		ml, (Move) {.from = SQ_C7, .to = SQ_C5, .mv_type = MV_PAWN_DOUBLE, .piece = PAWN}));
	TEST_ASSERT_TRUE(move_list_contains(
		ml, (Move) {.from = SQ_D7, .to = SQ_D6, .mv_type = MV_QUIET, .piece = PAWN}));
	TEST_ASSERT_TRUE(move_list_contains(
		ml, (Move) {.from = SQ_D7, .to = SQ_D5, .mv_type = MV_PAWN_DOUBLE, .piece = PAWN}));
	TEST_ASSERT_TRUE(move_list_contains(
		ml, (Move) {.from = SQ_E7, .to = SQ_E6, .mv_type = MV_QUIET, .piece = PAWN}));
	TEST_ASSERT_TRUE(move_list_contains(
		ml, (Move) {.from = SQ_E7, .to = SQ_E5, .mv_type = MV_PAWN_DOUBLE, .piece = PAWN}));
	TEST_ASSERT_TRUE(move_list_contains(
		ml, (Move) {.from = SQ_F7, .to = SQ_F6, .mv_type = MV_QUIET, .piece = PAWN}));
	TEST_ASSERT_TRUE(move_list_contains(
		ml, (Move) {.from = SQ_F7, .to = SQ_F5, .mv_type = MV_PAWN_DOUBLE, .piece = PAWN}));
	TEST_ASSERT_TRUE(move_list_contains(
		ml, (Move) {.from = SQ_G7, .to = SQ_G6, .mv_type = MV_QUIET, .piece = PAWN}));
	TEST_ASSERT_TRUE(move_list_contains(
		ml, (Move) {.from = SQ_G7, .to = SQ_G5, .mv_type = MV_PAWN_DOUBLE, .piece = PAWN}));
	TEST_ASSERT_TRUE(move_list_contains(
		ml, (Move) {.from = SQ_H7, .to = SQ_H6, .mv_type = MV_QUIET, .piece = PAWN}));
	TEST_ASSERT_TRUE(move_list_contains(
		ml, (Move) {.from = SQ_H7, .to = SQ_H5, .mv_type = MV_PAWN_DOUBLE, .piece = PAWN}));
}

void test_white_pawns_can_only_move_forward_at_non_starting_rows(void) {
	Piece pawn = (Piece) {.player = PLAYER_W, .type = PAWN};
	for (Square sqr = SQ_A1; sqr <= SQ_H7; sqr++) {	 // skip the rank 8
		if (sqr >= SQ_A2 && sqr <= SQ_H2) {			 // skip the starting rows
			continue;
		}
		board_set_piece(board, pawn, sqr);
		MoveList *ml = movegen_generate(board, PLAYER_W);
		TEST_ASSERT_EQUAL_size_t(1, move_list_size(ml));

		Move m;
		move_list_get(ml, 0, &m);
		TEST_ASSERT_EQUAL(m.to, m.from + DIR_N);
		board_remove_piece(board, sqr);
		move_list_destroy(&ml);
	}
}

void test_black_pawns_can_only_move_forward_at_non_starting_rows(void) {
	Piece pawn = (Piece) {.player = PLAYER_B, .type = PAWN};
	for (Square sqr = SQ_A2; sqr <= SQ_H8; sqr++) {	 // skip the rank 1
		if (sqr >= SQ_A7 && sqr <= SQ_H7) {			 // skip the starting rows
			continue;
		}
		board_set_piece(board, pawn, sqr);

		MoveList *ml = movegen_generate(board, PLAYER_B);
		TEST_ASSERT_EQUAL_size_t(1, move_list_size(ml));

		Move m;
		move_list_get(ml, 0, &m);
		TEST_ASSERT_EQUAL(m.to, m.from + DIR_S);
		board_remove_piece(board, sqr);
		move_list_destroy(&ml);
	}
}

void test_white_pawns_can_capture_enemies_at_ne_and_nw(void) {
	Piece w_pawn = (Piece) {.player = PLAYER_W, .type = PAWN};
	Piece b_pawn = (Piece) {.player = PLAYER_B, .type = PAWN};
	for (Square sqr = SQ_A1; sqr <= SQ_H7; sqr++) {					 // skip the rank 8
		if (utils_get_file(sqr) == 0 || utils_get_file(sqr) == 7) {	 // skip the edges
			continue;
		}
		Square nw = sqr + DIR_NW;
		Square ne = sqr + DIR_NE;
		board_set_piece(board, w_pawn, sqr);
		board_set_piece(board, b_pawn, nw);
		board_set_piece(board, b_pawn, ne);
		MoveList *ml = movegen_generate(board, PLAYER_W);

		if (utils_get_rank(sqr) == 1) {
			TEST_ASSERT_EQUAL_size_t(4, move_list_size(ml));
		} else {
			TEST_ASSERT_EQUAL_size_t(3, move_list_size(ml));
		}

		Move capture_nw = (Move) {.from = sqr, .to = nw, .mv_type = MV_CAPTURE, .piece = PAWN};
		Move capture_ne = (Move) {.from = sqr, .to = ne, .mv_type = MV_CAPTURE, .piece = PAWN};
		TEST_ASSERT_TRUE(move_list_contains(ml, capture_nw));
		TEST_ASSERT_TRUE(move_list_contains(ml, capture_ne));
		board_remove_piece(board, sqr);
		board_remove_piece(board, nw);
		board_remove_piece(board, ne);
		move_list_destroy(&ml);
	}
	for (Square sqr = SQ_A1; sqr <= SQ_A7; sqr += DIR_N) {
		Square ne = sqr + DIR_NE;
		board_set_piece(board, b_pawn, ne);
		board_set_piece(board, w_pawn, sqr);
		MoveList *ml = movegen_generate(board, PLAYER_W);

		if (utils_get_rank(sqr) == 1) {
			TEST_ASSERT_EQUAL_size_t(3, move_list_size(ml));
		} else {
			TEST_ASSERT_EQUAL_size_t(2, move_list_size(ml));
		}
		Move capture_ne = (Move) {.from = sqr, .to = ne, .mv_type = MV_CAPTURE, .piece = PAWN};
		TEST_ASSERT_TRUE(move_list_contains(ml, capture_ne));

		board_remove_piece(board, sqr);
		board_remove_piece(board, ne);
		move_list_destroy(&ml);
	}
	for (Square sqr = SQ_H1; sqr <= SQ_H7; sqr += DIR_N) {
		Square nw = sqr + DIR_NW;
		board_set_piece(board, b_pawn, nw);
		board_set_piece(board, w_pawn, sqr);
		MoveList *ml = movegen_generate(board, PLAYER_W);

		if (utils_get_rank(sqr) == 1) {
			TEST_ASSERT_EQUAL_size_t(3, move_list_size(ml));
		} else {
			TEST_ASSERT_EQUAL_size_t(2, move_list_size(ml));
		}
		Move capture_nw = (Move) {.from = sqr, .to = nw, .mv_type = MV_CAPTURE, .piece = PAWN};
		TEST_ASSERT_TRUE(move_list_contains(ml, capture_nw));

		board_remove_piece(board, sqr);
		board_remove_piece(board, nw);
		move_list_destroy(&ml);
	}
}

void test_black_pawns_can_capture_enemies_at_se_and_sw(void) {
	Piece w_pawn = (Piece) {.player = PLAYER_W, .type = PAWN};
	Piece b_pawn = (Piece) {.player = PLAYER_B, .type = PAWN};
	for (Square sqr = SQ_A2; sqr <= SQ_H8; sqr++) {					 // skip the rank 1
		if (utils_get_file(sqr) == 0 || utils_get_file(sqr) == 7) {	 // skip the edges
			continue;
		}
		Square sw = sqr + DIR_SW;
		Square se = sqr + DIR_SE;
		board_set_piece(board, b_pawn, sqr);
		board_set_piece(board, w_pawn, sw);
		board_set_piece(board, w_pawn, se);
		MoveList *ml = movegen_generate(board, PLAYER_B);

		if (utils_get_rank(sqr) == 6) {
			TEST_ASSERT_EQUAL_size_t(4, move_list_size(ml));
		} else {
			TEST_ASSERT_EQUAL_size_t(3, move_list_size(ml));
		}
		Move capture_se = (Move) {.from = sqr, .to = se, .mv_type = MV_CAPTURE, .piece = PAWN};
		Move capture_sw = (Move) {.from = sqr, .to = sw, .mv_type = MV_CAPTURE, .piece = PAWN};
		TEST_ASSERT_TRUE(move_list_contains(ml, capture_se));
		TEST_ASSERT_TRUE(move_list_contains(ml, capture_sw));

		board_remove_piece(board, sqr);
		board_remove_piece(board, sw);
		board_remove_piece(board, se);
		move_list_destroy(&ml);
	}
	for (Square sqr = SQ_A2; sqr <= SQ_A8; sqr += DIR_S) {
		Square se = sqr + DIR_SE;
		board_set_piece(board, w_pawn, se);
		board_set_piece(board, b_pawn, sqr);
		MoveList *ml = movegen_generate(board, PLAYER_B);

		if (utils_get_rank(sqr) == 6) {
			TEST_ASSERT_EQUAL_size_t(3, move_list_size(ml));
		} else {
			TEST_ASSERT_EQUAL_size_t(2, move_list_size(ml));
		}
		Move capture_se = (Move) {.from = sqr, .to = se, .mv_type = MV_CAPTURE, .piece = PAWN};
		TEST_ASSERT_TRUE(move_list_contains(ml, capture_se));

		board_remove_piece(board, sqr);
		board_remove_piece(board, se);
		move_list_destroy(&ml);
	}
	for (Square sqr = SQ_H2; sqr <= SQ_H8; sqr += DIR_S) {
		Square sw = sqr + DIR_SW;
		board_set_piece(board, b_pawn, sw);
		board_set_piece(board, w_pawn, sqr);
		MoveList *ml = movegen_generate(board, PLAYER_B);

		if (utils_get_rank(sqr) == 1) {
			TEST_ASSERT_EQUAL_size_t(3, move_list_size(ml));
		} else {
			TEST_ASSERT_EQUAL_size_t(2, move_list_size(ml));
		}
		Move capture_sw = (Move) {.from = sqr, .to = sw, .mv_type = MV_CAPTURE, .piece = PAWN};
		TEST_ASSERT_TRUE(move_list_contains(ml, capture_sw));

		board_remove_piece(board, sqr);
		board_remove_piece(board, sw);
		move_list_destroy(&ml);
	}
}

void test_white_pawn_cant_capture_allies(void) {
	Piece  w_pawn = (Piece) {.player = PLAYER_W, .type = PAWN};
	Square sqr	  = SQ_C3;
	board_set_piece(board, w_pawn, sqr);
	board_set_piece(board, w_pawn, sqr + DIR_NE);
	board_set_piece(board, w_pawn, sqr + DIR_NW);

	MoveList *ml = movegen_generate(board, PLAYER_W);
	for (size_t i = 0; i < move_list_size(ml); i++) {
		Move m;
		move_list_get(ml, i, &m);
		TEST_ASSERT_TRUE(m.mv_type != MV_CAPTURE);
	}
	move_list_destroy(&ml);
}

void test_black_pawns_cant_capture_allies(void) {
	Piece  b_pawn = (Piece) {.player = PLAYER_B, .type = PAWN};
	Square sqr	  = SQ_C3;
	board_set_piece(board, b_pawn, sqr);
	board_set_piece(board, b_pawn, sqr + DIR_SE);
	board_set_piece(board, b_pawn, sqr + DIR_SW);

	MoveList *ml = movegen_generate(board, PLAYER_B);
	for (size_t i = 0; i < move_list_size(ml); i++) {
		Move m;
		move_list_get(ml, i, &m);
		TEST_ASSERT_TRUE(m.mv_type != MV_CAPTURE);
	}
	move_list_destroy(&ml);
}

void test_rook_can_move_in_cross(void) {
	Piece rook = (Piece) {.player = PLAYER_W, .type = ROOK};
	for (Square sqr = SQ_A1; sqr <= SQ_H8; sqr++) {
		board_set_piece(board, rook, sqr);
		MoveList *ml = movegen_generate(board, rook.player);
		TEST_ASSERT_EQUAL_size_t(14, move_list_size(ml));

		int file = utils_get_file(sqr);
		int rank = utils_get_rank(sqr);

		int n_moves_cnt = 7 - rank;
		int s_moves_cnt = rank;
		int e_moves_cnt = 7 - file;
		int w_moves_cnt = file;

		for (int i = 1; i <= n_moves_cnt; i++) {
			Move m = (Move) {
				.from = sqr, .to = sqr + DIR_N * i, .mv_type = MV_QUIET, .piece = rook.type};
			sprintf(err_msg,
					"rook should be able to move N to square:%d from square: %d",
					m.to,
					m.from);
			TEST_ASSERT_TRUE_MESSAGE(move_list_contains(ml, m), err_msg);
		}
		for (int i = 1; i <= s_moves_cnt; i++) {
			Move m = (Move) {
				.from = sqr, .to = sqr + DIR_S * i, .mv_type = MV_QUIET, .piece = rook.type};
			sprintf(err_msg,
					"rook should be able to move S to square:%d from square: %d",
					m.to,
					m.from);
			TEST_ASSERT_TRUE_MESSAGE(move_list_contains(ml, m), err_msg);
		}
		for (int i = 1; i <= e_moves_cnt; i++) {
			Move m = (Move) {
				.from = sqr, .to = sqr + DIR_E * i, .mv_type = MV_QUIET, .piece = rook.type};
			sprintf(err_msg,
					"rook should be able to move E to square:%d from square: %d",
					m.to,
					m.from);
			TEST_ASSERT_TRUE_MESSAGE(move_list_contains(ml, m), err_msg);
		}

		for (int i = 1; i <= w_moves_cnt; i++) {
			Move m = (Move) {
				.from = sqr, .to = sqr + DIR_W * i, .mv_type = MV_QUIET, .piece = rook.type};
			sprintf(err_msg,
					"rook should be able to move W to square:%d from square: %d",
					m.to,
					m.from);
			TEST_ASSERT_TRUE_MESSAGE(move_list_contains(ml, m), err_msg);
		}
		board_remove_piece(board, sqr);
		move_list_destroy(&ml);
	}
}

void test_rook_cant_go_over_allies(void) {
	Piece  rook = (Piece) {.player = PLAYER_W, .type = ROOK};
	Piece  pawn = (Piece) {.player = PLAYER_W, .type = PAWN};
	Square sqr	= SQ_D4;
	board_set_piece(board, rook, sqr);
	board_set_piece(board, pawn, sqr + DIR_N);
	board_set_piece(board, pawn, sqr + DIR_S);
	board_set_piece(board, pawn, sqr + DIR_E);
	board_set_piece(board, pawn, sqr + DIR_W);
	MoveList *ml = movegen_generate(board, rook.player);
	// movegen will include the pawns so we'll just check if there is any moves that start at the
	// rook sqr
	for (size_t i = 0; i < move_list_size(ml); i++) {
		Move m;
		move_list_get(ml, i, &m);
		TEST_ASSERT_TRUE(m.from != sqr);
	}
}

void test_rook_cant_go_over_enemies(void) {
	Piece  rook = (Piece) {.player = PLAYER_W, .type = ROOK};
	Piece  pawn = (Piece) {.player = PLAYER_B, .type = PAWN};
	Square sqr	= SQ_D4;
	board_set_piece(board, rook, sqr);
	board_set_piece(board, pawn, sqr + DIR_N);
	board_set_piece(board, pawn, sqr + DIR_S);
	board_set_piece(board, pawn, sqr + DIR_E);
	board_set_piece(board, pawn, sqr + DIR_W);
	MoveList *ml = movegen_generate(board, rook.player);
	TEST_ASSERT_EQUAL(4, move_list_size(ml));
}

void test_knight_can_move_in_L(void) {
	Piece knight = (Piece) {.player = PLAYER_W, .type = KNIGHT};
	for (Square sqr = SQ_A1; sqr <= SQ_H8; sqr++) {
		board_set_piece(board, knight, sqr);
		int		  file = utils_get_file(sqr);
		MoveList *ml   = movegen_generate(board, knight.player);

		Move nwn = (Move) {
			.from = sqr, .to = sqr + DIR_NW + DIR_N, .mv_type = MV_QUIET, .piece = knight.type};
		Move nww = (Move) {
			.from = sqr, .to = sqr + DIR_NW + DIR_W, .mv_type = MV_QUIET, .piece = knight.type};
		Move nen = (Move) {
			.from = sqr, .to = sqr + DIR_NE + DIR_N, .mv_type = MV_QUIET, .piece = knight.type};
		Move nee = (Move) {
			.from = sqr, .to = sqr + DIR_NE + DIR_E, .mv_type = MV_QUIET, .piece = knight.type};

		Move sws = (Move) {
			.from = sqr, .to = sqr + DIR_SW + DIR_S, .mv_type = MV_QUIET, .piece = knight.type};
		Move sww = (Move) {
			.from = sqr, .to = sqr + DIR_SW + DIR_W, .mv_type = MV_QUIET, .piece = knight.type};
		Move ses = (Move) {
			.from = sqr, .to = sqr + DIR_SE + DIR_S, .mv_type = MV_QUIET, .piece = knight.type};
		Move see = (Move) {
			.from = sqr, .to = sqr + DIR_SE + DIR_E, .mv_type = MV_QUIET, .piece = knight.type};

		if (file != 0) {
			if (nwn.to > SQ_NONE && nwn.to < SQ_CNT) {
				sprintf(err_msg, "knight at %d should be able to move NWN to %d", nwn.from, nwn.to);
				TEST_ASSERT_TRUE(move_list_contains(ml, nwn));
			}
			if (sws.to > SQ_NONE && sws.to < SQ_CNT) {
				sprintf(err_msg, "knight at %d should be able to move SWS to %d", sws.from, sws.to);
				TEST_ASSERT_TRUE_MESSAGE(move_list_contains(ml, sws), err_msg);
			}
		}
		if (file != 0 && file != 1) {
			if (nww.to > SQ_NONE && nww.to < SQ_CNT) {
				TEST_ASSERT_TRUE(move_list_contains(ml, nww));
			}
			if (sww.to > SQ_NONE && sww.to < SQ_CNT) {
				sprintf(err_msg, "knight at %d should be able to move SWW to %d", sww.from, sww.to);
				TEST_ASSERT_TRUE_MESSAGE(move_list_contains(ml, sww), err_msg);
			}
		}

		if (file != 7) {
			if (nen.to > SQ_NONE && nen.to < SQ_CNT) {
				sprintf(err_msg, "knight at %d should be able to move NEN to %d", nen.from, nen.to);
				TEST_ASSERT_TRUE(move_list_contains(ml, nen));
			}
			if (ses.to > SQ_NONE && ses.to < SQ_CNT) {
				sprintf(err_msg, "knight at %d should be able to move SES to %d", ses.from, ses.to);
				TEST_ASSERT_TRUE_MESSAGE(move_list_contains(ml, ses), err_msg);
			}
		}
		if (file != 7 && file != 6) {
			if (nen.to > SQ_NONE && nee.to < SQ_CNT) {
				sprintf(err_msg, "knight at %d should be able to move NEE to %d", nee.from, nee.to);
				TEST_ASSERT_TRUE(move_list_contains(ml, nee));
			}
			if (see.to > SQ_NONE && see.to < SQ_CNT) {
				sprintf(err_msg, "knight at %d should be able to move SEE to %d", see.from, see.to);
				TEST_ASSERT_TRUE_MESSAGE(move_list_contains(ml, see), err_msg);
			}
		}
		board_remove_piece(board, sqr);
		move_list_destroy(&ml);
	}
}

void test_bishop_can_move_in_diagonals(void) {
	Piece bishop = (Piece) {.player = PLAYER_W, .type = BISHOP};
	for (Square sqr = SQ_A1; sqr <= SQ_H8; sqr++) {
		board_set_piece(board, bishop, sqr);
		MoveList *ml = movegen_generate(board, bishop.player);

		int file = utils_get_file(sqr);
		int rank = utils_get_rank(sqr);
		for (int i = 1, f = file + 1, r = rank + 1; f >= 0 && f <= 7 && r >= 0 && r <= 7;
			 f++, r++, i++) {
			Move m = (Move) {
				.from = sqr, .to = sqr + DIR_NE * i, .mv_type = MV_QUIET, .piece = bishop.type};
			sprintf(err_msg,
					"bishop should be able to move NE to square:%d from square: %d",
					m.to,
					m.from);
			TEST_ASSERT_TRUE_MESSAGE(move_list_contains(ml, m), err_msg);
		}
		for (int i = 1, f = file + 1, r = rank - 1; f >= 0 && f <= 7 && r >= 0 && r <= 7;
			 f++, r--, i++) {
			Move m = (Move) {
				.from = sqr, .to = sqr + DIR_SE * i, .mv_type = MV_QUIET, .piece = bishop.type};
			sprintf(err_msg,
					"bishop should be able to move SE to square:%d from square: %d",
					m.to,
					m.from);
			TEST_ASSERT_TRUE_MESSAGE(move_list_contains(ml, m), err_msg);
		}
		for (int i = 1, f = file - 1, r = rank - 1; f >= 0 && f <= 7 && r >= 0 && r <= 7;
			 f--, r--, i++) {
			Move m = (Move) {
				.from = sqr, .to = sqr + DIR_SW * i, .mv_type = MV_QUIET, .piece = bishop.type};
			sprintf(err_msg,
					"bishop should be able to move SW to square:%d from square: %d",
					m.to,
					m.from);
			TEST_ASSERT_TRUE_MESSAGE(move_list_contains(ml, m), err_msg);
		}
		for (int i = 1, f = file - 1, r = rank + 1; f >= 0 && f <= 7 && r >= 0 && r <= 7;
			 f--, r++, i++) {
			Move m = (Move) {
				.from = sqr, .to = sqr + DIR_NW * i, .mv_type = MV_QUIET, .piece = bishop.type};
			sprintf(err_msg,
					"bishop should be able to move NW to square:%d from square: %d",
					m.to,
					m.from);
			TEST_ASSERT_TRUE_MESSAGE(move_list_contains(ml, m), err_msg);
		}

		board_remove_piece(board, sqr);
		move_list_destroy(&ml);
	}
}

void test_bishop_cant_skip_allies(void) {
	Piece  bishop = (Piece) {.player = PLAYER_W, .type = BISHOP};
	Square sqr	  = SQ_C3;
	board_set_piece(board, bishop, sqr);
	board_set_piece(board, bishop, sqr + DIR_NE);
	board_set_piece(board, bishop, sqr + DIR_SE);
	board_set_piece(board, bishop, sqr + DIR_NW);
	board_set_piece(board, bishop, sqr + DIR_SW);
	MoveList *ml = movegen_generate(board, bishop.player);
	for (size_t i = 0; i < move_list_size(ml); i++) {
		Move m;
		move_list_get(ml, i, &m);
		TEST_ASSERT_TRUE(m.from != sqr);
	}
	move_list_destroy(&ml);
}

void test_bishop_cant_skip_enemies(void) {
	Piece  bishop = (Piece) {.player = PLAYER_W, .type = BISHOP};
	Piece  pawn	  = (Piece) {.player = PLAYER_B, .type = PAWN};
	Square sqr	  = SQ_C3;
	board_set_piece(board, bishop, sqr);
	board_set_piece(board, pawn, sqr + DIR_NE);
	board_set_piece(board, pawn, sqr + DIR_SE);
	board_set_piece(board, pawn, sqr + DIR_NW);
	board_set_piece(board, pawn, sqr + DIR_SW);
	MoveList *ml = movegen_generate(board, bishop.player);
	TEST_ASSERT_EQUAL(4, move_list_size(ml));
	move_list_destroy(&ml);
}

void test_queen_can_move_in_cross_and_diag(void) {
	Piece queen = (Piece) {.player = PLAYER_W, .type = QUEEN};
	for (Square sqr = SQ_A1; sqr <= SQ_H8; sqr++) {
		board_set_piece(board, queen, sqr);
		MoveList *ml   = movegen_generate(board, queen.player);
		int		  file = utils_get_file(sqr);
		int		  rank = utils_get_rank(sqr);
		for (int i = 1, r = rank + 1; r >= 0 && r <= 7; r++, i++) {
			Move m = (Move) {
				.from = sqr, .to = sqr + DIR_N * i, .mv_type = MV_QUIET, .piece = queen.type};
			sprintf(err_msg,
					"queen should be able to move N to square:%d from square: %d",
					m.to,
					m.from);
			TEST_ASSERT_TRUE_MESSAGE(move_list_contains(ml, m), err_msg);
		}
		for (int i = 1, r = rank - 1; r >= 0 && r <= 7; r--, i++) {
			Move m = (Move) {
				.from = sqr, .to = sqr + DIR_S * i, .mv_type = MV_QUIET, .piece = queen.type};
			sprintf(err_msg,
					"queen should be able to move S to square:%d from square: %d",
					m.to,
					m.from);
			TEST_ASSERT_TRUE_MESSAGE(move_list_contains(ml, m), err_msg);
		}
		for (int i = 1, f = file + 1; f >= 0 && f <= 7; f++, i++) {
			Move m = (Move) {
				.from = sqr, .to = sqr + DIR_E * i, .mv_type = MV_QUIET, .piece = queen.type};
			sprintf(err_msg,
					"queen should be able to move E to square:%d from square: %d",
					m.to,
					m.from);
			TEST_ASSERT_TRUE_MESSAGE(move_list_contains(ml, m), err_msg);
		}
		for (int i = 1, f = file - 1; f >= 0 && f <= 7; f--, i++) {
			Move m = (Move) {
				.from = sqr, .to = sqr + DIR_W * i, .mv_type = MV_QUIET, .piece = queen.type};
			sprintf(err_msg,
					"queen should be able to move W to square:%d from square: %d",
					m.to,
					m.from);
			TEST_ASSERT_TRUE_MESSAGE(move_list_contains(ml, m), err_msg);
		}
		for (int i = 1, f = file + 1, r = rank + 1; f >= 0 && f <= 7 && r >= 0 && r <= 7;
			 f++, r++, i++) {
			Move m = (Move) {
				.from = sqr, .to = sqr + DIR_NE * i, .mv_type = MV_QUIET, .piece = queen.type};
			sprintf(err_msg,
					"queen should be able to move NE to square:%d from square: %d",
					m.to,
					m.from);
			TEST_ASSERT_TRUE_MESSAGE(move_list_contains(ml, m), err_msg);
		}
		for (int i = 1, f = file + 1, r = rank - 1; f >= 0 && f <= 7 && r >= 0 && r <= 7;
			 f++, r--, i++) {
			Move m = (Move) {
				.from = sqr, .to = sqr + DIR_SE * i, .mv_type = MV_QUIET, .piece = queen.type};
			sprintf(err_msg,
					"queen should be able to move SE to square:%d from square: %d",
					m.to,
					m.from);
			TEST_ASSERT_TRUE_MESSAGE(move_list_contains(ml, m), err_msg);
		}
		for (int i = 1, f = file - 1, r = rank - 1; f >= 0 && f <= 7 && r >= 0 && r <= 7;
			 f--, r--, i++) {
			Move m = (Move) {
				.from = sqr, .to = sqr + DIR_SW * i, .mv_type = MV_QUIET, .piece = queen.type};
			sprintf(err_msg,
					"queen should be able to move SW to square:%d from square: %d",
					m.to,
					m.from);
			TEST_ASSERT_TRUE_MESSAGE(move_list_contains(ml, m), err_msg);
		}
		for (int i = 1, f = file - 1, r = rank + 1; f >= 0 && f <= 7 && r >= 0 && r <= 7;
			 f--, r++, i++) {
			Move m = (Move) {
				.from = sqr, .to = sqr + DIR_NW * i, .mv_type = MV_QUIET, .piece = queen.type};
			sprintf(err_msg,
					"queen should be able to move NW to square:%d from square: %d",
					m.to,
					m.from);
			TEST_ASSERT_TRUE_MESSAGE(move_list_contains(ml, m), err_msg);
		}
		board_remove_piece(board, sqr);
		move_list_destroy(&ml);
	}
}

void test_queen_cant_skip_allies(void) {
	Piece  queen = (Piece) {.player = PLAYER_W, .type = QUEEN};
	Piece  pawn	 = (Piece) {.player = PLAYER_W, .type = PAWN};
	Square sqr	 = SQ_C3;
	board_set_piece(board, queen, sqr);
	board_set_piece(board, pawn, sqr + DIR_N);
	board_set_piece(board, pawn, sqr + DIR_S);
	board_set_piece(board, pawn, sqr + DIR_E);
	board_set_piece(board, pawn, sqr + DIR_W);
	board_set_piece(board, pawn, sqr + DIR_NE);
	board_set_piece(board, pawn, sqr + DIR_SE);
	board_set_piece(board, pawn, sqr + DIR_NW);
	board_set_piece(board, pawn, sqr + DIR_SW);
	MoveList *ml = movegen_generate(board, queen.player);
	for (size_t i = 0; i < move_list_size(ml); i++) {
		Move m;
		move_list_get(ml, i, &m);
		TEST_ASSERT_TRUE(m.from != sqr);
	}
	move_list_destroy(&ml);
}

void test_queen_cant_skip_enemies(void) {
	Piece  queen = (Piece) {.player = PLAYER_W, .type = QUEEN};
	Piece  pawn	 = (Piece) {.player = PLAYER_B, .type = PAWN};
	Square sqr	 = SQ_C3;
	board_set_piece(board, queen, sqr);
	board_set_piece(board, pawn, sqr + DIR_N);
	board_set_piece(board, pawn, sqr + DIR_S);
	board_set_piece(board, pawn, sqr + DIR_E);
	board_set_piece(board, pawn, sqr + DIR_W);
	board_set_piece(board, pawn, sqr + DIR_NE);
	board_set_piece(board, pawn, sqr + DIR_SE);
	board_set_piece(board, pawn, sqr + DIR_NW);
	board_set_piece(board, pawn, sqr + DIR_SW);
	MoveList *ml = movegen_generate(board, queen.player);
	TEST_ASSERT_EQUAL(8, move_list_size(ml));
	move_list_destroy(&ml);
}

int main(void) {
	UNITY_BEGIN();
	RUN_TEST(test_white_pawns_have_two_moves_at_starting_row);
	RUN_TEST(test_black_pawns_have_two_moves_at_starting_row);
	RUN_TEST(test_white_pawns_can_only_move_forward_at_non_starting_rows);
	RUN_TEST(test_black_pawns_can_only_move_forward_at_non_starting_rows);
	RUN_TEST(test_white_pawns_can_capture_enemies_at_ne_and_nw);
	RUN_TEST(test_rook_can_move_in_cross);
	RUN_TEST(test_rook_cant_go_over_allies);
	RUN_TEST(test_rook_cant_go_over_enemies);
	RUN_TEST(test_knight_can_move_in_L);
	RUN_TEST(test_bishop_can_move_in_diagonals);
	RUN_TEST(test_bishop_cant_skip_allies);
	RUN_TEST(test_bishop_cant_skip_enemies);
	RUN_TEST(test_queen_can_move_in_cross_and_diag);
	RUN_TEST(test_queen_cant_skip_allies);
	RUN_TEST(test_queen_cant_skip_enemies);
	return UNITY_END();
}
