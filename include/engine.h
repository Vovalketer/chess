#ifndef ENGINE_H
#define ENGINE_H

#include <stdbool.h>

#include "types.h"

typedef struct {
	bool mask[8][8];
	int count;
} MoveMask;

bool create_empty_board(BoardState **state);
bool create_standard_match(BoardState **state);
void destroy_game(BoardState **state);
Player get_active_player(const BoardState *state);
int get_turn(const BoardState *state);
Piece get_piece(const BoardState *state, int x, int y);
bool move_piece(BoardState *state, int x_src, int y_src, int x_dest, int y_dest);
MoveMask get_valid_moves(const BoardState *state, int x, int y);

#endif
