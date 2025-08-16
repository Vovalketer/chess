#ifndef BOARD_H
#define BOARD_H

#include "../common/array_tpl.h"
#include "../include/types.h"
ARRAY_DEFINE_TYPE(History, HistoryList, history)

// TODO: add zobrist key
// we must expose the board because the speed of the movegen depends on bit manipulation

struct Board {
	uint64_t	 pieces[2][6];	  // bitboards, Player and Piece used as index
	uint64_t	 occupancies[2];  // white - black
	uint16_t	 fullmove_counter;
	uint8_t		 castling_rights;
	uint8_t		 halfmove_clock;
	Square		 ep_target;	 // will be set to -1 if no en passant target
	Player		 side;
	HistoryList *history;  // we could infer the ply from the size of the list

	// uint64_t zobrist_key;
};

Board *board_create(void);
Board *board_clone(const Board *board);
void   board_destroy(Board **board);
bool   board_from_fen(Board *board, const char *fen);

// piece
void	  board_set_piece(Board *board, Piece piece, Square sqr);
void	  board_remove_piece(Board *board, Square sqr);
void	  board_move_piece(Board *board, Square from, Square to);
PieceType board_get_piece_type(const Board *board, Square sqr);
Piece	  board_get_piece(const Board *board, Square sqr);
Player	  board_get_occupant(const Board *board, Square sqr);
Player	  board_get_player_turn(const Board *board);

bool board_is_square_threatened(const Board *board, Square sqr, Player player);
bool board_is_check(const Board *board, Player player);

// castling rights
void		   board_set_castling_rights(Board *board, CastlingRights cr);
CastlingRights board_get_castling_rights(const Board *board, Player player);
bool		   board_has_castling_rights(const Board *board, CastlingRights cr);
void		   board_remove_castling_rights(Board *board, CastlingRights cr);

void board_apply_history(Board *board, History hist);
void board_print(const Board *board);

#endif	// BOARD_H
