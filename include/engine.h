#ifndef ENGINE_H
#define ENGINE_H

#include "history.h"
#include "position.h"
#include "turn_moves.h"
#include "types.h"

typedef struct {
	bool mask[8][8];
	int count;
} MoveMask;

bool engine_create_match(GameState **state);
bool engine_create_match_from_fen(GameState **state, const char *fen);
void engine_destroy_match(GameState **state);
Player engine_get_active_player(GameState *state);
int engine_get_turn(GameState *state);
Piece engine_get_piece(GameState *state, Position pos);
bool engine_move_piece(GameState *state, Position src, Position dst);
MoveMask engine_get_valid_moves(GameState *state, Position pos);
const TurnMoves *engine_get_legal_moves(GameState *state);
const TurnHistory *engine_get_history(GameState *state);
const TurnRecord *engine_get_last_turn_record(GameState *state);
void engine_undo_move(GameState *state);

#endif
