#ifndef MATCH_H
#define MATCH_H

#include "history.h"
#include "position.h"
#include "types.h"

struct Board;

bool match_create(MatchState **state);
bool match_create_empty(MatchState **state);
bool match_clone(MatchState **dst, const MatchState *src);
void match_destroy(MatchState **state);
struct Board *match_get_board(MatchState *state);
bool match_move_piece(MatchState *state, Position src, Position dst);
Piece match_get_piece(const MatchState *state, Position pos);
Player match_get_player_turn(const MatchState *state);
int match_get_turn(const MatchState *state);
int match_previous_turn(MatchState *state);
int match_next_turn(MatchState *state);
bool match_append_turn_record(MatchState *state, Move move);
bool match_get_turn_record(MatchState *state, size_t turn, TurnRecord **out_record);
TurnHistory *match_get_history(MatchState *state);
// Returns a clone of the history. User is in charge of freeing the memory
bool match_get_history_clone(MatchState *state, TurnHistory **out_history);
bool match_undo_move(MatchState *state);
#endif
