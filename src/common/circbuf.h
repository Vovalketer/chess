#ifndef CIRCULAR_BUFFER_H
#define CIRCULAR_BUFFER_H

#include <stddef.h>
#include <stdlib.h>
#include <string.h>

/**
 * Generic circular buffer implementation, designed to be embedded in another struct
 * Warning: This implementation uses the extensions __typeof__, _Static_assert
 * and __builtin_types_compatible_p to achieve compile-time type safety
 * then GNU statement expressions to provide function-like behavior
 *
 * Usage: declare a struct type with the macro DECLARE_CIRCBUF
 * then use the generic macros to access the buffer.
 */

/**
 * Functions
 * circbuf_init(cb_ptr)
 * circbuf_clear(cb_ptr)
 * circbuf_len(cb_ptr)
 * circbuf_full(cb_ptr)
 * circbuf_empty(cb_ptr)
 * circbuf_put(cb_ptr, val)
 * circbuf_put_overwrite(cb_ptr, val)
 * circbuf_get(cb_ptr, out_ptr)
 */

/**
 * @brief Macro to declare a type-specific circular buffer.
 *
 * This macro defines a new struct type that contains the buffer's metadata
 * and a typed array
 *
 * @param name The name of the struct
 * @param type The data type of the elements in the buffer.
 * @param cap The total capacity of the buffer (number of elements).
 */
#define DECLARE_CIRCBUF(name, type) \
	struct name {                   \
		size_t in;                  \
		size_t out;                 \
		size_t capacity;            \
		type  *data;                \
	}

#define DEFINE_CIRCBUF_BUFFER(name, type, cap) \
	DECLARE_CIRCBUF(name, type, cap);          \
	type		name##_buf[cap];               \
	struct name name = {.in = 0, .out = 0, .capacity = cap, .data = name##_buf};

/**
 * @brief Initializes a declared circular buffer.
 *
 * @param cb_ptr A pointer to the declared circular buffer struct.
 * @param buff A pointer to the buffer's data array.
 * @param cap The total capacity of the buffer (number of elements).
 */

#define circbuf_init(cb_ptr, buff, cap)      \
	do {                                     \
		__typeof__(cb_ptr) __cb	 = (cb_ptr); \
		__typeof__(cap)	   __cap = (cap);    \
		__cb->in				 = 0;        \
		__cb->out				 = 0;        \
		__cb->capacity			 = __cap;    \
		__cb->data				 = buff;     \
	} while (0)

/**
 * @brief Initializes a declared circular buffer using malloc.
 *
 * @param cb_ptr A pointer to the declared circular buffer struct.
 * @param type The data type of the elements in the buffer.
 * @param cap The total capacity of the buffer (number of elements).
 * @return 1 if successful, 0 otherwise.
 * */

#define circbuf_alloc(cb_ptr, type, cap)                           \
	({                                                             \
		__typeof__(cap)	   __cap = (cap);                          \
		__typeof__(cb_ptr) __cb	 = (cb_ptr);                       \
		(__cb)->data			 = malloc((__cap) * sizeof(type)); \
		if ((__cb)->data) {                                        \
			(__cb)->capacity = __cap;                              \
			(__cb)->in		 = 0;                                  \
			(__cb)->out		 = 0;                                  \
		}                                                          \
		(__cb)->data != NULL;                                      \
	})

#define circbuf_free(cb_ptr)                \
	do {                                    \
		__typeof__(cb_ptr) __cb = (cb_ptr); \
		free((__cb)->data);                 \
		(__cb)->data	 = NULL;            \
		(__cb)->capacity = 0;               \
		(__cb)->in = (__cb)->out = 0;       \
	} while (0)

/**
 * @brief Clears the buffer by resetting its pointers.
 *
 * @param cb_ptr A pointer to the declared circular buffer struct.
 */
#define circbuf_clear(cb_ptr) \
	do {                      \
		(cb_ptr)->in  = 0;    \
		(cb_ptr)->out = 0;    \
	} while (0)

/**
 * @brief Gets the length of the circular buffer.
 *
 * @param cb_ptr A pointer to the declared circular buffer struct.
 * @return The length of the buffer.
 */
#define circbuf_len(cb_ptr)                                                               \
	({                                                                                    \
		((cb_ptr)->capacity == 0)                                                         \
			? (size_t) 0                                                                  \
			: (((cb_ptr)->in + (cb_ptr)->capacity - (cb_ptr)->out) % (cb_ptr)->capacity); \
	})
/**
 * @brief Checks if the buffer is full.
 *
 * @param cb_ptr A pointer to the declared circular buffer struct.
 * @return 1 if full, 0 otherwise.
 */
#define circbuf_full(cb_ptr) \
	((cb_ptr)->capacity != 0) && ((((cb_ptr)->in + 1) % (cb_ptr)->capacity) == (cb_ptr)->out)
/**
 * @brief Checks if the buffer is empty.
 *
 * @param cb_ptr A pointer to the declared circular buffer struct.
 * @return 1 if empty, 0 otherwise.
 */
#define circbuf_empty(cb_ptr) ((cb_ptr)->in == (cb_ptr)->out)

#define __CIRCBUF_TYPE_CHECK_VAL(cb, val)                                           \
	_Static_assert(                                                                 \
		__builtin_types_compatible_p(__typeof__((val)), __typeof__((cb)->data[0])), \
		"Error: Incompatible type. The item's type does not match the buffer's element type.");
#define __CIRCBUF_TYPE_CHECK_PTR(cb, val_ptr)                                              \
	_Static_assert(                                                                        \
		__builtin_types_compatible_p(__typeof__((val_ptr)), __typeof__(&((cb)->data[0]))), \
		"Error: Incompatible type. The item's type does not match the buffer's element type.");
/**
 * @brief Puts an element into the circular buffer.
 *
 * This macro provides a compile-time assertion to ensure that the
 * element's type matches the buffer's element type.
 *
 * @param cb_ptr A pointer to the declared circular buffer struct.
 * @param val The value to be inserted.
 * @return 1 on success, 0 if the buffer is full.
 */
#define circbuf_put(cb_ptr, val)                                                    \
	({                                                                              \
		__CIRCBUF_TYPE_CHECK_VAL(cb_ptr, val);                                      \
		int				   __ret = 0;                                               \
		__typeof__(val)	   __val = (val);                                           \
		__typeof__(cb_ptr) __cb	 = (cb_ptr);                                        \
		if (!circbuf_full(__cb)) {                                                  \
			memcpy(&((__cb)->data[(__cb)->in]), (&__val), sizeof((__cb)->data[0])); \
			(__cb)->in = ((__cb)->in + 1) % (__cb)->capacity;                       \
			__ret	   = 1;                                                         \
		}                                                                           \
		__ret;                                                                      \
	})

/**
 * @brief Puts an element into the circular buffer overwriting old
 * elements if the buffer is full. Always succeeds so it returns nothing
 *
 * This macro provides a compile-time assertion to ensure that the
 * element's type matches the buffer's element type.
 *
 * @param cb_ptr A pointer to the declared circular buffer struct.
 * @param val The value to be inserted.
 */
#define circbuf_put_overwrite(cb_ptr, val)                                      \
	do {                                                                        \
		__CIRCBUF_TYPE_CHECK_VAL(cb_ptr, val);                                  \
		__typeof__(cb_ptr) __cb	 = (cb_ptr);                                    \
		__typeof__(val)	   __val = (val);                                       \
		if (circbuf_full(__cb)) {                                               \
			(__cb)->out = ((__cb)->out + 1) % (__cb)->capacity;                 \
		}                                                                       \
		memcpy(&((__cb)->data[(__cb)->in]), (&__val), sizeof((__cb)->data[0])); \
		(__cb)->in = ((__cb)->in + 1) % (__cb)->capacity;                       \
	} while (0)

/**
 * @brief Gets an element from the circular buffer.
 *
 * This macro copies the element at the head of the buffer to the
 * provided pointer. It provides a compile-time assertion to ensure
 * type safety.
 *
 * @param cb_ptr A pointer to the declared circular buffer struct.
 * @param out_val_ptr A pointer to where the retrieved value should be stored.
 * @return 1 on success, 0 if the buffer is empty.
 */
#define circbuf_get(cb_ptr, out_val_ptr)                                                  \
	({                                                                                    \
		__CIRCBUF_TYPE_CHECK_PTR(cb_ptr, (out_val_ptr));                                  \
		int				   __ret = 0;                                                     \
		__typeof__(cb_ptr) __cb	 = (cb_ptr);                                              \
		if (!circbuf_empty(__cb)) {                                                       \
			memcpy((out_val_ptr), &((__cb)->data[(__cb)->out]), sizeof((__cb)->data[0])); \
			(__cb)->out = ((__cb)->out + 1) % (__cb)->capacity;                           \
			__ret		= 1;                                                              \
		}                                                                                 \
		__ret;                                                                            \
	})

#endif	// CIRCULAR_BUFFER_H
