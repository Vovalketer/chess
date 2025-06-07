#include "array.h"

#include <stdio.h>

#include "criterion/criterion.h"
#include "criterion/new/assert.h"
bool _equals_int(void* a, void* b);
Array* list = NULL;

void setup(void) {
	cr_assert(array_create(&list, sizeof(int)));
}

void teardown(void) {
	array_destroy(&list);
}

bool _array_contains(Array* list, void* data, bool (*equals)(void*, void*)) {
	for (size_t i = 0; i < array_size(list); i++) {
		void* d = NULL;
		if (array_get(list, i, &d)) {
			if (equals(data, d)) {
				return true;
			}
		}
	}
	return false;
}

TestSuite(array, .init = setup, .fini = teardown);

Test(array, array_create) {
	cr_assert_not_null(list);
}

Test(array, array_prepend_one_element) {
	int data = 1;
	cr_assert(array_prepend(list, &data));
	cr_assert(eq(uint, array_size(list), 1));
}

Test(array, array_size) {
	for (size_t i = 0; i < 10; i++) {
		int data = i;
		cr_assert(array_append(list, &data));
	}
	cr_assert_eq(array_size(list), 10);
}

Test(array, array_prepend_many_elements) {
	for (size_t i = 0; i < 10; i++) {
		int data = i;
		cr_assert(array_prepend(list, &data));
	}
	cr_assert(eq(uint, array_size(list), 10));
}

Test(array, array_append_one_element) {
	int data = 1;
	cr_assert(array_append(list, &data));
	cr_assert(eq(uint, array_size(list), 1));
}

Test(array, array_append_many_elements) {
	for (size_t i = 0; i < 10; i++) {
		int data = i;
		cr_assert(array_append(list, &data));
	}
	cr_assert(eq(uint, array_size(list), 10));
}

Test(array, array_insert) {
	for (size_t i = 0; i < 10; i++) {
		int data = i;
		cr_assert(array_insert(list, i, &data));
	}
	cr_assert(eq(uint, array_size(list), 10));

	int d = 14;
	cr_assert(array_insert(list, 4, &d));

	void* data = NULL;
	cr_assert(array_get(list, 4, &data));
	cr_assert(eq(uint, *(int*) data, 14));
	// check neighbours
	data = NULL;
	cr_assert(array_get(list, 3, &data));
	cr_assert(eq(uint, *(int*) data, 3));
	data = NULL;
	cr_assert(array_get(list, 5, &data));
	cr_assert(eq(uint, *(int*) data, 4));
}

Test(array, array_get_last_element) {
	for (size_t i = 0; i < 10; i++) {
		int data = i;
		cr_assert(array_append(list, &data));
	}

	void* data = NULL;
	cr_assert(array_get(list, 9, &data));
	cr_assert(eq(uint, *(int*) data, 9));
}

Test(array, array_get_element_can_be_mutated) {
	for (size_t i = 0; i < 10; i++) {
		int data = i;
		cr_assert(array_append(list, &data));
	}

	void* data = NULL;
	cr_assert(array_get(list, 4, &data));
	*(int*) data = 100;

	void* data2 = NULL;
	cr_assert(array_get(list, 4, &data2));
	cr_assert(eq(uint, *(int*) data2, 100));
}

bool _equals_int(void* a, void* b) {
	return *(int*) a == *(int*) b;
}

// Test(array, array_contains) {
// 	for (size_t i = 0; i < 10; i++) {
// 		int data = i;
// 		cr_assert(array_append(list, &data));
// 	}
//
// 	int d = 8;
// 	cr_assert(array_contains(list, &d, equals_int));
// 	d = 100;
// 	cr_assert_not(array_contains(list, &d, equals_int));
// }

Test(array, array_remove) {
	for (size_t i = 0; i < 10; i++) {
		int data = i;
		cr_assert(array_append(list, &data));
	}

	void* data;
	cr_assert(array_pop(list, 4, &data));
	cr_assert(eq(uint, array_size(list), 9));
	cr_assert(eq(uint, *(int*) data, 4));

	// check the removed object is not present in the list
	cr_assert_not(_array_contains(list, data, _equals_int));

	free(data);
}

Test(array, array_remove_first) {
	for (size_t i = 0; i < 10; i++) {
		int data = i;
		cr_assert(array_append(list, &data));
	}

	void* data = NULL;
	cr_assert(array_pop_first(list, &data));
	cr_assert(eq(uint, array_size(list), 9));
	cr_assert(eq(uint, *(int*) data, 0));

	// check the removed object is not present in the list
	cr_assert_not(_array_contains(list, data, _equals_int));

	free(data);
}

Test(array, array_remove_last) {
	for (size_t i = 0; i < 10; i++) {
		int data = i;
		cr_assert(array_append(list, &data));
	}

	void* data = NULL;
	cr_assert(array_pop_last(list, &data));
	cr_assert(eq(uint, array_size(list), 9));
	cr_assert(eq(uint, *(int*) data, 9));

	// check the removed object is not present in the list
	cr_assert_not(_array_contains(list, data, _equals_int));

	free(data);
}

Test(array, array_remove_first_all_elements) {
	for (size_t i = 0; i < 10; i++) {
		int data = i;
		cr_assert(array_append(list, &data));
	}
	for (size_t i = 0; i < 10; i++) {
		void* data = NULL;
		cr_assert(array_pop_first(list, &data));
		cr_assert(eq(uint, array_size(list), 9 - i));
		cr_assert_not_null(data);
		cr_assert(eq(uint, *(int*) data, i));
		free(data);
	}
}

Test(array, array_remove_last_all_elements) {
	for (size_t i = 0; i < 10; i++) {
		int data = i;
		cr_assert(array_append(list, &data));
	}
	for (size_t i = 0; i < 10; i++) {
		void* data = NULL;
		cr_assert(array_pop_last(list, &data));
		cr_assert(eq(uint, array_size(list), 9 - i));
		cr_assert_not_null(data);
		cr_assert(eq(uint, *(int*) data, 9 - i));
		free(data);
	}
}

Test(array, array_delete_at) {
	for (size_t i = 0; i < 10; i++) {
		int data = i;
		cr_assert(array_append(list, &data));
	}

	int index_to_delete = 4;
	cr_assert(array_remove(list, index_to_delete));
	cr_assert(eq(uint, array_size(list), 9));

	// check the removed object is not present in the list

	int data = index_to_delete;
	cr_assert_not(_array_contains(list, &data, _equals_int));
}

Test(array, array_delete_first) {
	for (size_t i = 0; i < 10; i++) {
		int data = i;
		cr_assert(array_append(list, &data));
	}

	cr_assert(array_remove_first(list));
	cr_assert(eq(uint, array_size(list), 9));

	// check the removed object is not present in the list
	int data = 0;
	cr_assert_not(_array_contains(list, &data, _equals_int));
}

Test(array, array_delete_last) {
	for (size_t i = 0; i < 10; i++) {
		int data = i;
		cr_assert(array_append(list, &data));
	}

	cr_assert(array_remove_last(list));
	cr_assert(eq(uint, array_size(list), 9));

	// check the removed object is not present in the list
	int data = 9;
	cr_assert_not(_array_contains(list, &data, _equals_int));
}

Test(array, array_clear) {
	for (size_t i = 0; i < 10; i++) {
		int data = i;
		cr_assert(array_append(list, &data));
	}

	array_clear(list);
	cr_assert(eq(uint, array_size(list), 0));
}

Test(array, array_can_insert_after_clear) {
	for (size_t i = 0; i < 10; i++) {
		int data = i;
		cr_assert(array_append(list, &data));
	}
	array_clear(list);
	int data_to_add = 10;
	cr_assert(array_append(list, &data_to_add));
	cr_assert(eq(uint, array_size(list), 1));

	void* data = NULL;
	array_get(list, 0, &data);
	cr_assert(eq(int, *(int*) data, 10));
}

Test(array, array_get_out_of_bounds) {
	for (size_t i = 0; i < 10; i++) {
		int data = i;
		cr_assert(array_append(list, &data));
	}

	void* data = NULL;
	cr_assert_not(array_get(list, 10, &data));
	cr_assert_null(data);
}

Test(array, array_get_negative_index) {
	for (size_t i = 0; i < 10; i++) {
		int data = i;
		cr_assert(array_append(list, &data));
	}

	void* data = NULL;
	cr_assert_not(array_get(list, -1, &data));
	cr_assert_null(data);
}

Test(array, array_insert_out_of_bounds) {
	for (size_t i = 0; i < 10; i++) {
		int data = i;
		cr_assert(array_append(list, &data));
	}

	int data = 10;
	cr_assert_not(array_insert(list, 11, &data));
}

Test(array, array_insert_negative_index) {
	for (size_t i = 0; i < 10; i++) {
		int data = i;
		cr_assert(array_append(list, &data));
	}

	int data = 10;
	cr_assert_not(array_insert(list, -1, &data));
}

Test(array, array_remove_out_of_bounds) {
	for (size_t i = 0; i < 10; i++) {
		int data = i;
		cr_assert(array_append(list, &data));
	}

	void* data = NULL;
	cr_assert_not(array_pop(list, 10, &data));
	cr_assert_null(data);
}

Test(array, array_remove_negative_index) {
	for (size_t i = 0; i < 10; i++) {
		int data = i;
		cr_assert(array_append(list, &data));
	}

	void* data = NULL;
	cr_assert_not(array_pop(list, -1, &data));
	cr_assert_null(data);
}

Test(array, array_delete_at_out_of_bounds) {
	for (size_t i = 0; i < 10; i++) {
		int data = i;
		cr_assert(array_append(list, &data));
	}

	cr_assert_not(array_remove(list, 10));
}

Test(array, array_delete_at_negative_index) {
	for (size_t i = 0; i < 10; i++) {
		int data = i;
		cr_assert(array_append(list, &data));
	}

	cr_assert_not(array_remove(list, -1));
}
