#include "bits.h"

#include <stdint.h>

uint64_t bits_get(const uint64_t bits, int offset) {
	return bits & (1ULL << offset);
}

void bits_set(uint64_t *bits, int offset) {
	*bits |= (1ULL << offset);
}

void bits_clear(uint64_t *bits, int offset) {
	*bits &= ~(1ULL << offset);
}

int8_t bits_pop(uint64_t *bits) {
	uint64_t bit = bits_get_lsb(*bits);
	*bits &= (*bits - 1);
	return bit;
}

uint64_t bits_shift_copy(const uint64_t bits, int offset) {
	return offset > 0 ? bits << offset : bits >> -offset;
}

void bits_shift(uint64_t *bits, int offset) {
	*bits = bits_shift_copy(*bits, offset);
}

// NOTE: using builtins for this is faster than the alternatives
// but they are not portable

int8_t bits_get_lsb(const uint64_t bits) {
	return __builtin_ctzll(bits);
}

int8_t bits_get_msb(const uint64_t bits) {
	return __builtin_clzll(bits);
}

int8_t bits_get_popcount(const uint64_t bits) {
	return __builtin_popcountll(bits);
}
