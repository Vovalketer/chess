#include "match.h"

#include "criterion/criterion.h"
#include "criterion/new/assert.h"

MatchState *match = NULL;

void setup(void) {
	match_create(&match);
}

void teardown(void) {
	match_destroy(&match);
	match = NULL;
}

TestSuite(match, .init = setup, .fini = teardown);

Test(match, create_returns_board_with_standard_piece_placement) {
	for (int i = 0; i < 8; i++) {
		Piece b_pawn = match_get_piece(match, (Position) {i, 1});
		cr_assert_eq(b_pawn.type, PAWN);
		cr_assert_eq(b_pawn.player, BLACK_PLAYER);
	}

	cr_assert_eq(match_get_piece(match, (Position) {0, 0}).type, ROOK);
	cr_assert_eq(match_get_piece(match, (Position) {0, 0}).player, BLACK_PLAYER);
	cr_assert_eq(match_get_piece(match, (Position) {1, 0}).type, KNIGHT);
	cr_assert_eq(match_get_piece(match, (Position) {1, 0}).player, BLACK_PLAYER);
	cr_assert_eq(match_get_piece(match, (Position) {2, 0}).type, BISHOP);
	cr_assert_eq(match_get_piece(match, (Position) {2, 0}).player, BLACK_PLAYER);
	cr_assert_eq(match_get_piece(match, (Position) {3, 0}).type, QUEEN);
	cr_assert_eq(match_get_piece(match, (Position) {3, 0}).player, BLACK_PLAYER);
	cr_assert_eq(match_get_piece(match, (Position) {4, 0}).type, KING);
	cr_assert_eq(match_get_piece(match, (Position) {4, 0}).player, BLACK_PLAYER);
	cr_assert_eq(match_get_piece(match, (Position) {5, 0}).type, BISHOP);
	cr_assert_eq(match_get_piece(match, (Position) {5, 0}).player, BLACK_PLAYER);
	cr_assert_eq(match_get_piece(match, (Position) {6, 0}).type, KNIGHT);
	cr_assert_eq(match_get_piece(match, (Position) {6, 0}).player, BLACK_PLAYER);
	cr_assert_eq(match_get_piece(match, (Position) {7, 0}).type, ROOK);
	cr_assert_eq(match_get_piece(match, (Position) {7, 0}).player, BLACK_PLAYER);

	for (int i = 0; i < 8; i++) {
		Piece w_pawn = match_get_piece(match, (Position) {i, 6});
		cr_assert_eq(w_pawn.type, PAWN);
		cr_assert_eq(w_pawn.player, WHITE_PLAYER);
	}

	cr_assert_eq(match_get_piece(match, (Position) {0, 7}).type, ROOK);
	cr_assert_eq(match_get_piece(match, (Position) {0, 7}).player, WHITE_PLAYER);
	cr_assert_eq(match_get_piece(match, (Position) {1, 7}).type, KNIGHT);
	cr_assert_eq(match_get_piece(match, (Position) {1, 7}).player, WHITE_PLAYER);
	cr_assert_eq(match_get_piece(match, (Position) {2, 7}).type, BISHOP);
	cr_assert_eq(match_get_piece(match, (Position) {2, 7}).player, WHITE_PLAYER);
	cr_assert_eq(match_get_piece(match, (Position) {3, 7}).type, QUEEN);
	cr_assert_eq(match_get_piece(match, (Position) {3, 7}).player, WHITE_PLAYER);
	cr_assert_eq(match_get_piece(match, (Position) {4, 7}).type, KING);
	cr_assert_eq(match_get_piece(match, (Position) {4, 7}).player, WHITE_PLAYER);
	cr_assert_eq(match_get_piece(match, (Position) {5, 7}).type, BISHOP);
	cr_assert_eq(match_get_piece(match, (Position) {5, 7}).player, WHITE_PLAYER);
	cr_assert_eq(match_get_piece(match, (Position) {6, 7}).type, KNIGHT);
	cr_assert_eq(match_get_piece(match, (Position) {6, 7}).player, WHITE_PLAYER);
	cr_assert_eq(match_get_piece(match, (Position) {7, 7}).type, ROOK);
	cr_assert_eq(match_get_piece(match, (Position) {7, 7}).player, WHITE_PLAYER);
}

Test(match, get_turn_returns_zero_when_turn_is_zero) {
	int t = match_get_turn(match);
	cr_assert_eq(t, 0);
}

Test(match, next_turn_returns_one_when_transitioning_from_zero_to_one) {
	int t = match_next_turn(match);
	cr_assert_eq(t, 1);
}

Test(match, get_turn_returns_one_when_turn_is_one) {
	match_next_turn(match);
	int t = match_get_turn(match);
	cr_assert_eq(t, 1);
}

Test(match, get_player_turn_returns_white_when_turn_is_even) {
	for (int i = 0; i < 10; i++) {
		if (i % 2 == 0) {
			Player p = match_get_player_turn(match);
			cr_assert_eq(p, WHITE_PLAYER);
		}
		match_next_turn(match);
	}
}

Test(match, get_player_turn_returns_black_when_turn_is_uneven) {
	for (int i = 0; i < 10; i++) {
		if (i % 2 != 0) {
			Player p = match_get_player_turn(match);
			cr_assert_eq(p, BLACK_PLAYER);
		}
		match_next_turn(match);
	}
}

Test(match, append_turn_record_returns_true_when_successful) {
	Move move = (Move) {(Position) {0, 0}, (Position) {0, 1}};
	bool result = match_append_turn_record(match, move);
	cr_assert(result);
}

Test(match, append_turn_record_appends_move_to_history) {
	Move move = (Move) {(Position) {0, 6}, (Position) {0, 5}};
	bool result = match_append_turn_record(match, move);
	cr_assert(result);
	TurnRecord *record = NULL;
	match_get_turn_record(match, 0, &record);
	cr_assert(move_eq(move, record->move));
	cr_assert_eq(record->turn, 0);
	cr_assert_eq(record->captured_piece.type, EMPTY);
	cr_assert_eq(record->player, WHITE_PLAYER);

	match_next_turn(match);
	Move move2 = (Move) {(Position) {0, 1}, (Position) {0, 2}};
	match_append_turn_record(match, move2);
	cr_assert(result);
	TurnRecord *record2 = NULL;
	match_get_turn_record(match, 1, &record2);
	cr_assert(move_eq(move2, record2->move));
	cr_assert_eq(record2->turn, 1);
	cr_assert_eq(record2->captured_piece.type, EMPTY);
	cr_assert_eq(record2->player, BLACK_PLAYER);
}

Test(match, turn_record_contains_captured_piece) {
	Move move = (Move) {(Position) {0, 6}, (Position) {0, 1}};
	bool result = match_append_turn_record(match, move);
	cr_assert(result);
	TurnRecord *record = NULL;
	match_get_turn_record(match, 0, &record);
	cr_assert(move_eq(move, record->move));
	cr_assert_eq(record->turn, 0);
	cr_assert_eq(record->captured_piece.type, PAWN);
	cr_assert_eq(record->captured_piece.player, BLACK_PLAYER);
}

Test(match, history_contains_moves) {
	for (int i = 0; i < 7; i++) {
		Move move = (Move) {(Position) {0, i}, (Position) {0, i + 1}};
		bool result = match_append_turn_record(match, move);
		cr_assert(result);
		match_next_turn(match);
	}
	TurnHistory *history = NULL;
	match_get_history(match, &history);
	cr_assert(eq(uint, 7, history_size(history)));
	for (int i = 0; i < 7; i++) {
		TurnRecord *record = NULL;
		history_get(history, i, &record);
		cr_assert(move_eq((Move) {(Position) {0, i}, (Position) {0, i + 1}}, record->move));
		cr_assert(eq(int, record->turn, i));
	}
}

Test(match, undo_turn) {
	for (int i = 0; i < 7; i++) {
		Move move = (Move) {(Position) {0, i}, (Position) {0, i + 1}};
		bool result = match_append_turn_record(match, move);
		cr_assert(result);
		match_next_turn(match);
	}
	TurnHistory *history = NULL;
	match_get_history(match, &history);
	for (int i = 6; i > -1; i--) {
		TurnRecord *record = NULL;
		history_get(history, i, &record);
		cr_assert(move_eq((Move) {(Position) {0, i}, (Position) {0, i + 1}}, record->move));
		cr_assert(eq(int, record->turn, i));
		match_undo_turn(match);
	}
}
