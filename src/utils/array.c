#include "array.h"

#include <assert.h>
#include <math.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define DEFAULT_ARRAY_INITIAL_CAPACITY 16
#define DEFAULT_ARRAY_GROWTH_FACTOR 2

struct Array {
	size_t size;
	size_t capacity;
	size_t elem_size;
	double growth_factor;
	uint8_t* data;
};

bool array_create_with_config(Array** list, size_t elem_size, size_t capacity, double growth_factor) {
	assert(list != NULL);
	assert(capacity > 0);
	assert(growth_factor > 1);
	Array* arr = malloc(sizeof(Array));
	if (arr == NULL) {
		return false;
	}

	void* l = malloc(elem_size * capacity);
	if (l == NULL) {
		free(arr);
		return false;
	}

	arr->data = l;
	arr->size = 0;
	arr->capacity = capacity;
	arr->elem_size = elem_size;
	arr->growth_factor = growth_factor;

	*list = arr;

	return true;
}

bool array_create(Array** list, size_t elem_size) {
	return array_create_with_config(
		list, elem_size, DEFAULT_ARRAY_INITIAL_CAPACITY, DEFAULT_ARRAY_GROWTH_FACTOR);
}

void array_destroy(Array** list) {
	if (list && *list != NULL) {
		free((*list)->data);
		free(*list);
		*list = NULL;
	}
}

void array_clear(Array* list) {
	assert(list != NULL);
	list->size = 0;
}

bool array_insert(Array* list, size_t index, void* data) {
	assert(list != NULL);
	assert(data != NULL);
	if (index > list->size) {
		return false;
	}

	if (list->size == list->capacity) {
		size_t new_cap = (size_t) ceil(list->capacity * list->growth_factor);
		// avoid growth = 0 if initial_capacity is low and the multiplication rounds to 0
		if (new_cap <= list->capacity) {
			new_cap = list->capacity + 1;
		}
		void* r = realloc(list->data, new_cap * list->elem_size);
		if (r == NULL) {
			return false;
		}
		list->capacity = new_cap;
		list->data = r;
	}
	// shift data to the right
	// size * element_size = total size of the array
	// size - index = number of elements to move

	// base is the start of the array, casted to uint8_t to avoid undefined behavior
	uint8_t* base = list->data;
	size_t move_size = (list->size - index) * list->elem_size;
	if (move_size > 0) {
		uint8_t* src = base + index * list->elem_size;
		uint8_t* dst = base + (index + 1) * list->elem_size;
		memmove(dst, src, move_size);
	}
	memcpy(base + index * list->elem_size, data, list->elem_size);
	list->size++;
	return true;
}

bool array_prepend(Array* list, void* data) {
	return array_insert(list, 0, data);
}

bool array_append(Array* list, void* data) {
	return array_insert(list, list->size, data);
}

bool array_pop(Array* list, size_t index, void** out_data) {
	assert(list != NULL);
	if (index >= list->size) {
		return false;
	}

	void* out = malloc(list->elem_size);
	if (out == NULL) {
		return false;
	}

	// base is the start of the array, casted to uint8_t to avoid undefined behavior
	uint8_t* base = list->data;
	if (out_data != NULL) {
		memcpy(out, base + index * list->elem_size, list->elem_size);
		*out_data = out;
	}

	size_t move_size = (list->size - index - 1) * list->elem_size;
	if (move_size > 0) {
		uint8_t* src = base + (index + 1) * list->elem_size;
		uint8_t* dst = base + index * list->elem_size;
		memmove(dst, src, move_size);
	}
	list->size--;
	return true;
}

bool array_pop_first(Array* list, void** out_data) {
	return array_pop(list, 0, out_data);
}

bool array_pop_last(Array* list, void** out_data) {
	return array_pop(list, list->size - 1, out_data);
}

bool array_remove(Array* list, size_t index) {
	return array_pop(list, index, NULL);
}

bool array_remove_first(Array* list) {
	return array_remove(list, 0);
}

bool array_remove_last(Array* list) {
	return array_remove(list, list->size - 1);
}

bool array_get(Array* list, size_t index, void** out_data) {
	assert(list != NULL);
	assert(out_data != NULL);
	if (index >= list->size) {
		return false;
	}

	// base is the start of the array, casted to uint8_t to avoid undefined behavior
	uint8_t* base = list->data;
	*out_data = base + index * list->elem_size;

	return true;
}

bool array_get_first(Array* list, void** out_data) {
	return array_get(list, 0, out_data);
}

bool array_get_last(Array* list, void** out_data) {
	return array_get(list, array_size(list) - 1, out_data);
}

size_t array_size(Array* list) {
	return list->size;
}

bool array_clone(Array** dst, const Array* src) {
	assert(dst != NULL);
	assert(src != NULL);
	Array* b;
	bool result = array_create(&b, src->elem_size);
	if (!result) {
		return false;
	}
	memcpy(b->data, src->data, src->elem_size * src->size);
	b->size = src->size;
	b->capacity = src->capacity;
	b->elem_size = src->elem_size;
	b->growth_factor = src->growth_factor;
	*dst = b;
	return true;
}
