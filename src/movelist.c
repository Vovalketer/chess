#include "movelist.h"

#include <assert.h>
#include <stddef.h>

bool move_list_contains(MoveList *list, Move move) {
	assert(list != NULL);
	Move m;
	for (size_t i = 0; i < move_list_size(list); i++) {
		move_list_get(list, i, &m);
		if (m.from == move.from && m.to == move.to) {
			return true;
		}
	}
	return false;
}
