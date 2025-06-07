#include "movelist.h"

#include <assert.h>

bool move_list_contains(MoveList *list, Move move) {
	assert(list != NULL);
	for (size_t i = 0; i < move_list_size(list); i++) {
		Move *m = NULL;
		move_list_get(list, i, &m);
		if (move_eq(*m, move)) {
			return true;
		}
	}
	return false;
}
