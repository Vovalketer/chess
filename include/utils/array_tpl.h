#ifndef ARRAY_TPL_H
#define ARRAY_TPL_H

#include <stdbool.h>
#include <stdlib.h>	 //required for the internal malloc/free

#include "array.h"
#define _LIST_STRUCT(TYPE) TYPE##List
#define ARRAY_DEFINE_TYPE(TYPE, NAME)                                                                 \
                                                                                                          \
	typedef struct {                                                                                      \
		Array* raw;                                                                                       \
	} _LIST_STRUCT(TYPE);                                                                                 \
                                                                                                          \
	__attribute__((unused)) static inline bool NAME##_create(_LIST_STRUCT(TYPE) * *list) {                \
		_LIST_STRUCT(TYPE)* tmp = malloc(sizeof(_LIST_STRUCT(TYPE)));                                     \
		if (!tmp)                                                                                         \
			return false;                                                                                 \
		tmp->raw = NULL;                                                                                  \
		*list = tmp;                                                                                      \
		return array_create(&(*list)->raw, sizeof(TYPE));                                                 \
	}                                                                                                     \
                                                                                                          \
	__attribute__((unused)) static inline bool NAME##_create_with_config(                                 \
		_LIST_STRUCT(TYPE) * *list, size_t initial_capacity, double growth_factor) {                      \
		_LIST_STRUCT(TYPE)* tmp = malloc(sizeof(_LIST_STRUCT(TYPE)));                                     \
		if (!tmp)                                                                                         \
			return false;                                                                                 \
		tmp->raw = NULL;                                                                                  \
		*list = tmp;                                                                                      \
		return array_create_with_config(&(*list)->raw, sizeof(TYPE), initial_capacity, growth_factor);    \
	}                                                                                                     \
                                                                                                          \
	__attribute__((unused)) static inline void NAME##_destroy(_LIST_STRUCT(TYPE) * *list) {               \
		if (!list || !*list)                                                                              \
			return;                                                                                       \
		array_destroy(&(*list)->raw);                                                                     \
		free(*list);                                                                                      \
		*list = NULL;                                                                                     \
	}                                                                                                     \
                                                                                                          \
	__attribute__((unused)) static inline void NAME##_clear(_LIST_STRUCT(TYPE) * list) {                  \
		array_clear(list->raw);                                                                           \
	}                                                                                                     \
                                                                                                          \
	__attribute__((unused)) static inline size_t NAME##_size(_LIST_STRUCT(TYPE) * list) {                 \
		return array_size(list->raw);                                                                     \
	}                                                                                                     \
                                                                                                          \
	__attribute__((unused)) static inline bool NAME##_insert(                                             \
		_LIST_STRUCT(TYPE) * list, size_t index, TYPE value) {                                            \
		return array_insert(list->raw, index, &value);                                                    \
	}                                                                                                     \
                                                                                                          \
	__attribute__((unused)) static inline bool NAME##_prepend(_LIST_STRUCT(TYPE) * list, TYPE value) {    \
		return array_prepend(list->raw, &value);                                                          \
	}                                                                                                     \
                                                                                                          \
	__attribute__((unused)) static inline bool NAME##_append(_LIST_STRUCT(TYPE) * list, TYPE value) {     \
		return array_append(list->raw, &value);                                                           \
	}                                                                                                     \
                                                                                                          \
	__attribute__((unused)) static inline bool NAME##_pop(                                                \
		_LIST_STRUCT(TYPE) * list, size_t index, TYPE** out) {                                            \
		void* tmp = NULL;                                                                                 \
		if (!array_pop(list->raw, index, &tmp))                                                           \
			return false;                                                                                 \
		*out = (TYPE*) tmp;                                                                               \
		return true;                                                                                      \
	}                                                                                                     \
                                                                                                          \
	__attribute__((unused)) static inline bool NAME##_pop_first(_LIST_STRUCT(TYPE) * list, TYPE * *out) { \
		void* tmp = NULL;                                                                                 \
		if (!array_pop_first(list->raw, &tmp))                                                            \
			return false;                                                                                 \
		*out = (TYPE*) tmp;                                                                               \
		return true;                                                                                      \
	}                                                                                                     \
                                                                                                          \
	__attribute__((unused)) static inline bool NAME##_pop_last(_LIST_STRUCT(TYPE) * list, TYPE * *out) {  \
		void* tmp = NULL;                                                                                 \
		if (!array_pop_last(list->raw, &tmp))                                                             \
			return false;                                                                                 \
		*out = (TYPE*) tmp;                                                                               \
		return true;                                                                                      \
	}                                                                                                     \
                                                                                                          \
	__attribute__((unused)) static inline bool NAME##_remove(_LIST_STRUCT(TYPE) * list, size_t index) {   \
		return array_remove(list->raw, index);                                                            \
	}                                                                                                     \
                                                                                                          \
	__attribute__((unused)) static inline bool NAME##_remove_first(_LIST_STRUCT(TYPE) * list) {           \
		return array_remove_first(list->raw);                                                             \
	}                                                                                                     \
                                                                                                          \
	__attribute__((unused)) static inline bool NAME##_remove_last(_LIST_STRUCT(TYPE) * list) {            \
		return array_remove_last(list->raw);                                                              \
	}                                                                                                     \
                                                                                                          \
	__attribute__((unused)) static inline bool NAME##_get(                                                \
		_LIST_STRUCT(TYPE) * list, size_t index, TYPE** out) {                                            \
		void* tmp = NULL;                                                                                 \
		if (!array_get(list->raw, index, &tmp))                                                           \
			return false;                                                                                 \
		*out = (TYPE*) tmp;                                                                               \
		return true;                                                                                      \
	}

#endif	// ARRAY_TPL_H
