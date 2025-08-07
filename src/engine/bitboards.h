#ifndef BITBOARDS_H
#define BITBOARDS_H

#include <stdbool.h>
#include <stdint.h>

#include "../include/types.h"

void bitboards_init(void);

uint64_t bitboards_get_pawn_attacks(Square sqr, Player player);
uint64_t bitboards_get_pawn_pushes(Square sqr, Player player);
uint64_t bitboards_get_pawn_double_pushes(Square sqr, Player player);
uint64_t bitboards_get_rook_attacks(Square sqr, uint64_t occupancies);
uint64_t bitboards_get_knight_attacks(Square sqr);
uint64_t bitboards_get_bishop_attacks(Square sqr, uint64_t occupancies);
uint64_t bitboards_get_queen_attacks(Square sqr, uint64_t occupancies);
uint64_t bitboards_get_king_attacks(Square sqr);

#endif /* BITBOARDS_H */
