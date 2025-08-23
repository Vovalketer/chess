#include <stdint.h>
#include <stdio.h>

#include "../external/unity/unity.h"
#include "board.h"
#include "hash.h"
#include "log.h"
#include "makemove.h"
#include "types.h"
Board*	   board				= NULL;
const char fen_standard_match[] = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";

void setUp(void) {
	board = board_create();
	hash_init();
}

void tearDown(void) {
	board_destroy(&board);
}

// board_equals will be a hash comparison so we have this helper instead for tests
bool _board_eq(const Board* b1, const Board* b2) {
	if (b1->side != b2->side) {
		log_info("Sides do not match");
		return false;
	}
	if (b1->castling_rights != b2->castling_rights) {
		log_info("Castling rights do not match");
		return false;
	}
	if (b1->ep_target != b2->ep_target) {
		log_info("EP target does not match");
		return false;
	}
	if (b1->halfmove_clock != b2->halfmove_clock) {
		log_info("Halfmove clock does not match");
		return false;
	}
	if (b1->fullmove_counter != b2->fullmove_counter) {
		log_info("Fullmove counter does not match");
		return false;
	}
	for (int i = 0; i < PIECE_TYPE_CNT; i++) {
		if (b1->pieces[PLAYER_W][i] != b2->pieces[PLAYER_W][i]) {
			log_info("Piece %d does not match", i);
			return false;
		}
		if (b1->pieces[PLAYER_B][i] != b2->pieces[PLAYER_B][i]) {
			log_info("Piece %d does not match", i);
			return false;
		}
	}
	for (Player i = 0; i < PLAYER_CNT; i++) {
		if (b1->occupancies[i] != b2->occupancies[i]) {
			log_info("Occupancy %d does not match", i);
			return false;
		}
	}
	return true;
}

void test_hash_is_not_zero(void) {
	uint64_t hash = hash_board(board);
	TEST_ASSERT_NOT_EQUAL_UINT64(0, hash);
}

void test_same_board_returns_same_hash(void) {
	uint64_t hash1 = hash_board(board);
	uint64_t hash2 = hash_board(board);
	TEST_ASSERT_EQUAL_UINT64(hash1, hash2);
}

void test_different_board_returns_different_hash(void) {
	board_from_fen(board, fen_standard_match);
	Board* board2 = board_create();
	board_from_fen(board2, "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR b KQkq - 0 1");
	TEST_ASSERT_NOT_EQUAL_UINT64(hash_board(board), hash_board(board2));
	board_destroy(&board2);
}

void test_different_board_with_same_fen_returns_same_hash(void) {
	board_from_fen(board, fen_standard_match);
	Board* board2 = board_create();
	board_from_fen(board2, fen_standard_match);
	TEST_ASSERT_EQUAL_UINT64(hash_board(board), hash_board(board2));
	board_destroy(&board2);
}

void test_hash_invariant_on_add_remove(void) {
	uint64_t hash = hash_board(board);
	board->hash	  = hash;
	for (PieceType pt = PAWN; pt < PIECE_TYPE_CNT; pt++) {
		for (Player p = PLAYER_W; p < PLAYER_CNT; p++) {
			for (Square sq = SQ_A1; sq < SQ_CNT; sq++) {
				board_set_piece(board, (Piece) {.player = p, .type = pt}, sq);
				uint64_t hash_with_piece = hash_board(board);
				TEST_ASSERT_NOT_EQUAL_UINT64(hash, hash_with_piece);
				board_remove_piece(board, sq);
				TEST_ASSERT_EQUAL_UINT64(hash, hash_board(board));
			}
		}
	}
}

void test_hash_invariant_on_ep_add_remove(void) {
	uint64_t hash = hash_board(board);
	board->hash	  = hash;
	for (Square sq = SQ_A3; sq <= SQ_H3; sq++) {
		board->ep_target	  = sq;
		uint64_t hash_with_ep = hash_board(board);
		TEST_ASSERT_NOT_EQUAL_UINT64(hash, hash_with_ep);
		board->ep_target = SQ_NONE;
		TEST_ASSERT_EQUAL_UINT64(hash, hash_board(board));
	}
	for (Square sq = SQ_A6; sq <= SQ_H6; sq++) {
		board->ep_target	  = sq;
		uint64_t hash_with_ep = hash_board(board);
		TEST_ASSERT_NOT_EQUAL_UINT64(hash, hash_with_ep);
		board->ep_target = SQ_NONE;
		TEST_ASSERT_EQUAL_UINT64(hash, hash_board(board));
	}
}

void test_hash_consistent_after_ep_target_removal(void) {
	Piece  w_pawn = (Piece) {.player = PLAYER_W, .type = PAWN};
	Square from	  = SQ_A2;
	Square to	  = SQ_A4;
	board_set_piece(board, w_pawn, from);

	Piece  b_pawn	   = (Piece) {.player = PLAYER_B, .type = PAWN};
	Square b_pawn_from = SQ_B2;
	Square b_pawn_to   = SQ_B3;
	board_set_piece(board, b_pawn, b_pawn_from);
	TEST_ASSERT_TRUE(board->ep_target == SQ_NONE);

	uint64_t hash = hash_board(board);
	board->hash	  = hash;

	Move mv_ep = (Move) {
		.piece = w_pawn, .from = from, .to = to, .captured_type = EMPTY, .mv_type = MV_PAWN_DOUBLE};
	TEST_ASSERT_TRUE(make_move(board, mv_ep));

	Move mv_no_ep = (Move) {.piece		   = b_pawn,
							.from		   = b_pawn_from,
							.to			   = b_pawn_to,
							.captured_type = EMPTY,
							.mv_type	   = MV_QUIET};
	TEST_ASSERT_TRUE(make_move(board, mv_no_ep));

	TEST_ASSERT_EQUAL_UINT64(hash_board(board), board->hash);
}

void test_hash_consistent_after_pawn_move(void) {
	Piece  w_pawn = (Piece) {.player = PLAYER_W, .type = PAWN};
	Square from	  = SQ_A2;
	Square to	  = SQ_A3;
	Move   move	  = (Move) {
			.from = from, .to = to, .piece = w_pawn, .captured_type = EMPTY, .mv_type = MV_QUIET};
	board_set_piece(board, w_pawn, from);
	uint64_t hash_at_creation = hash_board(board);
	board->hash				  = hash_at_creation;
	TEST_ASSERT_TRUE(make_move(board, move));

	TEST_ASSERT_NOT_EQUAL_UINT64(hash_at_creation, board->hash);
	TEST_ASSERT_EQUAL_UINT64(hash_board(board), board->hash);
}

void test_hash_consistent_after_w_pawn_double_move(void) {
	Piece  w_pawn = (Piece) {.player = PLAYER_W, .type = PAWN};
	Square from	  = SQ_A2;
	Square to	  = SQ_A4;
	Move   move	  = (Move) {
			.from = from, .to = to, .piece = w_pawn, .captured_type = EMPTY, .mv_type = MV_PAWN_DOUBLE};
	board_set_piece(board, w_pawn, from);
	uint64_t hash_at_creation = hash_board(board);
	board->hash				  = hash_at_creation;

	TEST_ASSERT_TRUE(make_move(board, move));

	TEST_ASSERT_NOT_EQUAL_UINT64(hash_at_creation, board->hash);
	TEST_ASSERT_EQUAL_UINT64(hash_board(board), board->hash);
}

void test_hash_consistent_after_b_pawn_double_move(void) {
	Piece  b_pawn	  = (Piece) {.player = PLAYER_W, .type = PAWN};
	Square b_pawn_sqr = SQ_A7;
	board_set_piece(board, b_pawn, b_pawn_sqr);
	Move	 b_pawn_mv		  = (Move) {.from		   = b_pawn_sqr,
										.to			   = SQ_A5,
										.piece		   = b_pawn,
										.captured_type = EMPTY,
										.mv_type	   = MV_PAWN_DOUBLE};
	uint64_t hash_at_creation = hash_board(board);
	board->hash				  = hash_at_creation;
	bool b_moved			  = make_move(board, b_pawn_mv);
	TEST_ASSERT_TRUE(b_moved);

	TEST_ASSERT_NOT_EQUAL_UINT64(hash_at_creation, board->hash);
	TEST_ASSERT_EQUAL_UINT64(hash_board(board), board->hash);
}

void test_hash_consistent_after_w_capture(void) {
	Piece  w_pawn	  = (Piece) {.player = PLAYER_W, .type = PAWN};
	Square w_pawn_sqr = SQ_A2;
	Piece  b_pawn	  = (Piece) {.player = PLAYER_B, .type = PAWN};
	Square b_pawn_sqr = SQ_B3;
	board_set_piece(board, w_pawn, w_pawn_sqr);
	board_set_piece(board, b_pawn, b_pawn_sqr);
	board->hash = hash_board(board);

	Move mv = (Move) {.piece		 = w_pawn,
					  .from			 = w_pawn_sqr,
					  .to			 = b_pawn_sqr,
					  .captured_type = PAWN,
					  .mv_type		 = MV_CAPTURE};
	TEST_ASSERT_TRUE(make_move(board, mv));

	TEST_ASSERT_EQUAL_UINT64(hash_board(board), board->hash);
}

void test_hash_consistent_after_multiple_captures(void) {
	Piece  w_pawn  = (Piece) {.player = PLAYER_W, .type = PAWN};
	Piece  b_pawn  = (Piece) {.player = PLAYER_B, .type = PAWN};
	Square w1	   = SQ_C5;
	Square w2	   = SQ_E5;
	Square cap_pos = SQ_D4;
	Square b1	   = SQ_C3;
	Square b2	   = SQ_E3;
	board_set_piece(board, w_pawn, w1);
	board_set_piece(board, w_pawn, w2);
	board_set_piece(board, b_pawn, cap_pos);
	board_set_piece(board, b_pawn, b1);
	board_set_piece(board, b_pawn, b2);
	board->hash = hash_board(board);
	// p . p
	// . p .
	// P . P
	Move m1 = (Move) {
		.from = w1, .to = cap_pos, .captured_type = PAWN, .mv_type = MV_CAPTURE, .piece = w_pawn};
	TEST_ASSERT_TRUE(make_move(board, m1));
	Move m2 = (Move) {
		.from = b1, .to = cap_pos, .captured_type = PAWN, .mv_type = MV_CAPTURE, .piece = b_pawn};
	TEST_ASSERT_TRUE(make_move(board, m2));
	Move m3 = (Move) {
		.from = w2, .to = cap_pos, .captured_type = PAWN, .mv_type = MV_CAPTURE, .piece = w_pawn};
	TEST_ASSERT_TRUE(make_move(board, m3));
	Move m4 = (Move) {
		.from = b2, .to = cap_pos, .captured_type = PAWN, .mv_type = MV_CAPTURE, .piece = b_pawn};
	TEST_ASSERT_TRUE(make_move(board, m4));

	TEST_ASSERT_EQUAL_UINT64(hash_board(board), board->hash);
}

void test_hash_consistent_after_b_capture(void) {
	Piece  w_pawn	  = (Piece) {.player = PLAYER_W, .type = PAWN};
	Square w_pawn_sqr = SQ_A2;
	Piece  b_pawn	  = (Piece) {.player = PLAYER_B, .type = PAWN};
	Square b_pawn_sqr = SQ_B3;
	board_set_piece(board, w_pawn, w_pawn_sqr);
	board_set_piece(board, b_pawn, b_pawn_sqr);
	uint64_t hash_at_creation = hash_board(board);
	board->hash				  = hash_at_creation;

	Move mv = (Move) {.piece		 = b_pawn,
					  .from			 = b_pawn_sqr,
					  .to			 = w_pawn_sqr,
					  .captured_type = PAWN,
					  .mv_type		 = MV_CAPTURE};
	TEST_ASSERT_TRUE(make_move(board, mv));

	TEST_ASSERT_NOT_EQUAL_UINT64(hash_at_creation, board->hash);
	TEST_ASSERT_EQUAL_UINT64(hash_board(board), board->hash);
}

void test_hash_consistent_after_promotion(void) {
	Piece  w_pawn = (Piece) {.player = PLAYER_W, .type = PAWN};
	Square from	  = SQ_A7;
	Square to	  = SQ_A8;
	board_set_piece(board, w_pawn, from);
	board->hash = hash_board(board);
	Move mv		= (Move) {
			.piece = w_pawn, .captured_type = EMPTY, .from = from, .to = to, .mv_type = MV_Q_PROM};
	TEST_ASSERT_TRUE(make_move(board, mv));

	TEST_ASSERT_EQUAL_UINT64(hash_board(board), board->hash);
}

void test_hash_consistent_after_promotion_capture(void) {
	Piece  w_pawn = (Piece) {.player = PLAYER_W, .type = PAWN};
	Piece  b_pawn = (Piece) {.player = PLAYER_B, .type = PAWN};
	Square from	  = SQ_A7;
	Square to	  = SQ_B8;
	board_set_piece(board, w_pawn, from);
	board_set_piece(board, b_pawn, to);
	board->hash = hash_board(board);
	Move mv		= (Move) {.piece		 = w_pawn,
						  .captured_type = b_pawn.type,
						  .from			 = from,
						  .to			 = to,
						  .mv_type		 = MV_Q_PROM_CAPTURE};
	TEST_ASSERT_TRUE(make_move(board, mv));

	TEST_ASSERT_EQUAL_UINT64(hash_board(board), board->hash);
}

void test_hash_consistent_after_w_ks_castling(void) {
	Piece  w_king	  = (Piece) {.player = PLAYER_W, .type = KING};
	Square w_king_sqr = KING_CASTLING_W_KS_SRC;
	board_set_piece(board, w_king, w_king_sqr);

	Piece  w_rook	  = (Piece) {.player = PLAYER_W, .type = ROOK};
	Square w_rook_sqr = ROOK_CASTLING_W_KS_SRC;
	board_set_piece(board, w_rook, w_rook_sqr);
	board_set_castling_rights(board, CASTLING_ALL_RIGHTS);

	board->hash = hash_board(board);

	Move mv = (Move) {.piece		 = w_king,
					  .from			 = w_king_sqr,
					  .to			 = KING_CASTLING_W_KS_DST,
					  .captured_type = EMPTY,
					  .mv_type		 = MV_KS_CASTLE};
	TEST_ASSERT_TRUE(make_move(board, mv));

	TEST_ASSERT_EQUAL_UINT64(hash_board(board), board->hash);
}

void test_hash_consistent_after_w_qs_castling(void) {
	Piece  w_king	  = (Piece) {.player = PLAYER_W, .type = KING};
	Square w_king_sqr = KING_CASTLING_W_QS_SRC;
	board_set_piece(board, w_king, w_king_sqr);

	Piece  w_rook	  = (Piece) {.player = PLAYER_W, .type = ROOK};
	Square w_rook_sqr = ROOK_CASTLING_W_QS_SRC;
	board_set_piece(board, w_rook, w_rook_sqr);
	board_set_castling_rights(board, CASTLING_WHITE_QS);

	board->hash = hash_board(board);

	Move mv = (Move) {.piece		 = w_king,
					  .from			 = w_king_sqr,
					  .to			 = KING_CASTLING_W_QS_DST,
					  .captured_type = EMPTY,
					  .mv_type		 = MV_QS_CASTLE};
	TEST_ASSERT_TRUE(make_move(board, mv));

	TEST_ASSERT_EQUAL_UINT64(hash_board(board), board->hash);
}

void test_hash_consistent_after_b_ks_castling(void) {
	Piece  b_king	  = (Piece) {.player = PLAYER_B, .type = KING};
	Square b_king_sqr = KING_CASTLING_B_KS_SRC;
	board_set_piece(board, b_king, b_king_sqr);

	Piece  b_rook	  = (Piece) {.player = PLAYER_B, .type = ROOK};
	Square b_rook_sqr = ROOK_CASTLING_B_KS_SRC;
	board_set_piece(board, b_rook, b_rook_sqr);
	board_set_castling_rights(board, CASTLING_ALL_RIGHTS);

	board->hash = hash_board(board);
	board_print(board);
	fflush(stdout);

	Move mv = (Move) {.piece		 = b_king,
					  .from			 = b_king_sqr,
					  .to			 = KING_CASTLING_B_KS_DST,
					  .captured_type = EMPTY,
					  .mv_type		 = MV_KS_CASTLE};
	TEST_ASSERT_TRUE(make_move(board, mv));
	board_print(board);

	TEST_ASSERT_EQUAL_UINT64(hash_board(board), board->hash);
}

void test_hash_consistent_after_b_qs_castling(void) {
	Piece  b_king	  = (Piece) {.player = PLAYER_B, .type = KING};
	Square b_king_sqr = KING_CASTLING_B_QS_SRC;
	board_set_piece(board, b_king, b_king_sqr);

	Piece  b_rook	  = (Piece) {.player = PLAYER_B, .type = ROOK};
	Square b_rook_sqr = ROOK_CASTLING_B_QS_SRC;
	board_set_piece(board, b_rook, b_rook_sqr);
	board_set_castling_rights(board, CASTLING_ALL_RIGHTS);

	board->hash = hash_board(board);

	Move mv = (Move) {.piece		 = b_king,
					  .from			 = b_king_sqr,
					  .to			 = KING_CASTLING_B_QS_DST,
					  .captured_type = EMPTY,
					  .mv_type		 = MV_QS_CASTLE};
	TEST_ASSERT_TRUE(make_move(board, mv));

	TEST_ASSERT_EQUAL_UINT64(hash_board(board), board->hash);
}

int main(void) {
	UNITY_BEGIN();

	RUN_TEST(test_hash_is_not_zero);
	RUN_TEST(test_same_board_returns_same_hash);
	RUN_TEST(test_different_board_returns_different_hash);
	RUN_TEST(test_different_board_with_same_fen_returns_same_hash);
	RUN_TEST(test_hash_invariant_on_add_remove);
	RUN_TEST(test_hash_invariant_on_ep_add_remove);
	RUN_TEST(test_hash_consistent_after_ep_target_removal);
	RUN_TEST(test_hash_consistent_after_pawn_move);
	RUN_TEST(test_hash_consistent_after_w_pawn_double_move);
	RUN_TEST(test_hash_consistent_after_b_pawn_double_move);
	RUN_TEST(test_hash_consistent_after_w_capture);
	RUN_TEST(test_hash_consistent_after_b_capture);
	RUN_TEST(test_hash_consistent_after_multiple_captures);
	RUN_TEST(test_hash_consistent_after_promotion);
	RUN_TEST(test_hash_consistent_after_promotion_capture);
	RUN_TEST(test_hash_consistent_after_w_ks_castling);
	RUN_TEST(test_hash_consistent_after_w_qs_castling);
	RUN_TEST(test_hash_consistent_after_b_ks_castling);
	RUN_TEST(test_hash_consistent_after_b_qs_castling);

	return UNITY_END();
}
