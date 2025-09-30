#ifndef MOVELIST_H
#define MOVELIST_H
#include "../common/vector.h"
#include "../include/types.h"
VECTOR_DEFINE_TYPE(Move, MoveList, move_list)

bool move_list_contains(MoveList *list, Move move);

#endif
