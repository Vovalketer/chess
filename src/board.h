#ifndef BOARD_H
#define BOARD_H

#include "types.h"

// TODO: add zobrist key
// we must expose the board because the speed of the movegen depends on bit manipulation

struct Board {
	uint64_t pieces[2][6];	  // bitboards, Player and Piece used as index
	uint64_t occupancies[2];  // white - black
	uint16_t fullmove_counter;
	uint8_t	 castling_rights;
	int8_t	 ep_target;	 // ep key, will be set to -1 if no en passant target
	Player	 side;
	uint8_t	 halfmove_clock;
	// 7 bytes padding
	// uint64_t zobrist_key;
};

Board *board_create(void);
void   board_destroy(Board **board);

bool   board_set_piece(Board *board, Player player, PieceType piece, Square sqr);
void   board_remove_piece(Board *board, Square sqr);
Piece  board_get_piece(const Board *board, Square sqr);
Piece  board_create_piece(Player player, PieceType piece);
Player board_get_opponent(Player player);
bool   board_has_enemy(const Board *board, Square sqr, Player player);
bool   board_has_ally(const Board *board, Square sqr, Player player);
bool   board_is_occupied(const Board *board, Square sqr);
Player board_get_occupant(const Board *board, Square sqr);
bool   board_has_castling_rights(const Board *board, CastlingRights cr);

void board_print(const Board *board);

#endif	// BOARD_H
