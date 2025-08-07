#ifndef ENGINE_H
#define ENGINE_H

#include "types.h"

typedef struct {
	int x;
	int y;
} Position;

typedef struct {
	bool mask[8][8];
} MoveMask;

bool	 engine_create_standard_match(Board **board);
bool	 engine_create_match_from_fen(Board **board, const char *fen);
void	 engine_destroy_match(Board **board);
Piece	 engine_get_piece(Board *board, Position pos);
MoveMask engine_get_valid_moves(Board *board, Position pos);
bool	 engine_move_piece(Board *board, Position from, Position to);
bool	 engine_autoplay_move(Board *board);
void	 engine_undo_move(Board *board);
#endif	// ENGINE_H
