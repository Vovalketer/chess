#ifndef ENGINE_H
#define ENGINE_H


#include "types.h"

typedef struct {
	bool mask[8][8];
	int count;
} MoveMask;

bool engine_create_match(BoardState **state);
void engine_destroy_match(BoardState **state);
Player engine_get_active_player(const BoardState *state);
int engine_get_turn(const BoardState *state);
Piece engine_get_piece(const BoardState *state, int x, int y);
bool engine_move_piece(BoardState *state, int x_src, int y_src, int x_dest, int y_dest);
MoveMask engine_get_valid_moves(const BoardState *state, int x, int y);

#endif
