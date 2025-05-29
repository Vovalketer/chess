#include "../include/move.h"

Move move_create(Position src, Position dst) {
	return (Move) {src, dst};
}

bool move_eq(Move m1, Move m2) {
	return m1.src.x == m2.src.x && m1.src.y == m2.src.y && m1.dst.x == m2.dst.x && m1.dst.y == m2.dst.y;
}
