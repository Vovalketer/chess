#ifndef BOARD_H
#define BOARD_H

#include "types.h"

// TODO: add zobrist key
// we must expose the board because the speed of the movegen depends on bit manipulation

struct Board {
	uint64_t pieces[6];		  // bitboards, uses Piece as index
	uint64_t occupancies[2];  // white - black
	uint16_t fullmove_counter;
	uint8_t	 castling_rights;
	uint8_t	 side;
	int8_t	 ep_target;	 // ep key, will be set to -1 if no en passant target
	uint8_t	 halfmove_clock;
	// 2 bytes padding
	// uint64_t zobrist_key;
};

Board *board_create(void);
void   board_destroy(Board **board);

bool	  board_set_piece(Board *board, Player player, PieceType piece, Square sqr);
void	  board_remove_piece(Board *board, Square sqr);
PieceType board_get_piece(const Board *board, Square sqr);

void board_print(const Board *board);

#endif	// BOARD_H
