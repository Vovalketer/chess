#include "array_tpl.h"

#include <stdio.h>

#include "criterion/criterion.h"
#include "criterion/new/assert.h"

typedef struct NestedTestStruct {
	size_t x;
} NestedTestStruct;

typedef struct TestStruct {
	size_t a;
	size_t b;
	NestedTestStruct nested;
} TestStruct;

ARRAY_DEFINE_TYPE(TestStruct, TestStruct_list)
TestStructList* list = NULL;

void setup(void) {
	cr_assert(TestStruct_list_create(&list));
}

void teardown(void) {
	TestStruct_list_destroy(&list);
}

TestSuite(array_template, .init = setup, .fini = teardown);

Test(array_template, adds_multiple_elements) {
	for (size_t i = 0; i < 10; i++) {
		TestStruct data = (TestStruct) {10 + i, 20 + i, (NestedTestStruct) {30 + i}};
		TestStruct_list_append(list, data);
	}
	size_t size = TestStruct_list_size(list);
	cr_assert_eq(size, 10);
}

Test(array_template, gets_elements) {
	for (size_t i = 0; i < 10; i++) {
		TestStruct data = (TestStruct) {10 + i, 20 + i, (NestedTestStruct) {30 + i}};
		TestStruct_list_append(list, data);
	}
	for (size_t i = 0; i < 10; i++) {
		TestStruct* data = NULL;
		TestStruct_list_get(list, i, &data);
		cr_assert(eq(uint, 10 + i, data->a));
		cr_assert(eq(uint, 20 + i, data->b));
		cr_assert(eq(uint, 30 + i, data->nested.x));
	}
}

Test(array_template, takes_elements) {
	for (size_t i = 0; i < 10; i++) {
		TestStruct data = (TestStruct) {10 + i, 20 + i, (NestedTestStruct) {30 + i}};
		TestStruct_list_append(list, data);
	}
	for (size_t i = 0; i < 10; i++) {
		TestStruct* data = NULL;
		TestStruct_list_pop_first(list, &data);
		cr_assert(eq(uint, 10 + i, data->a));
		cr_assert(eq(uint, 20 + i, data->b));
		cr_assert(eq(uint, 30 + i, data->nested.x));
		free(data);
	}
	cr_assert(eq(uint, 0, TestStruct_list_size(list)));
}

Test(array_template, removes_elements) {
	for (size_t i = 0; i < 10; i++) {
		TestStruct data = (TestStruct) {10 + i, 20 + i, (NestedTestStruct) {30 + i}};
		TestStruct_list_append(list, data);
	}
	for (size_t i = 0; i < 10; i++) {
		cr_assert(TestStruct_list_remove_last(list));
		cr_assert(eq(uint, 9 - i, TestStruct_list_size(list)));
	}
	cr_assert(eq(uint, 0, TestStruct_list_size(list)));
}

Test(array_template, clear) {
	for (size_t i = 0; i < 10; i++) {
		TestStruct data = (TestStruct) {10 + i, 20 + i, (NestedTestStruct) {30 + i}};
		TestStruct_list_append(list, data);
	}
	TestStruct_list_clear(list);
	cr_assert(eq(uint, 0, TestStruct_list_size(list)));
}
