#ifndef UTILS_H
#define UTILS_H
#include "types.h"

Player utils_get_opponent(Player player);
int	   utils_get_file(Square sqr);
int	   utils_get_rank(Square sqr);
Square utils_fr_to_square(int file, int rank);
char   utils_piece_to_char(Piece piece);
Piece  utils_char_to_piece(char c);
bool   utils_is_valid_square(Square sqr);

#endif	// UTILS_H
