#ifndef UTILS_H
#define UTILS_H
#include "types.h"

typedef struct {
	char str[256];
} FixedStr;

Player		utils_get_opponent(Player player);
int			utils_get_file(Square sqr);
int			utils_get_rank(Square sqr);
Square		utils_fr_to_square(int file, int rank);
Square		utils_ep_capture_pos(Square sqr, Player p);
char		utils_piece_to_char(Piece piece);
Piece		utils_char_to_piece(char c);
bool		utils_is_valid_square(Square sqr);
const char* utils_square_to_str(Square sqr);
const char* utils_piece_type_to_str(PieceType type);
const char* utils_move_type_to_str(MoveType type);
const char* utils_piece_to_unicode_str(Piece piece);
// returns a char[256] string with the move description as a string, for debugging
FixedStr utils_move_description(Board* board, Move move);
FixedStr utils_square_contents(Board* board, Square sqr);

#endif	// UTILS_H
