#ifndef BOARD_H
#define BOARD_H

#include <stdbool.h>

#include "types.h"

bool board_create(BoardState **state);
void board_destroy(BoardState **state);
Piece board_get_piece(const BoardState *state, int x, int y);
bool board_set_piece(BoardState *state, Piece piece, int x, int y);
bool board_move_piece(BoardState *state, int x_src, int y_src, int x_dest, int y_dest);
void board_remove_piece(BoardState *state, int x, int y);
Player board_get_player_turn(const BoardState *state);
int board_get_turn(const BoardState *state);
int board_next_turn(BoardState *state);
bool board_is_empty(const BoardState *state, int x, int y);
bool board_is_within_bounds(int x, int y);

#endif
