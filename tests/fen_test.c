#include "fen.h"

#include "criterion/criterion.h"
#include "criterion/new/assert.h"
#include "game_state.h"
#include "log.h"

GameState* state = NULL;
int prev_log_level;

void setup(void) {
	prev_log_level = log_get_level();
	log_set_level(LOG_NONE);
	gstate_create_empty(&state);
}

void teardown(void) {
	log_set_level(prev_log_level);
	gstate_destroy(&state);
	state = NULL;
}

TestSuite(fen, .init = setup, .fini = teardown);

Test(fen, parse_empty_board) {
	bool parsed = fen_parse("8/8/8/8/8/8/8/8 w - - 0 1", state);
	cr_assert_eq(parsed, true);

	for (int i = 0; i < 8; i++) {
		for (int j = 0; j < 8; j++) {
			Position pos = (Position) {.x = i, .y = j};
			Piece p = gstate_get_piece(state, pos);
			cr_assert_eq(p.type, EMPTY);
		}
	}
}

Test(fen, parse_board_with_initial_setup) {
	GameState* standard = NULL;
	gstate_create(&standard);

	bool parsed = fen_parse("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1", state);
	cr_assert_eq(parsed, true);

	for (int i = 0; i < 8; i++) {
		for (int j = 0; j < 8; j++) {
			Position pos = (Position) {.x = i, .y = j};
			Piece fen_piece = gstate_get_piece(state, pos);
			Piece standard_piece = gstate_get_piece(standard, pos);
			cr_assert_eq(fen_piece.type, standard_piece.type);
			cr_assert_eq(fen_piece.player, standard_piece.player);
		}
	}

	gstate_destroy(&standard);
}

Test(fen, parse_board_with_castling_rights) {
	bool parsed = fen_parse("8/8/8/8/8/8/8/8 w KQkq - 0 1", state);
	cr_assert_eq(parsed, true);
	cr_assert(gstate_has_castling_rights_kingside(state, WHITE_PLAYER));
	cr_assert(gstate_has_castling_rights_queenside(state, WHITE_PLAYER));
	cr_assert(gstate_has_castling_rights_kingside(state, BLACK_PLAYER));
	cr_assert(gstate_has_castling_rights_queenside(state, BLACK_PLAYER));
}

Test(fen, parse_no_white_castling_rights) {
	bool parsed = fen_parse("8/8/8/8/8/8/8/R3K2R w kq - 0 1", state);
	cr_assert_eq(parsed, true);
	cr_assert_not(gstate_has_castling_rights_kingside(state, WHITE_PLAYER));
	cr_assert_not(gstate_has_castling_rights_queenside(state, WHITE_PLAYER));
	cr_assert(gstate_has_castling_rights_kingside(state, BLACK_PLAYER));
	cr_assert(gstate_has_castling_rights_queenside(state, BLACK_PLAYER));
}

Test(fen, parse_no_black_castling_rights) {
	bool parsed = fen_parse("8/8/8/8/8/8/8/2R1K2R b KQ - 0 1", state);
	cr_assert_eq(parsed, true);
	cr_assert(gstate_has_castling_rights_kingside(state, WHITE_PLAYER));
	cr_assert(gstate_has_castling_rights_queenside(state, WHITE_PLAYER));
	cr_assert_not(gstate_has_castling_rights_kingside(state, BLACK_PLAYER));
	cr_assert_not(gstate_has_castling_rights_queenside(state, BLACK_PLAYER));
}

Test(fen, parse_no_castling_rights) {
	bool parsed = fen_parse("8/8/8/8/8/8/8/R3K2R b - - 0 1", state);
	cr_assert_eq(parsed, true);
	cr_assert_not(gstate_has_castling_rights_kingside(state, WHITE_PLAYER));
	cr_assert_not(gstate_has_castling_rights_queenside(state, WHITE_PLAYER));
	cr_assert_not(gstate_has_castling_rights_kingside(state, BLACK_PLAYER));
	cr_assert_not(gstate_has_castling_rights_queenside(state, BLACK_PLAYER));
}

Test(fen, parse_kingside_castling_rights) {
	bool parsed = fen_parse("8/8/8/8/8/8/8/R3K2R w Kk - 0 1", state);
	cr_assert_eq(parsed, true);
	cr_assert(gstate_has_castling_rights_kingside(state, WHITE_PLAYER));
	cr_assert_not(gstate_has_castling_rights_queenside(state, WHITE_PLAYER));
	cr_assert(gstate_has_castling_rights_kingside(state, BLACK_PLAYER));
	cr_assert_not(gstate_has_castling_rights_queenside(state, BLACK_PLAYER));
}

Test(fen, parse_queenside_castling_rights) {
	bool parsed = fen_parse("8/8/8/8/8/8/8/R3K2R w Qq - 0 1", state);
	cr_assert_eq(parsed, true);
	cr_assert_not(gstate_has_castling_rights_kingside(state, WHITE_PLAYER));
	cr_assert(gstate_has_castling_rights_queenside(state, WHITE_PLAYER));
	cr_assert_not(gstate_has_castling_rights_kingside(state, BLACK_PLAYER));
	cr_assert(gstate_has_castling_rights_queenside(state, BLACK_PLAYER));
}

Test(fen, parse_invalid_castling_rights) {
	bool parsed = fen_parse("8/8/8/8/8/8/8/R3K2R w tq - 0 1", state);
	cr_assert_eq(parsed, false);
}

Test(fen, parse_en_passant) {
	bool parsed = fen_parse("8/8/8/8/8/8/8/R3K2R w Qq e6 0 1", state);
	cr_assert_eq(parsed, true);
	Position expected = (Position) {4, 2};
	Position actual = gstate_get_en_passant_target(state);
	cr_assert(eq(int, expected.x, actual.x));
	cr_assert(eq(int, expected.y, actual.y));
}

Test(fen, parse_en_passant_top_right_corner) {
	bool parsed = fen_parse("8/8/8/8/8/8/8/R3K2R w Qq h8 0 1", state);
	cr_assert_eq(parsed, true);
	Position expected = (Position) {7, 0};
	Position actual = gstate_get_en_passant_target(state);
	cr_assert(eq(int, expected.x, actual.x));
	cr_assert(eq(int, expected.y, actual.y));
}

Test(fen, parse_en_passant_bottom_left_corner) {
	bool parsed = fen_parse("8/8/8/8/8/8/8/2R1K2R b Qq a1 0 1", state);
	cr_assert_eq(parsed, true);
	Position expected = (Position) {0, 7};
	Position actual = gstate_get_en_passant_target(state);
	cr_assert(eq(int, expected.x, actual.x));
	cr_assert(eq(int, expected.y, actual.y));
}

Test(fen, parse_en_passant_invalid_number) {
	bool parsed = fen_parse("8/8/8/8/8/8/8/R3K2R w Qq e0 0 1", state);
	cr_assert_eq(parsed, false);
}

Test(fen, parse_en_passant_out_of_bounds_number) {
	bool parsed = fen_parse("8/8/8/8/8/8/8/R3K2R w Qq e9 0 1", state);
	cr_assert_eq(parsed, false);
}

Test(fen, parse_en_passant_invalid_letter) {
	bool parsed = fen_parse("8/8/8/8/8/8/8/R3K2R w Qq i2 0 1", state);
	cr_assert_eq(parsed, false);
}

Test(fen, parse_halfmove_clock_zero) {
	bool parsed = fen_parse("8/8/8/8/8/8/8/R3K2R w Qq e6 0 1", state);
	cr_assert_eq(parsed, true);
	cr_assert_eq(gstate_get_halfmove_clock(state), 0);
}

Test(fen, parse_halfmove_clock_one) {
	bool parsed = fen_parse("8/8/8/8/8/8/8/R3K2R w Qq e6 1 1", state);
	cr_assert_eq(parsed, true);
	cr_assert_eq(gstate_get_halfmove_clock(state), 1);
}

Test(fen, parse_halfmove_clock_hundred) {
	bool parsed = fen_parse("8/8/8/8/8/8/8/R3K2R w Qq e6 100 1", state);
	cr_assert_eq(parsed, true);
	cr_assert_eq(gstate_get_halfmove_clock(state), 100);
}

Test(fen, parse_halfmove_clock_invalid) {
	bool parsed = fen_parse("8/8/8/8/8/8/8/R3K2R w Qq e6 1a0 1", state);
	cr_assert_eq(parsed, false);
}

Test(fen, parse_active_player_white) {
	bool parsed = fen_parse("8/8/8/8/8/8/8/R3K2R w Kk e6 0 1", state);
	cr_assert_eq(parsed, true);
	cr_assert_eq(gstate_get_player_turn(state), WHITE_PLAYER);
}

Test(fen, parse_active_player_black) {
	bool parsed = fen_parse("8/8/8/8/8/8/8/R3K2R b Kk e6 1 1", state);
	cr_assert_eq(parsed, true);
	cr_assert_eq(gstate_get_player_turn(state), BLACK_PLAYER);
}

Test(fen, parse_active_player_invalid_char) {
	bool parsed = fen_parse("8/8/8/8/8/8/8/R3K2R n Kk e6 1 1", state);
	cr_assert_eq(parsed, false);
}

Test(fen, parse_active_player_invalid_int) {
	bool parsed = fen_parse("8/8/8/8/8/8/8/R3K2R 2 Kk e6 1 1", state);
	cr_assert_eq(parsed, false);
}

Test(fen, parse_fullmove_counter_zero) {
	bool parsed = fen_parse("8/8/8/8/8/8/8/R3K2R w Kk e6 1 0", state);
	cr_assert_eq(parsed, true);
	cr_assert_eq(gstate_get_fullmove_counter(state), 0);
}

Test(fen, parse_fullmove_counter_one) {
	bool parsed = fen_parse("8/8/8/8/8/8/8/R3K2R w Kk e6 1 1", state);
	cr_assert_eq(parsed, true);
	cr_assert_eq(gstate_get_fullmove_counter(state), 1);
}

Test(fen, parse_fullmove_counter_prime) {
	bool parsed = fen_parse("8/8/8/8/8/8/8/R3K2R w Kk e6 0 13", state);
	cr_assert_eq(parsed, true);
	cr_assert_eq(gstate_get_fullmove_counter(state), 13);
}

Test(fen, parse_fullmove_counter_hundred) {
	bool parsed = fen_parse("8/8/8/8/8/8/8/R3K2R w Kk e6 10 100", state);
	cr_assert_eq(parsed, true);
	cr_assert_eq(gstate_get_fullmove_counter(state), 100);
}

Test(fen, parse_fullmove_counter_invalid) {
	bool parsed = fen_parse("8/8/8/8/8/8/8/R3K2R w Kk e6 10 1a1", state);
	cr_assert_eq(parsed, false);
}
