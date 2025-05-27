#ifndef BOARD_H
#define BOARD_H

#include "types.h"

bool board_create(BoardState **state);
void board_destroy(BoardState **state);
Piece board_get_piece(const BoardState *state, Position pos);
bool board_set_piece(BoardState *state, Piece piece, Position pos);
bool board_move_piece(BoardState *state, Position src, Position dst);
void board_remove_piece(BoardState *state, Position pos);
Player board_get_player_turn(const BoardState *state);
int board_get_turn(const BoardState *state);
int board_next_turn(BoardState *state);
bool board_is_empty(const BoardState *state, Position pos);
bool board_is_enemy(const BoardState *state, Player player, Position pos);
bool board_is_within_bounds(Position pos);

#endif
