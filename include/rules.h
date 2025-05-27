#ifndef RULES_H
#define RULES_H
#include "../include/move.h"
#include "types.h"

bool rules_is_valid_move(BoardState *state, Move move);
bool rules_is_check(BoardState *state, Player player);

#endif
