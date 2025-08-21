#include "../src/engine/fen.h"

#include <stdint.h>

#include "../external/unity/unity.h"
#include "../src/engine/board.h"
#include "../src/engine/utils.h"

Board* board = NULL;

void setUp(void) {
	board = board_create();
}

void tearDown(void) {
	board_destroy(&board);
}

void test_parse_empty_board(void) {
	bool parsed = fen_parse("8/8/8/8/8/8/8/8 w - - 0 1", board);
	TEST_ASSERT_TRUE(parsed);

	for (int i = 0; i < 8; i++) {
		for (int j = 0; j < 8; j++) {
			Square sqr = utils_fr_to_square(i, j);
			Piece  p   = board_get_piece(board, sqr);
			TEST_ASSERT_EQUAL(p.type, EMPTY);
		}
	}

	TEST_ASSERT_EQUAL(board->side, PLAYER_W);
	TEST_ASSERT_EQUAL(board->ep_target, SQ_NONE);
	TEST_ASSERT_EQUAL(board->castling_rights, 0);
	TEST_ASSERT_EQUAL(board->halfmove_clock, 0);
	TEST_ASSERT_EQUAL(board->fullmove_counter, 1);
}

void test_parse_initial_setup(void) {
	bool parsed = fen_parse("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1", board);
	TEST_ASSERT_TRUE(parsed);

	Piece w_lrook = board_get_piece(board, SQ_A1);
	TEST_ASSERT_EQUAL(w_lrook.type, ROOK);
	TEST_ASSERT_EQUAL(w_lrook.player, PLAYER_W);

	Piece w_lknight = board_get_piece(board, SQ_B1);
	TEST_ASSERT_EQUAL(w_lknight.type, KNIGHT);
	TEST_ASSERT_EQUAL(w_lknight.player, PLAYER_W);

	Piece w_lbishop = board_get_piece(board, SQ_C1);
	TEST_ASSERT_EQUAL(w_lbishop.type, BISHOP);
	TEST_ASSERT_EQUAL(w_lbishop.player, PLAYER_W);

	Piece w_queen = board_get_piece(board, SQ_D1);
	TEST_ASSERT_EQUAL(w_queen.type, QUEEN);
	TEST_ASSERT_EQUAL(w_queen.player, PLAYER_W);

	Piece w_king = board_get_piece(board, SQ_E1);
	TEST_ASSERT_EQUAL(w_king.type, KING);
	TEST_ASSERT_EQUAL(w_king.player, PLAYER_W);

	Piece w_rbishop = board_get_piece(board, SQ_F1);
	TEST_ASSERT_EQUAL(w_rbishop.type, BISHOP);
	TEST_ASSERT_EQUAL(w_rbishop.player, PLAYER_W);

	Piece w_rknight = board_get_piece(board, SQ_G1);
	TEST_ASSERT_EQUAL(w_rknight.type, KNIGHT);
	TEST_ASSERT_EQUAL(w_rknight.player, PLAYER_W);

	Piece w_rrook = board_get_piece(board, SQ_H1);
	TEST_ASSERT_EQUAL(w_rrook.type, ROOK);
	TEST_ASSERT_EQUAL(w_rrook.player, PLAYER_W);

	for (int rank = 2; rank < 6; rank++) {
		for (int file = 0; file < 8; file++) {
			Square sqr	 = utils_fr_to_square(file, rank);
			Piece  piece = board_get_piece(board, sqr);
			TEST_ASSERT_EQUAL(piece.type, EMPTY);
		}
	}

	Piece b_lrook = board_get_piece(board, SQ_A8);
	TEST_ASSERT_EQUAL(b_lrook.type, ROOK);
	TEST_ASSERT_EQUAL(b_lrook.player, PLAYER_B);

	Piece b_lknight = board_get_piece(board, SQ_B8);
	TEST_ASSERT_EQUAL(b_lknight.type, KNIGHT);
	TEST_ASSERT_EQUAL(b_lknight.player, PLAYER_B);

	Piece b_lbishop = board_get_piece(board, SQ_C8);
	TEST_ASSERT_EQUAL(b_lbishop.type, BISHOP);
	TEST_ASSERT_EQUAL(b_lbishop.player, PLAYER_B);

	Piece b_queen = board_get_piece(board, SQ_D8);
	TEST_ASSERT_EQUAL(b_queen.type, QUEEN);
	TEST_ASSERT_EQUAL(b_queen.player, PLAYER_B);

	Piece b_king = board_get_piece(board, SQ_E8);
	TEST_ASSERT_EQUAL(b_king.type, KING);
	TEST_ASSERT_EQUAL(b_king.player, PLAYER_B);

	Piece b_rbishop = board_get_piece(board, SQ_F8);
	TEST_ASSERT_EQUAL(b_rbishop.type, BISHOP);
	TEST_ASSERT_EQUAL(b_rbishop.player, PLAYER_B);

	Piece b_rknight = board_get_piece(board, SQ_G8);
	TEST_ASSERT_EQUAL(b_rknight.type, KNIGHT);
	TEST_ASSERT_EQUAL(b_rknight.player, PLAYER_B);

	Piece b_rrook = board_get_piece(board, SQ_H8);
	TEST_ASSERT_EQUAL(b_rrook.type, ROOK);
	TEST_ASSERT_EQUAL(b_rrook.player, PLAYER_B);
}

void test_parse_all_castling_rights(void) {
	bool parsed = fen_parse("8/8/8/8/8/8/8/8 w KQkq - 0 1", board);
	TEST_ASSERT_EQUAL(parsed, true);
	TEST_ASSERT_EQUAL(board->castling_rights, CASTLING_ALL_RIGHTS);
}

void test_parse_white_no_castling_rights(void) {
	bool parsed = fen_parse("8/8/8/8/8/8/8/R3K2R w kq - 0 1", board);
	TEST_ASSERT_EQUAL(parsed, true);
	TEST_ASSERT_EQUAL(board->castling_rights, CASTLING_BLACK_ALL);
}

void test_parse_no_black_castling_rights(void) {
	bool parsed = fen_parse("8/8/8/8/8/8/8/2R1K2R b KQ - 0 1", board);
	TEST_ASSERT_EQUAL(parsed, true);
	TEST_ASSERT_EQUAL(board->castling_rights, CASTLING_WHITE_ALL);
}

void test_parse_no_castling_rights(void) {
	bool parsed = fen_parse("8/8/8/8/8/8/8/R3K2R b - - 0 1", board);
	TEST_ASSERT_EQUAL(parsed, true);
	TEST_ASSERT_EQUAL(board->castling_rights, CASTLING_NO_RIGHTS);
}

void test_parse_kingside_castling_rights(void) {
	bool parsed = fen_parse("8/8/8/8/8/8/8/R3K2R w Kk - 0 1", board);
	TEST_ASSERT_EQUAL(parsed, true);
	TEST_ASSERT_EQUAL(board->castling_rights, CASTLING_WHITE_KS | CASTLING_BLACK_KS);
}

void test_parse_queenside_castling_rights(void) {
	bool parsed = fen_parse("8/8/8/8/8/8/8/R3K2R w Qq - 0 1", board);
	TEST_ASSERT_EQUAL(parsed, true);
	TEST_ASSERT_EQUAL(board->castling_rights, CASTLING_WHITE_QS | CASTLING_BLACK_QS);
}

void test_parse_invalid_castling_rights(void) {
	bool parsed = fen_parse("8/8/8/8/8/8/8/R3K2R w tq - 0 1", board);
	TEST_ASSERT_EQUAL(parsed, false);
}

void test_parse_en_passant(void) {
	bool parsed = fen_parse("8/8/8/8/8/8/8/R3K2R w Qq e6 0 1", board);
	TEST_ASSERT_EQUAL(parsed, true);
	Square expected = SQ_E6;
	Square actual	= board->ep_target;
	TEST_ASSERT_EQUAL(expected, actual);
}

void test_parse_en_passant_top_right_corner(void) {
	bool parsed = fen_parse("8/8/8/8/8/8/8/R3K2R w Qq h8 0 1", board);
	TEST_ASSERT_EQUAL(parsed, true);
	Square expected = SQ_H8;
	Square actual	= board->ep_target;
	TEST_ASSERT_EQUAL(expected, actual);
}

void test_parse_en_passant_bottom_left_corner(void) {
	bool parsed = fen_parse("8/8/8/8/8/8/8/2R1K2R b Qq a1 0 1", board);
	TEST_ASSERT_EQUAL(parsed, true);
	Square expected = SQ_A1;
	Square actual	= board->ep_target;
	TEST_ASSERT_EQUAL(expected, actual);
}

void test_parse_en_passant_invalid_number(void) {
	bool parsed = fen_parse("8/8/8/8/8/8/8/R3K2R w Qq e0 0 1", board);
	TEST_ASSERT_EQUAL(parsed, false);
}

void test_parse_en_passant_out_of_bounds_number(void) {
	bool parsed = fen_parse("8/8/8/8/8/8/8/R3K2R w Qq e9 0 1", board);
	TEST_ASSERT_EQUAL(parsed, false);
}

void test_parse_en_passant_invalid_letter(void) {
	bool parsed = fen_parse("8/8/8/8/8/8/8/R3K2R w Qq i2 0 1", board);
	TEST_ASSERT_EQUAL(parsed, false);
}

void test_parse_halfmove_clock_zero(void) {
	bool parsed = fen_parse("8/8/8/8/8/8/8/R3K2R w Qq e6 0 1", board);
	TEST_ASSERT_EQUAL(parsed, true);
	TEST_ASSERT_EQUAL(board->halfmove_clock, 0);
}

void test_parse_halfmove_clock_one(void) {
	bool parsed = fen_parse("8/8/8/8/8/8/8/R3K2R w Qq e6 1 1", board);
	TEST_ASSERT_EQUAL(parsed, true);
	TEST_ASSERT_EQUAL(board->halfmove_clock, 1);
}

void test_parse_halfmove_clock_hundred(void) {
	bool parsed = fen_parse("8/8/8/8/8/8/8/R3K2R w Qq e6 100 1", board);
	TEST_ASSERT_EQUAL(parsed, true);
	TEST_ASSERT_EQUAL(board->halfmove_clock, 100);
}

void test_parse_halfmove_clock_invalid(void) {
	bool parsed = fen_parse("8/8/8/8/8/8/8/R3K2R w Qq e6 1a0 1", board);
	TEST_ASSERT_EQUAL(parsed, false);
}

void test_parse_active_player_white(void) {
	bool parsed = fen_parse("8/8/8/8/8/8/8/R3K2R w Kk e6 0 1", board);
	TEST_ASSERT_EQUAL(parsed, true);
	TEST_ASSERT_EQUAL(board->side, PLAYER_W);
}

void test_parse_active_player_black(void) {
	bool parsed = fen_parse("8/8/8/8/8/8/8/R3K2R b Kk e6 1 1", board);
	TEST_ASSERT_EQUAL(parsed, true);
	TEST_ASSERT_EQUAL(board->side, PLAYER_B);
}

void test_parse_active_player_invalid_char(void) {
	bool parsed = fen_parse("8/8/8/8/8/8/8/R3K2R n Kk e6 1 1", board);
	TEST_ASSERT_EQUAL(parsed, false);
}

void test_parse_active_player_invalid_int(void) {
	bool parsed = fen_parse("8/8/8/8/8/8/8/R3K2R 2 Kk e6 1 1", board);
	TEST_ASSERT_EQUAL(parsed, false);
}

void test_parse_fullmove_counter_zero(void) {
	bool parsed = fen_parse("8/8/8/8/8/8/8/R3K2R w Kk e6 1 0", board);
	TEST_ASSERT_EQUAL(parsed, true);
	TEST_ASSERT_EQUAL(board->fullmove_counter, 0);
}

void test_parse_fullmove_counter_one(void) {
	bool parsed = fen_parse("8/8/8/8/8/8/8/R3K2R w Kk e6 1 1", board);
	TEST_ASSERT_EQUAL(parsed, true);
	TEST_ASSERT_EQUAL(board->fullmove_counter, 1);
}

void test_parse_fullmove_counter_prime(void) {
	bool parsed = fen_parse("8/8/8/8/8/8/8/R3K2R w Kk e6 0 13", board);
	TEST_ASSERT_EQUAL(parsed, true);
	TEST_ASSERT_EQUAL(board->fullmove_counter, 13);
}

void test_parse_fullmove_counter_hundred(void) {
	bool parsed = fen_parse("8/8/8/8/8/8/8/R3K2R w Kk e6 10 100", board);
	TEST_ASSERT_EQUAL(parsed, true);
	TEST_ASSERT_EQUAL(board->fullmove_counter, 100);
}

void test_parse_fullmove_counter_invalid(void) {
	bool parsed = fen_parse("8/8/8/8/8/8/8/R3K2R w Kk e6 10 1a1", board);
	TEST_ASSERT_EQUAL(parsed, false);
}

void test_parse_situation_kiwipete(void) {
	bool parsed =
		fen_parse("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1", board);
	TEST_ASSERT_TRUE(parsed);

	// white side

	Piece w_lrook = board_get_piece(board, SQ_A1);
	TEST_ASSERT_EQUAL(w_lrook.type, ROOK);
	TEST_ASSERT_EQUAL(w_lrook.player, PLAYER_W);

	Piece w_lknight = board_get_piece(board, SQ_C3);
	TEST_ASSERT_EQUAL(w_lknight.type, KNIGHT);
	TEST_ASSERT_EQUAL(w_lknight.player, PLAYER_W);

	Piece w_lbishop = board_get_piece(board, SQ_D2);
	TEST_ASSERT_EQUAL(w_lbishop.type, BISHOP);
	TEST_ASSERT_EQUAL(w_lbishop.player, PLAYER_W);

	Piece w_queen = board_get_piece(board, SQ_F3);
	TEST_ASSERT_EQUAL(w_queen.type, QUEEN);
	TEST_ASSERT_EQUAL(w_queen.player, PLAYER_W);

	Piece w_king = board_get_piece(board, SQ_E1);
	TEST_ASSERT_EQUAL(w_king.type, KING);
	TEST_ASSERT_EQUAL(w_king.player, PLAYER_W);

	Piece w_rbishop = board_get_piece(board, SQ_E2);
	TEST_ASSERT_EQUAL(w_rbishop.type, BISHOP);
	TEST_ASSERT_EQUAL(w_rbishop.player, PLAYER_W);

	Piece w_rknight = board_get_piece(board, SQ_E5);
	TEST_ASSERT_EQUAL(w_rknight.type, KNIGHT);
	TEST_ASSERT_EQUAL(w_rknight.player, PLAYER_W);

	Piece w_rrook = board_get_piece(board, SQ_H1);
	TEST_ASSERT_EQUAL(w_rrook.type, ROOK);
	TEST_ASSERT_EQUAL(w_rrook.player, PLAYER_W);

	Piece w_pawn1 = board_get_piece(board, SQ_A2);
	TEST_ASSERT_EQUAL(w_pawn1.type, PAWN);
	TEST_ASSERT_EQUAL(w_pawn1.player, PLAYER_W);

	Piece w_pawn2 = board_get_piece(board, SQ_B2);
	TEST_ASSERT_EQUAL(w_pawn2.type, PAWN);
	TEST_ASSERT_EQUAL(w_pawn2.player, PLAYER_W);

	Piece w_pawn3 = board_get_piece(board, SQ_C2);
	TEST_ASSERT_EQUAL(w_pawn3.type, PAWN);
	TEST_ASSERT_EQUAL(w_pawn3.player, PLAYER_W);

	Piece w_pawn4 = board_get_piece(board, SQ_D5);
	TEST_ASSERT_EQUAL(w_pawn4.type, PAWN);
	TEST_ASSERT_EQUAL(w_pawn4.player, PLAYER_W);

	Piece w_pawn5 = board_get_piece(board, SQ_E4);
	TEST_ASSERT_EQUAL(w_pawn5.type, PAWN);
	TEST_ASSERT_EQUAL(w_pawn5.player, PLAYER_W);

	Piece w_pawn6 = board_get_piece(board, SQ_F2);
	TEST_ASSERT_EQUAL(w_pawn6.type, PAWN);
	TEST_ASSERT_EQUAL(w_pawn6.player, PLAYER_W);

	Piece w_pawn7 = board_get_piece(board, SQ_G2);
	TEST_ASSERT_EQUAL(w_pawn7.type, PAWN);
	TEST_ASSERT_EQUAL(w_pawn7.player, PLAYER_W);

	Piece w_pawn8 = board_get_piece(board, SQ_H2);
	TEST_ASSERT_EQUAL(w_pawn8.type, PAWN);
	TEST_ASSERT_EQUAL(w_pawn8.player, PLAYER_W);

	// black side

	Piece b_lrook = board_get_piece(board, SQ_A8);
	TEST_ASSERT_EQUAL(b_lrook.type, ROOK);
	TEST_ASSERT_EQUAL(b_lrook.player, PLAYER_B);

	Piece b_lknight = board_get_piece(board, SQ_B6);
	TEST_ASSERT_EQUAL(b_lknight.type, KNIGHT);
	TEST_ASSERT_EQUAL(b_lknight.player, PLAYER_B);

	Piece b_lbishop = board_get_piece(board, SQ_A6);
	TEST_ASSERT_EQUAL(b_lbishop.type, BISHOP);
	TEST_ASSERT_EQUAL(b_lbishop.player, PLAYER_B);

	Piece b_queen = board_get_piece(board, SQ_E7);
	TEST_ASSERT_EQUAL(b_queen.type, QUEEN);
	TEST_ASSERT_EQUAL(b_queen.player, PLAYER_B);

	Piece b_king = board_get_piece(board, SQ_E8);
	TEST_ASSERT_EQUAL(b_king.type, KING);
	TEST_ASSERT_EQUAL(b_king.player, PLAYER_B);

	Piece b_rbishop = board_get_piece(board, SQ_G7);
	TEST_ASSERT_EQUAL(b_rbishop.type, BISHOP);
	TEST_ASSERT_EQUAL(b_rbishop.player, PLAYER_B);

	Piece b_rknight = board_get_piece(board, SQ_F6);
	TEST_ASSERT_EQUAL(b_rknight.type, KNIGHT);
	TEST_ASSERT_EQUAL(b_rknight.player, PLAYER_B);

	Piece b_rrook = board_get_piece(board, SQ_H8);
	TEST_ASSERT_EQUAL(b_rrook.type, ROOK);
	TEST_ASSERT_EQUAL(b_rrook.player, PLAYER_B);

	Piece b_pawn1 = board_get_piece(board, SQ_A7);
	TEST_ASSERT_EQUAL(b_pawn1.type, PAWN);
	TEST_ASSERT_EQUAL(b_pawn1.player, PLAYER_B);

	Piece b_pawn2 = board_get_piece(board, SQ_B4);
	TEST_ASSERT_EQUAL(b_pawn2.type, PAWN);
	TEST_ASSERT_EQUAL(b_pawn2.player, PLAYER_B);

	Piece b_pawn3 = board_get_piece(board, SQ_C7);
	TEST_ASSERT_EQUAL(b_pawn3.type, PAWN);
	TEST_ASSERT_EQUAL(b_pawn3.player, PLAYER_B);

	Piece b_pawn4 = board_get_piece(board, SQ_D7);
	TEST_ASSERT_EQUAL(b_pawn4.type, PAWN);
	TEST_ASSERT_EQUAL(b_pawn4.player, PLAYER_B);

	Piece b_pawn5 = board_get_piece(board, SQ_E6);
	TEST_ASSERT_EQUAL(b_pawn5.type, PAWN);
	TEST_ASSERT_EQUAL(b_pawn5.player, PLAYER_B);

	Piece b_pawn6 = board_get_piece(board, SQ_F7);
	TEST_ASSERT_EQUAL(b_pawn6.type, PAWN);
	TEST_ASSERT_EQUAL(b_pawn6.player, PLAYER_B);

	Piece b_pawn7 = board_get_piece(board, SQ_G6);
	TEST_ASSERT_EQUAL(b_pawn7.type, PAWN);
	TEST_ASSERT_EQUAL(b_pawn7.player, PLAYER_B);

	Piece b_pawn8 = board_get_piece(board, SQ_H3);
	TEST_ASSERT_EQUAL(b_pawn8.type, PAWN);
	TEST_ASSERT_EQUAL(b_pawn8.player, PLAYER_B);
}

void test_fen_from_board(void) {
	FenString fen = fen_from_board(board);
	TEST_ASSERT_EQUAL_STRING("8/8/8/8/8/8/8/8 w KQkq - 0 0", fen.str);
}

void test_fen_from_board_standard_match(void) {
	char standard[] = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
	bool parsed		= fen_parse(standard, board);
	TEST_ASSERT_TRUE(parsed);

	FenString fen = fen_from_board(board);
	TEST_ASSERT_EQUAL_STRING(standard, fen.str);
}

void test_fen_from_board_situation_kiwipete(void) {
	char kiwipete[] = "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1";
	bool parsed		= fen_parse(kiwipete, board);
	TEST_ASSERT_TRUE(parsed);
	board_print(board);

	FenString fen = fen_from_board(board);
	board_from_fen(board, fen.str);
	board_print(board);
	TEST_ASSERT_EQUAL_STRING(kiwipete, fen.str);
}

void test_fen_from_board_without_white_castling_rights(void) {
	char fen_situation[] = "r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1";
	bool parsed			 = fen_parse(fen_situation, board);
	TEST_ASSERT_TRUE(parsed);

	FenString fen = fen_from_board(board);
	TEST_ASSERT_EQUAL_STRING(fen_situation, fen.str);
}

void test_fen_from_board_without_black_castling_rights(void) {
	char fen_situation[] = "r2q1rk1/pP1p2pp/Q4n2/bbp1p3/Np6/1B3NBn/pPPP1PPP/R3K2R b KQ - 0 1";
	bool parsed			 = fen_parse(fen_situation, board);
	TEST_ASSERT_TRUE(parsed);

	FenString fen = fen_from_board(board);
	TEST_ASSERT_EQUAL_STRING(fen_situation, fen.str);
}

void test_fen_from_board_en_passant(void) {
	char fen_situation[] = "8/8/8/8/8/8/8/R3K2R w Qq e6 0 1";
	bool parsed			 = fen_parse(fen_situation, board);
	TEST_ASSERT_TRUE(parsed);

	FenString fen = fen_from_board(board);
	TEST_ASSERT_EQUAL_STRING(fen_situation, fen.str);
}

int main(void) {
	UNITY_BEGIN();
	RUN_TEST(test_parse_empty_board);
	RUN_TEST(test_parse_initial_setup);
	RUN_TEST(test_parse_all_castling_rights);
	RUN_TEST(test_parse_white_no_castling_rights);
	RUN_TEST(test_parse_no_black_castling_rights);
	RUN_TEST(test_parse_no_castling_rights);
	RUN_TEST(test_parse_kingside_castling_rights);
	RUN_TEST(test_parse_queenside_castling_rights);
	RUN_TEST(test_parse_invalid_castling_rights);
	RUN_TEST(test_parse_en_passant);
	RUN_TEST(test_parse_en_passant_top_right_corner);
	RUN_TEST(test_parse_en_passant_bottom_left_corner);
	RUN_TEST(test_parse_en_passant_invalid_number);
	RUN_TEST(test_parse_en_passant_out_of_bounds_number);
	RUN_TEST(test_parse_en_passant_invalid_letter);
	RUN_TEST(test_parse_halfmove_clock_zero);
	RUN_TEST(test_parse_halfmove_clock_one);
	RUN_TEST(test_parse_halfmove_clock_hundred);
	RUN_TEST(test_parse_halfmove_clock_invalid);
	RUN_TEST(test_parse_active_player_white);
	RUN_TEST(test_parse_active_player_black);
	RUN_TEST(test_parse_active_player_invalid_char);
	RUN_TEST(test_parse_active_player_invalid_int);
	RUN_TEST(test_parse_fullmove_counter_zero);
	RUN_TEST(test_parse_fullmove_counter_one);
	RUN_TEST(test_parse_fullmove_counter_prime);
	RUN_TEST(test_parse_fullmove_counter_hundred);
	RUN_TEST(test_parse_fullmove_counter_invalid);
	RUN_TEST(test_parse_situation_kiwipete);
	RUN_TEST(test_fen_from_board);
	RUN_TEST(test_fen_from_board_situation_kiwipete);
	RUN_TEST(test_fen_from_board_without_white_castling_rights);
	RUN_TEST(test_fen_from_board_without_black_castling_rights);
	RUN_TEST(test_fen_from_board_en_passant);

	return UNITY_END();
}
