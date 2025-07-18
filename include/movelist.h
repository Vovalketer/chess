#ifndef MOVELIST_H
#define MOVELIST_H
#include <stddef.h>

#include "array_tpl.h"
#include "move.h"
ARRAY_DEFINE_TYPE(Move, MoveList, move_list)

bool move_list_contains(MoveList *list, Move move);

#endif
