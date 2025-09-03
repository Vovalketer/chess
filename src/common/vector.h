#ifndef VECTOR_H
#define VECTOR_H

#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#ifndef VECTOR_GROWTH_FACTOR
#define VECTOR_GROWTH_FACTOR 2
#endif

#define VECTOR_DEFINE_TYPE(TYPE, CONTAINER_NAME, FN_PREFIX)                                        \
	typedef struct {                                                                               \
		size_t size;                                                                               \
		size_t capacity;                                                                           \
		TYPE  *data;                                                                               \
	} CONTAINER_NAME;                                                                              \
                                                                                                   \
	__attribute__((unused)) static void FN_PREFIX##_init(CONTAINER_NAME *v) {                      \
		v->data		= NULL;                                                                        \
		v->size		= 0;                                                                           \
		v->capacity = 0;                                                                           \
	}                                                                                              \
                                                                                                   \
	__attribute__((unused)) static bool FN_PREFIX##_reserve(CONTAINER_NAME *v, size_t new_cap) {   \
		if (new_cap <= v->capacity)                                                                \
			return true;                                                                           \
		TYPE *new_data = realloc(v->data, new_cap * sizeof(TYPE));                                 \
		if (!new_data)                                                                             \
			return false;                                                                          \
		v->data		= new_data;                                                                    \
		v->capacity = new_cap;                                                                     \
		return true;                                                                               \
	}                                                                                              \
                                                                                                   \
	__attribute__((unused)) static bool FN_PREFIX##_init_reserve(CONTAINER_NAME *v, size_t cap) {  \
		FN_PREFIX##_init(v);                                                                       \
		return FN_PREFIX##_reserve(v, cap);                                                        \
	}                                                                                              \
                                                                                                   \
	__attribute__((unused)) static CONTAINER_NAME *FN_PREFIX##_create(void) {                      \
		CONTAINER_NAME *v = malloc(sizeof(CONTAINER_NAME));                                        \
		FN_PREFIX##_init(v);                                                                       \
		FN_PREFIX##_reserve(v, 8);                                                                 \
		return v;                                                                                  \
	}                                                                                              \
                                                                                                   \
	__attribute__((unused)) static void FN_PREFIX##_free(CONTAINER_NAME *v) {                      \
		if (v->data)                                                                               \
			free(v->data);                                                                         \
		v->data		= NULL;                                                                        \
		v->size		= 0;                                                                           \
		v->capacity = 0;                                                                           \
	}                                                                                              \
                                                                                                   \
	__attribute__((unused)) static void FN_PREFIX##_destroy(CONTAINER_NAME **v) {                  \
		FN_PREFIX##_free(*v);                                                                      \
		free(*v);                                                                                  \
		*v = NULL;                                                                                 \
	}                                                                                              \
                                                                                                   \
	__attribute__((unused)) static bool FN_PREFIX##_insert(                                        \
		CONTAINER_NAME *v, size_t index, TYPE value) {                                             \
		if (index > v->size)                                                                       \
			return false;                                                                          \
		if (v->size == v->capacity) {                                                              \
			size_t new_cap = v->capacity ? v->capacity * VECTOR_GROWTH_FACTOR : 8;                 \
			if (!FN_PREFIX##_reserve(v, new_cap))                                                  \
				return false;                                                                      \
		}                                                                                          \
		memmove(&v->data[index + 1], &v->data[index], (v->size - index) * sizeof(TYPE));           \
		v->data[index] = value;                                                                    \
		v->size++;                                                                                 \
		return true;                                                                               \
	}                                                                                              \
                                                                                                   \
	__attribute__((unused)) static bool FN_PREFIX##_remove(CONTAINER_NAME *v, size_t index) {      \
		if (index >= v->size)                                                                      \
			return false;                                                                          \
		memmove(&v->data[index], &v->data[index + 1], (v->size - index - 1) * sizeof(TYPE));       \
		v->size--;                                                                                 \
		return true;                                                                               \
	}                                                                                              \
                                                                                                   \
	__attribute__((unused)) static inline void FN_PREFIX##_push_back(CONTAINER_NAME *v,            \
																	 TYPE			 value) {                 \
		if (v->size == v->capacity) {                                                              \
			size_t new_cap = v->capacity ? v->capacity * VECTOR_GROWTH_FACTOR : 8;                 \
			FN_PREFIX##_reserve(v, new_cap);                                                       \
		}                                                                                          \
		v->data[v->size++] = value;                                                                \
	}                                                                                              \
                                                                                                   \
	__attribute__((unused)) static inline void FN_PREFIX##_push_front(CONTAINER_NAME *v,           \
																	  TYPE			  value) {                \
		if (v->size == v->capacity) {                                                              \
			size_t new_cap = v->capacity ? v->capacity * VECTOR_GROWTH_FACTOR : 8;                 \
			FN_PREFIX##_reserve(v, new_cap);                                                       \
		}                                                                                          \
		memmove(v->data + 1, v->data, v->size * sizeof(TYPE));                                     \
		v->data[0] = value;                                                                        \
		v->size++;                                                                                 \
	}                                                                                              \
                                                                                                   \
	__attribute__((unused)) static inline TYPE FN_PREFIX##_pop_back(CONTAINER_NAME *v) {           \
		assert(v->size > 0 && "pop_back called on empty vector");                                  \
		return v->data[--v->size];                                                                 \
	}                                                                                              \
                                                                                                   \
	__attribute__((unused)) static inline TYPE FN_PREFIX##_pop_front(CONTAINER_NAME *v) {          \
		assert(v->size > 0 && "pop_front called on empty vector");                                 \
		TYPE value = v->data[0];                                                                   \
		memmove(v->data, v->data + 1, (v->size - 1) * sizeof(TYPE));                               \
		v->size--;                                                                                 \
		return value;                                                                              \
	}                                                                                              \
                                                                                                   \
	__attribute__((unused)) static inline TYPE *FN_PREFIX##_get(CONTAINER_NAME *v, size_t index) { \
		if (index >= v->size)                                                                      \
			return NULL;                                                                           \
		return &v->data[index];                                                                    \
	}                                                                                              \
                                                                                                   \
	__attribute__((unused)) static inline void FN_PREFIX##_sort(                                   \
		CONTAINER_NAME *v, int (*cmp)(const void *, const void *)) {                               \
		qsort(v->data, v->size, sizeof(TYPE), cmp);                                                \
	}                                                                                              \
                                                                                                   \
	__attribute__((unused)) static inline void FN_PREFIX##_clear(CONTAINER_NAME *v) {              \
		v->size = 0;                                                                               \
	}                                                                                              \
                                                                                                   \
	__attribute__((unused)) static inline size_t FN_PREFIX##_size(CONTAINER_NAME *v) {             \
		return v->size;                                                                            \
	}                                                                                              \
                                                                                                   \
	__attribute__((unused)) static inline bool FN_PREFIX##_empty(CONTAINER_NAME *v) {              \
		return v->size == 0;                                                                       \
	}                                                                                              \
                                                                                                   \
	__attribute__((unused)) static inline bool FN_PREFIX##_clone(CONTAINER_NAME *dst,              \
																 CONTAINER_NAME *src) {            \
		if (!FN_PREFIX##_reserve(dst, src->size))                                                  \
			return false;                                                                          \
		dst->data = memcpy(dst->data, src->data, sizeof(TYPE) * src->size);                        \
		dst->size = src->size;                                                                     \
		return true;                                                                               \
	}

#endif	// VECTOR_H
