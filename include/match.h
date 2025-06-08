#ifndef MATCH_H
#define MATCH_H

#include "position.h"
#include "turn_history.h"
#include "types.h"

struct Board;

bool match_create(MatchState **state);
bool match_clone(MatchState **dst, const MatchState *src);
void match_destroy(MatchState **state);
const struct Board *match_get_board(const MatchState *state);
bool match_move_piece(MatchState *state, Position src, Position dst);
Player match_get_player_turn(const MatchState *state);
int match_get_turn(const MatchState *state);
int match_next_turn(MatchState *state);
#endif
