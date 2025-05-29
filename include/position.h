#ifndef POSITION_H
#define POSITION_H

#include <stdbool.h>

typedef struct {
	int x;
	int y;
} Position;

bool position_eq(Position p1, Position p2);
#endif
