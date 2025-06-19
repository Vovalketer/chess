#ifndef ARRAY_TPL_H
#define ARRAY_TPL_H

#include <stdbool.h>
#include <stdlib.h>

#include "array.h"
#define _ARRAY_NAME(CONTAINER_NAME) CONTAINER_NAME

// helps to silence the unused header warning
static inline void _free_wrapper(void* wrapper) {
	free(wrapper);
}

#define ARRAY_DEFINE_TYPE(TYPE, CONTAINER_NAME, FN_PREFIX)                                                   \
                                                                                                             \
	typedef struct {                                                                                         \
		Array* raw;                                                                                          \
	} _ARRAY_NAME(CONTAINER_NAME);                                                                           \
                                                                                                             \
	__attribute__((unused)) static inline bool FN_PREFIX##_create(_ARRAY_NAME(CONTAINER_NAME) * *list) {     \
		_ARRAY_NAME(CONTAINER_NAME)* tmp = malloc(sizeof(_ARRAY_NAME(CONTAINER_NAME)));                      \
		if (!tmp)                                                                                            \
			return false;                                                                                    \
		tmp->raw = NULL;                                                                                     \
		*list = tmp;                                                                                         \
		return array_create(&(*list)->raw, sizeof(TYPE));                                                    \
	}                                                                                                        \
                                                                                                             \
	__attribute__((unused)) static inline bool FN_PREFIX##_create_with_config(                               \
		_ARRAY_NAME(CONTAINER_NAME) * *list, size_t initial_capacity, double growth_factor) {                \
		_ARRAY_NAME(CONTAINER_NAME)* tmp = malloc(sizeof(_ARRAY_NAME(CONTAINER_NAME)));                      \
		if (!tmp)                                                                                            \
			return false;                                                                                    \
		tmp->raw = NULL;                                                                                     \
		*list = tmp;                                                                                         \
		return array_create_with_config(&(*list)->raw, sizeof(TYPE), initial_capacity, growth_factor);       \
	}                                                                                                        \
                                                                                                             \
	__attribute__((unused)) static inline void FN_PREFIX##_destroy(_ARRAY_NAME(CONTAINER_NAME) * *list) {    \
		if (!list || !*list)                                                                                 \
			return;                                                                                          \
		array_destroy(&(*list)->raw);                                                                        \
		_free_wrapper(*list);                                                                                \
		*list = NULL;                                                                                        \
	}                                                                                                        \
                                                                                                             \
	__attribute__((unused)) static inline void FN_PREFIX##_clear(_ARRAY_NAME(CONTAINER_NAME) * list) {       \
		array_clear(list->raw);                                                                              \
	}                                                                                                        \
                                                                                                             \
	__attribute__((unused)) static inline size_t FN_PREFIX##_size(_ARRAY_NAME(CONTAINER_NAME) * list) {      \
		return array_size(list->raw);                                                                        \
	}                                                                                                        \
                                                                                                             \
	__attribute__((unused)) static inline bool FN_PREFIX##_insert(                                           \
		_ARRAY_NAME(CONTAINER_NAME) * list, size_t index, TYPE value) {                                      \
		return array_insert(list->raw, index, &value);                                                       \
	}                                                                                                        \
                                                                                                             \
	__attribute__((unused)) static inline bool FN_PREFIX##_prepend(_ARRAY_NAME(CONTAINER_NAME) * list,       \
																   TYPE value) {                             \
		return array_prepend(list->raw, &value);                                                             \
	}                                                                                                        \
                                                                                                             \
	__attribute__((unused)) static inline bool FN_PREFIX##_append(_ARRAY_NAME(CONTAINER_NAME) * list,        \
																  TYPE value) {                              \
		return array_append(list->raw, &value);                                                              \
	}                                                                                                        \
                                                                                                             \
	__attribute__((unused)) static inline bool FN_PREFIX##_pop(                                              \
		_ARRAY_NAME(CONTAINER_NAME) * list, size_t index, TYPE** out) {                                      \
		void* tmp = NULL;                                                                                    \
		if (!array_pop(list->raw, index, &tmp))                                                              \
			return false;                                                                                    \
		*out = (TYPE*) tmp;                                                                                  \
		return true;                                                                                         \
	}                                                                                                        \
                                                                                                             \
	__attribute__((unused)) static inline bool FN_PREFIX##_pop_first(_ARRAY_NAME(CONTAINER_NAME) * list,     \
																	 TYPE * *out) {                          \
		void* tmp = NULL;                                                                                    \
		if (!array_pop_first(list->raw, &tmp))                                                               \
			return false;                                                                                    \
		*out = (TYPE*) tmp;                                                                                  \
		return true;                                                                                         \
	}                                                                                                        \
                                                                                                             \
	__attribute__((unused)) static inline bool FN_PREFIX##_pop_last(_ARRAY_NAME(CONTAINER_NAME) * list,      \
																	TYPE * *out) {                           \
		void* tmp = NULL;                                                                                    \
		if (!array_pop_last(list->raw, &tmp))                                                                \
			return false;                                                                                    \
		*out = (TYPE*) tmp;                                                                                  \
		return true;                                                                                         \
	}                                                                                                        \
                                                                                                             \
	__attribute__((unused)) static inline bool FN_PREFIX##_remove(_ARRAY_NAME(CONTAINER_NAME) * list,        \
																  size_t index) {                            \
		return array_remove(list->raw, index);                                                               \
	}                                                                                                        \
                                                                                                             \
	__attribute__((unused)) static inline bool FN_PREFIX##_remove_first(_ARRAY_NAME(CONTAINER_NAME) *        \
																		list) {                              \
		return array_remove_first(list->raw);                                                                \
	}                                                                                                        \
                                                                                                             \
	__attribute__((unused)) static inline bool FN_PREFIX##_remove_last(_ARRAY_NAME(CONTAINER_NAME) * list) { \
		return array_remove_last(list->raw);                                                                 \
	}                                                                                                        \
                                                                                                             \
	__attribute__((unused)) static inline bool FN_PREFIX##_get(                                              \
		_ARRAY_NAME(CONTAINER_NAME) * list, size_t index, TYPE** out) {                                      \
		void* tmp = NULL;                                                                                    \
		if (!array_get(list->raw, index, &tmp))                                                              \
			return false;                                                                                    \
		*out = (TYPE*) tmp;                                                                                  \
		return true;                                                                                         \
	}                                                                                                        \
                                                                                                             \
	__attribute__((unused)) static inline bool FN_PREFIX##_get_first(_ARRAY_NAME(CONTAINER_NAME) * list,     \
																	 TYPE * *out) {                          \
		void* tmp = NULL;                                                                                    \
		if (!array_get_first(list->raw, &tmp))                                                               \
			return false;                                                                                    \
		*out = (TYPE*) tmp;                                                                                  \
		return true;                                                                                         \
	}                                                                                                        \
                                                                                                             \
	__attribute__((unused)) static inline bool FN_PREFIX##_get_last(_ARRAY_NAME(CONTAINER_NAME) * list,      \
																	TYPE * *out) {                           \
		void* tmp = NULL;                                                                                    \
		if (!array_get_last(list->raw, &tmp))                                                                \
			return false;                                                                                    \
		*out = (TYPE*) tmp;                                                                                  \
		return true;                                                                                         \
	}                                                                                                        \
                                                                                                             \
	__attribute__((unused)) static inline bool FN_PREFIX##_clone(_ARRAY_NAME(CONTAINER_NAME) * *dst,         \
																 const _ARRAY_NAME(CONTAINER_NAME) * src) {  \
		*dst = malloc(sizeof **dst);                                                                         \
		if (!*dst)                                                                                           \
			return false;                                                                                    \
		return array_clone(&(*dst)->raw, src->raw);                                                          \
	}

#endif	// ARRAY_TPL_H
