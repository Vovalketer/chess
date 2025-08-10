#include "movelist.h"

#include <assert.h>
#include <stddef.h>

bool move_list_contains(MoveList *list, Move move) {
	assert(list != NULL);
	for (size_t i = 0; i < move_list_size(list); i++) {
		Move *m = move_list_get(list, i);
		if (m->from == move.from && m->to == move.to && m->mv_type == move.mv_type &&
			m->piece == move.piece) {
			return true;
		}
	}
	return false;
}
