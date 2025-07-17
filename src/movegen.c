#include "movegen.h"

#include <assert.h>
#include <stdbool.h>
#include <stdint.h>

#include "bits.h"
#include "board.h"
#include "types.h"
#define NOT_FILE_A	0xFEFEFEFEFEFEFEFEULL  // zeros out file H
#define NOT_FILE_H	0x7F7F7F7F7F7F7F7FULL  // zeros out file A
#define NOT_FILE_AB 0xFCFCFCFCFCFCFCFCULL  // zeros out file AB
#define NOT_FILE_GH 0x3F3F3F3F3F3F3F3FULL  // zeros out file GH

static void init_pawn_attacks(void);
static void init_knight_attacks(void);
static void init_king_attacks(void);

typedef enum {
	DIR_N = 8,
	DIR_S = -8,
	DIR_W = -1,
	DIR_E = 1,

	DIR_NW = 7,
	DIR_NE = 9,
	DIR_SW = -9,
	DIR_SE = -7,
} Direction;

// move tables for pieces with fixed movements
uint64_t pawn_attacks[2][64];
uint64_t pawn_pushes[2][64];
uint64_t knight_attacks[64];
uint64_t king_attacks[64];

const int8_t diagonal_offsets[8] = {DIR_NW, DIR_SW, DIR_SE, DIR_NE};
const int8_t cross_offsets[4]	 = {DIR_N, DIR_S, DIR_E, DIR_W};

void movegen_init(void) {
	init_pawn_attacks();
	init_knight_attacks();
	init_king_attacks();
}

static void init_pawn_attacks(void) {
	for (Square sqr = SQ_A1; sqr < SQ_CNT; sqr++) {
		uint64_t bb = 0ULL;
		bits_set(&bb, sqr);
		// white
		uint64_t bb_w =
			(bits_shift_copy(bb, DIR_NE) & NOT_FILE_A) | (bits_shift_copy(bb, DIR_NW) & NOT_FILE_H);

		pawn_attacks[PLAYER_W][sqr] = bb_w;

		// black
		uint64_t bb_b =
			(bits_shift_copy(bb, DIR_SE) & NOT_FILE_A) | (bits_shift_copy(bb, DIR_SW) & NOT_FILE_H);

		pawn_attacks[PLAYER_B][sqr] = bb_b;
	}
}

static void init_knight_attacks(void) {
	for (Square sqr = SQ_A1; sqr < SQ_CNT; sqr++) {
		uint64_t bb = 0ULL;
		bits_set(&bb, sqr);
		uint64_t attacks = 0ULL;
		if (bb & NOT_FILE_A) {
			attacks |= bits_shift_copy(bb, DIR_NW + DIR_N) | bits_shift_copy(bb, DIR_SW + DIR_S);
		}
		if (bb & NOT_FILE_AB) {
			attacks |= bits_shift_copy(bb, DIR_NW + DIR_W) | bits_shift_copy(bb, DIR_SW + DIR_W);
		}
		if (bb & NOT_FILE_H) {
			attacks |= bits_shift_copy(bb, DIR_NE + DIR_N) | bits_shift_copy(bb, DIR_SE + DIR_S);
		}
		if (bb & NOT_FILE_GH) {
			attacks |= bits_shift_copy(bb, DIR_NE + DIR_E) | bits_shift_copy(bb, DIR_SE + DIR_E);
		}

		knight_attacks[sqr] = attacks;
	}
}

static void init_king_attacks(void) {
	for (Square sqr = SQ_A1; sqr < SQ_CNT; sqr++) {
		uint64_t bb = 0ULL;
		bits_set(&bb, sqr);
		uint64_t attacks = 0ULL;
		if (bb & NOT_FILE_A) {
			attacks |= bits_shift_copy(bb, DIR_N) | bits_shift_copy(bb, DIR_NW) | bits_shift_copy(bb, DIR_W) |
					   bits_shift_copy(bb, DIR_SW) | bits_shift_copy(bb, DIR_S);
		}
		if (bb & NOT_FILE_H) {
			attacks |= bits_shift_copy(bb, DIR_N) | bits_shift_copy(bb, DIR_NE) | bits_shift_copy(bb, DIR_E) |
					   bits_shift_copy(bb, DIR_SE) | bits_shift_copy(bb, DIR_S);
		}

		king_attacks[sqr] = attacks;
	}
}
