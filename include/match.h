#ifndef MATCH_H
#define MATCH_H

#include "types.h"

bool match_create(MatchState **state);
bool match_clone(MatchState **dst, const MatchState *src);
void match_destroy(MatchState **state);
Piece match_get_piece(const MatchState *state, Position pos);
bool match_set_piece(MatchState *state, Piece piece, Position pos);
bool match_move_piece(MatchState *state, Position src, Position dst);
void match_remove_piece(MatchState *state, Position pos);
Player match_get_player_turn(const MatchState *state);
int match_get_turn(const MatchState *state);
int match_next_turn(MatchState *state);
bool match_is_empty(const MatchState *state, Position pos);
bool match_is_enemy(const MatchState *state, Player player, Position pos);
bool match_is_within_bounds(Position pos);
Position match_find_king_pos(const MatchState *state, Player player);
#endif
