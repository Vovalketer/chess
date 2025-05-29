#include "../include/move.h"

#include <assert.h>

#include "../include/movelist.h"
#include "criterion/criterion.h"
#include "criterion/new/assert.h"

MoveList *moves;

void setup(void) {
	move_list_create(&moves);
}

void teardown(void) {
	move_list_destroy(&moves);
}

Test(moves, create_moves_list_succeeds) {
	MoveList *_moves;
	bool success = move_list_create(&_moves);
	cr_assert_eq(success, true);
	move_list_destroy(&_moves);
}

Test(moves, destroy_moves_list_makes_list_null) {
	MoveList *_moves;
	bool success = move_list_create(&_moves);
	cr_assert_eq(success, true);
	move_list_destroy(&_moves);
	cr_assert_null(_moves);
}

Test(moves, create_moves_list_has_correct_size, .init = setup, .fini = teardown) {
	cr_assert(eq(uint, move_list_size(moves), 0));
}

Test(moves, add_move_adds_move_to_list, .init = setup, .fini = teardown) {
	cr_assert(eq(uint, move_list_size(moves), 0));
	bool added = move_list_add(moves, move_create((Position) {1, 1}, (Position) {1, 1}));
	cr_assert_eq(added, true);
	cr_assert(eq(uint, move_list_size(moves), 1));
}

Test(moves, get_moves_list_size_returns_correct_size, .init = setup, .fini = teardown) {
	cr_assert(eq(uint, move_list_size(moves), 0));
	move_list_add(moves, move_create((Position) {1, 1}, (Position) {1, 1}));
	cr_assert(eq(uint, move_list_size(moves), 1));
}

Test(moves, clear_list_removes_all_moves, .init = setup, .fini = teardown) {
	for (int i = 1; i < 4; i++) {
		move_list_add(moves, move_create((Position) {i, i}, (Position) {i, i}));
	}
	move_list_clear(moves);
	cr_assert(eq(uint, move_list_size(moves), 0));
	cr_assert_not_null(moves);
}

Test(moves, is_move_in_list_returns_true_for_move_in_list, .init = setup, .fini = teardown) {
	for (int i = 1; i < 4; i++) {
		move_list_add(moves, move_create((Position) {i, i}, (Position) {i, i}));
	}
	bool is_present = move_list_contains(moves, move_create((Position) {2, 2}, (Position) {2, 2}));
	cr_assert(is_present);
}

Test(moves, is_move_in_list_returns_false_for_move_not_in_list, .init = setup, .fini = teardown) {
	move_list_add(moves, move_create((Position) {1, 1}, (Position) {1, 1}));
	bool is_present = move_list_contains(moves, move_create((Position) {2, 2}, (Position) {2, 2}));
	cr_assert_not(is_present);
}

Test(moves, get_move_at_index_returns_correct_move, .init = setup, .fini = teardown) {
	for (int i = 1; i < 4; i++) {
		move_list_add(moves, move_create((Position) {i, i}, (Position) {i, i}));
	}
	Move move = move_list_get(moves, 1);
	cr_assert_eq(move.src.x, 2);
	cr_assert_eq(move.src.y, 2);
	cr_assert_eq(move.dst.x, 2);
	cr_assert_eq(move.dst.y, 2);
}

Test(moves, remove_move_at_index_removes_item_at_the_end, .init = setup, .fini = teardown) {
	for (int i = 1; i < 4; i++) {
		move_list_add(moves, move_create((Position) {i, i}, (Position) {i, i}));
	}
	move_list_remove(moves, 2);
	cr_assert(eq(uint, move_list_size(moves), 2));
	bool is_present = move_list_contains(moves, move_create((Position) {3, 3}, (Position) {3, 3}));
	cr_assert_eq(is_present, false);
}

Test(moves, remove_move_at_index_removes_item_at_the_start, .init = setup, .fini = teardown) {
	for (int i = 1; i < 4; i++) {
		move_list_add(moves, move_create((Position) {i, i}, (Position) {i, i}));
	}
	move_list_remove(moves, 0);
	cr_assert(eq(uint, move_list_size(moves), 2));
	bool is_present = move_list_contains(moves, move_create((Position) {1, 1}, (Position) {1, 1}));
	cr_assert_eq(is_present, false);
}

Test(moves, move_list_resizes_correctly, .init = setup, .fini = teardown) {
	for (int i = 0; i < 17; i++) {
		move_list_add(moves, move_create((Position) {i, i}, (Position) {i, i}));
	}
	cr_assert_eq(move_list_size(moves), 17);
	for (int i = 0; i < 17; i++) {
		Move m = move_list_get(moves, i);
		cr_assert_eq(m.src.x, i);
		cr_assert_eq(m.src.y, i);
		cr_assert_eq(m.dst.x, i);
		cr_assert_eq(m.dst.y, i);
	}
}

Test(moves, pop_move_from_list_removes_item_at_the_end, .init = setup, .fini = teardown) {
	for (int i = 0; i < 4; i++) {
		move_list_add(moves, move_create((Position) {i, i}, (Position) {i, i}));
	}
	cr_assert(eq(uint, move_list_size(moves), 4));
	Move pop = move_list_pop(moves);
	cr_assert(eq(int, pop.src.x, 3));
	cr_assert(eq(int, pop.src.y, 3));
	cr_assert(eq(int, pop.dst.x, 3));
	cr_assert(eq(int, pop.dst.y, 3));
	cr_assert(eq(uint, move_list_size(moves), 3));
	bool is_present = move_list_contains(moves, move_create((Position) {3, 3}, (Position) {3, 3}));
	cr_assert_eq(is_present, false);
}
