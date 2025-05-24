#include "../include/move.h"

#include <assert.h>

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
	cr_assert(eq(uint, moves->size, 0));
}

Test(moves, create_moves_list_has_correct_capacity, .init = setup, .fini = teardown) {
	cr_assert(eq(uint, moves->_capacity, 8));
}

Test(moves, add_move_adds_move_to_list, .init = setup, .fini = teardown) {
	cr_assert(eq(uint, moves->size, 0));
	bool added = move_list_add(&moves, move_create(1, 1, 1, 1));
	cr_assert_eq(added, true);
	cr_assert(eq(uint, moves->size, 1));
}

Test(moves, get_moves_list_size_returns_correct_size, .init = setup, .fini = teardown) {
	cr_assert(eq(uint, move_list_size(moves), 0));
	move_list_add(&moves, move_create(1, 1, 1, 1));
	cr_assert(eq(uint, move_list_size(moves), 1));
}

Test(moves, clear_list_removes_all_moves, .init = setup, .fini = teardown) {
	move_list_add(&moves, move_create(1, 1, 1, 1));
	move_list_add(&moves, move_create(2, 2, 2, 2));
	move_list_add(&moves, move_create(3, 3, 3, 3));
	move_list_clear(moves);
	cr_assert(eq(uint, moves->size, 0));
	cr_assert_not_null(moves);
}

Test(moves, is_move_in_list_returns_true_for_move_in_list, .init = setup, .fini = teardown) {
	move_list_add(&moves, move_create(1, 1, 1, 1));
	move_list_add(&moves, move_create(2, 2, 2, 2));
	move_list_add(&moves, move_create(3, 3, 3, 3));
	bool is_present = move_list_contains(moves, move_create(2, 2, 2, 2));
	cr_assert(is_present);
}

Test(moves, is_move_in_list_returns_false_for_move_not_in_list, .init = setup, .fini = teardown) {
	move_list_add(&moves, move_create(1, 1, 1, 1));
	bool is_present = move_list_contains(moves, move_create(2, 2, 2, 2));
	cr_assert_not(is_present);
}

Test(moves, get_move_at_index_returns_correct_move, .init = setup, .fini = teardown) {
	move_list_add(&moves, move_create(1, 1, 1, 1));
	move_list_add(&moves, move_create(2, 2, 2, 2));
	move_list_add(&moves, move_create(3, 3, 3, 3));
	Move move = move_list_get(moves, 1);
	cr_assert_eq(move.x_src, 2);
	cr_assert_eq(move.y_src, 2);
	cr_assert_eq(move.x_dest, 2);
	cr_assert_eq(move.y_dest, 2);
}

Test(moves, remove_move_at_index_removes_item_at_the_end, .init = setup, .fini = teardown) {
	move_list_add(&moves, move_create(1, 1, 1, 1));
	move_list_add(&moves, move_create(2, 2, 2, 2));
	move_list_add(&moves, move_create(3, 3, 3, 3));
	move_list_remove(moves, 2);
	cr_assert(eq(uint, move_list_size(moves), 2));
	bool is_present = move_list_contains(moves, move_create(3, 3, 3, 3));
	cr_assert_eq(is_present, false);
}

Test(moves, remove_move_at_index_removes_item_at_the_start, .init = setup, .fini = teardown) {
	move_list_add(&moves, move_create(1, 1, 1, 1));
	move_list_add(&moves, move_create(2, 2, 2, 2));
	move_list_add(&moves, move_create(3, 3, 3, 3));
	move_list_remove(moves, 0);
	cr_assert(eq(uint, move_list_size(moves), 2));
	bool is_present = move_list_contains(moves, move_create(1, 1, 1, 1));
	cr_assert_eq(is_present, false);
}

Test(moves, move_list_resizes_correctly, .init = setup, .fini = teardown) {
	move_list_add(&moves, move_create(1, 1, 1, 1));
	move_list_add(&moves, move_create(2, 2, 2, 2));
	move_list_add(&moves, move_create(3, 3, 3, 3));
	move_list_add(&moves, move_create(4, 4, 4, 4));
	move_list_add(&moves, move_create(5, 5, 5, 5));
	move_list_add(&moves, move_create(6, 6, 6, 6));
	move_list_add(&moves, move_create(7, 7, 7, 7));
	move_list_add(&moves, move_create(8, 8, 8, 8));
	move_list_add(&moves, move_create(9, 9, 9, 9));
	move_list_add(&moves, move_create(10, 10, 10, 10));
	move_list_add(&moves, move_create(11, 11, 11, 11));
	move_list_add(&moves, move_create(12, 12, 12, 12));
	move_list_add(&moves, move_create(13, 13, 13, 13));
	move_list_add(&moves, move_create(14, 14, 14, 14));
	move_list_add(&moves, move_create(15, 15, 15, 15));
	move_list_add(&moves, move_create(16, 16, 16, 16));
	move_list_add(&moves, move_create(17, 17, 17, 17));
	cr_assert_eq(move_list_size(moves), 17);
	for (int i = 0; i < 17; i++) {
		Move m = move_list_get(moves, i);
		cr_assert_eq(m.x_src, i + 1);
		cr_assert_eq(m.y_src, i + 1);
		cr_assert_eq(m.x_dest, i + 1);
		cr_assert_eq(m.y_dest, i + 1);
	}
}
