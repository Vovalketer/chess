#ifndef ARRAY_H
#define ARRAY_H
#include <stdbool.h>
#include <stddef.h>

typedef struct Array Array;

bool array_create(Array** list, size_t elem_size);
bool array_create_with_config(Array** list, size_t elem_size, size_t initial_capacity, double growth_factor);
void array_destroy(Array** list);
void array_clear(Array* list);
bool array_insert(Array* list, size_t index, void* data);
bool array_prepend(Array* list, void* data);
bool array_append(Array* list, void* data);
bool array_pop(Array* list, size_t index, void* out_data);
bool array_pop_first(Array* list, void* out_data);
bool array_pop_last(Array* list, void* out_data);
bool array_remove(Array* list, size_t index);
bool array_remove_first(Array* list);
bool array_remove_last(Array* list);
bool array_get(Array* list, size_t index, void* out_data);
bool array_get_first(Array* list, void* out_data);
bool array_get_last(Array* list, void* out_data);
size_t array_size(Array* list);
bool array_clone(Array** dst, const Array* src);

#endif	// ARRAY_H
