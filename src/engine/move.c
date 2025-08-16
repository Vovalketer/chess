#include "types.h"

bool move_equals(const Move move1, const Move move2) {
	return move1.from == move2.from && move1.to == move2.to && move1.piece == move2.piece &&
		   move1.captured == move2.captured && move1.mv_type == move2.mv_type;
}
