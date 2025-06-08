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
