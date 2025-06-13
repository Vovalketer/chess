#ifndef ENGINE_H
#define ENGINE_H

#include "position.h"
#include "types.h"

typedef struct {
	bool mask[8][8];
	int count;
} MoveMask;

bool engine_create_match(MatchState **state);
void engine_destroy_match(MatchState **state);
Player engine_get_active_player(const MatchState *state);
int engine_get_turn(const MatchState *state);
Piece engine_get_piece(const MatchState *state, Position pos);
bool engine_move_piece(MatchState *state, Position src, Position dst);
MoveMask engine_get_valid_moves(const MatchState *state, Position pos);
void engine_undo_move(MatchState *state);

#endif
