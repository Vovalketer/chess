#ifndef BITS_H
#define BITS_H

#include <stdint.h>

uint64_t bits_get(const uint64_t bitboard, int offset);
void	 bits_set(uint64_t *bitboard, int offset);
void	 bits_clear(uint64_t *bitboard, int offset);
int8_t	 bits_pop_lsb(uint64_t *bitboard);
int8_t	 bits_pop_msb(uint64_t *bitboard);
void	 bits_shift(uint64_t *bitboard, int offset);
uint64_t bits_shift_copy(const uint64_t bitboard, int offset);
int8_t	 bits_get_lsb(const uint64_t bitboard);
int8_t	 bits_get_msb(const uint64_t bitboard);
int8_t	 bits_get_popcount(const uint64_t bitboard);

#endif
