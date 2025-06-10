#ifndef RULES_H
#define RULES_H
#include "../include/move.h"
#include "types.h"

bool rules_is_valid_move(MatchState *state, Move move);
bool rules_is_check(MatchState *state, Player player);
bool rules_is_check_after_move(MatchState *state, Move move);

#endif
