#ifndef BOARD_H
#define BOARD_H

#include <stdbool.h>

#include "position.h"
#include "types.h"
typedef struct Board Board;
bool board_create(Board **board);
bool board_destroy(Board **board);
bool board_clone(Board **clone, const Board *board);
void board_init_positions(Board *board);
bool board_is_within_bounds(Position pos);
bool board_set_piece(Board *board, Piece piece, Position pos);
void board_remove_piece(Board *board, Position pos);
Piece board_get_piece(const Board *board, Position pos);
bool board_move_piece(Board *board, Position src, Position dst);
bool board_is_empty(const Board *board, Position pos);
bool board_is_enemy(const Board *board, Player player, Position pos);
Position board_find_king_pos(const Board *board, Player player);

#endif	// BOARD_H
