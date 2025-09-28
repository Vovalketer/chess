#ifndef CONTAINER_OF_H
#define CONTAINER_OF_H

#define container_of(ptr, type, member)                         \
	({                                                          \
		const __typeof__(((type *) 0)->member) *__mptr = (ptr); \
		(type *) ((char *) __mptr - offsetof(type, member));    \
	})

#endif
