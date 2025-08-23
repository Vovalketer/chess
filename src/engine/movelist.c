#include "movelist.h"

#include <assert.h>
#include <stddef.h>

#include "types.h"

bool move_list_contains(MoveList *list, Move move) {
	assert(list != NULL);
	for (size_t i = 0; i < move_list_size(list); i++) {
		Move *m = move_list_get(list, i);
		if (move_equals(move, *m)) {
			return true;
		}
	}
	return false;
}
