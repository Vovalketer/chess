#include "../include/move.h"

	return p1.x == p2.x && p1.y == p2.y;
}

Move move_create(Position src, Position dst) {
	return (Move) {src, dst};
}
