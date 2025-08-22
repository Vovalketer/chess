#include "types.h"

bool move_equals(const Move move1, const Move move2) {
	return move1.from == move2.from && move1.to == move2.to &&
		   move1.piece.type == move2.piece.type && move1.piece.player == move2.piece.player &&
		   move1.captured_type == move2.captured_type && move1.mv_type == move2.mv_type;
}
