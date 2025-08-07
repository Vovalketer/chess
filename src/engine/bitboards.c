#include "bitboards.h"

#include <assert.h>
#include <stdbool.h>
#include <stdint.h>

#include "bits.h"
#include "types.h"
#define NOT_FILE_A	0xFEFEFEFEFEFEFEFEULL  // zeros out file A
#define NOT_FILE_H	0x7F7F7F7F7F7F7F7FULL  // zeros out file H
#define NOT_FILE_AB 0xFCFCFCFCFCFCFCFCULL  // zeros out file AB
#define NOT_FILE_GH 0x3F3F3F3F3F3F3F3FULL  // zeros out file GH
static void init_pawn_attacks(void);
static void init_pawn_pushes(void);
static void init_pawn_double_pushes(void);
static void init_knight_attacks(void);
static void init_king_attacks(void);

// move tables for pieces with fixed movements
uint64_t pawn_attacks[2][64];
uint64_t knight_attacks[64];
uint64_t king_attacks[64];
uint64_t pawn_pushes[2][64];
uint64_t pawn_double_pushes[2][8];

bool bitboards_initialized = false;

void bitboards_init(void) {
	init_pawn_attacks();
	init_pawn_pushes();
	init_pawn_double_pushes();
	init_knight_attacks();
	init_king_attacks();
	bitboards_initialized = true;
}

bool bitboards_is_init(void) {
	return bitboards_initialized;
}

static void init_pawn_pushes(void) {
	for (Square sqr = SQ_A1; sqr < SQ_CNT; sqr++) {
		uint64_t bb = 0ULL;
		bits_set(&bb, sqr);
		uint64_t w_pushes = 0ULL;

		// white
		w_pushes |= bits_shift_copy(bb, DIR_N);
		pawn_pushes[PLAYER_W][sqr] = w_pushes;

		// black
		uint64_t b_pushes = 0ULL;
		b_pushes |= bits_shift_copy(bb, DIR_S);
		pawn_pushes[PLAYER_B][sqr] = b_pushes;
	}
}

static void init_pawn_double_pushes(void) {
	for (Square sqr = SQ_A2, i = 0; sqr <= SQ_H2; sqr++, i++) {
		pawn_double_pushes[PLAYER_W][i] = 1ULL << (sqr + DIR_N * 2);
	}

	for (Square sqr = SQ_A7, i = 0; sqr <= SQ_H7; sqr++, i++) {
		pawn_double_pushes[PLAYER_B][i] = 1ULL << (sqr + DIR_S * 2);
	}
}

static void init_pawn_attacks(void) {
	for (Square sqr = SQ_A1; sqr < SQ_CNT; sqr++) {
		uint64_t bb = 0ULL;
		bits_set(&bb, sqr);
		// white
		uint64_t w_bb =
			(bits_shift_copy(bb, DIR_NE) & NOT_FILE_A) | (bits_shift_copy(bb, DIR_NW) & NOT_FILE_H);

		pawn_attacks[PLAYER_W][sqr] = w_bb;

		// black
		uint64_t b_bb =
			(bits_shift_copy(bb, DIR_SE) & NOT_FILE_A) | (bits_shift_copy(bb, DIR_SW) & NOT_FILE_H);

		pawn_attacks[PLAYER_B][sqr] = b_bb;
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
		attacks |= bits_shift_copy(bb, DIR_N);
		attacks |= bits_shift_copy(bb, DIR_S);
		attacks |= bits_shift_copy(bb, DIR_E) & NOT_FILE_A;
		attacks |= bits_shift_copy(bb, DIR_W) & NOT_FILE_H;
		attacks |= bits_shift_copy(bb, DIR_NW) & NOT_FILE_H;
		attacks |= bits_shift_copy(bb, DIR_SW) & NOT_FILE_H;
		attacks |= bits_shift_copy(bb, DIR_NE) & NOT_FILE_A;
		attacks |= bits_shift_copy(bb, DIR_SE) & NOT_FILE_A;

		king_attacks[sqr] = attacks;
	}
}

static uint64_t attacks_get_cross(Square sqr, uint64_t occupancies) {
	uint64_t attacks = 0ULL;
	int		 rank	 = sqr / 8;
	int		 file	 = sqr % 8;
	for (int r = rank + 1; r < 8; r++) {
		uint64_t bb = 1ULL << (r * 8 + file);
		attacks |= bb;
		// the attacks mask will include the blocker
		if (bb & occupancies) {
			break;
		}
	};
	for (int r = rank - 1; r >= 0; r--) {
		uint64_t bb = 1ULL << (r * 8 + file);
		attacks |= bb;
		if (bb & occupancies) {
			break;
		}
	};

	for (int f = file + 1; f < 8; f++) {
		uint64_t bb = 1ULL << (rank * 8 + f);
		attacks |= bb;
		if (bb & occupancies) {
			break;
		}
	};

	for (int f = file - 1; f >= 0; f--) {
		uint64_t bb = 1ULL << (rank * 8 + f);
		attacks |= bb;
		if (bb & occupancies) {
			break;
		}
	};

	return attacks;
}

static uint64_t attacks_get_diagonal(Square sqr, uint64_t occupancies) {
	uint64_t attacks = 0ULL;
	int		 rank	 = sqr / 8;
	int		 file	 = sqr % 8;

	for (int r = rank + 1, f = file + 1; r < 8 && f < 8; r++, f++) {
		uint64_t bb = 1ULL << (r * 8 + f);
		attacks |= bb;
		if (bb & occupancies) {
			break;
		}
	};

	for (int r = rank - 1, f = file - 1; r >= 0 && f >= 0; r--, f--) {
		uint64_t bb = 1ULL << (r * 8 + f);
		attacks |= bb;
		if (bb & occupancies) {
			break;
		}
	};

	for (int r = rank + 1, f = file - 1; r < 8 && f >= 0; r++, f--) {
		uint64_t bb = 1ULL << (r * 8 + f);
		attacks |= bb;
		if (bb & occupancies) {
			break;
		}
	}

	for (int r = rank - 1, f = file + 1; r >= 0 && f < 8; r--, f++) {
		uint64_t bb = 1ULL << (r * 8 + f);
		attacks |= bb;
		if (bb & occupancies) {
			break;
		}
	}

	return attacks;
}

uint64_t bitboards_get_pawn_attacks(Square sqr, Player player) {
	return pawn_attacks[player][sqr];
}

uint64_t bitboards_get_pawn_pushes(Square sqr, Player player) {
	return pawn_pushes[player][sqr];
}

uint64_t bitboards_get_pawn_double_pushes(Square sqr, Player player) {
	if (((sqr < SQ_A2 || sqr > SQ_H2) && player == PLAYER_W) ||
		((sqr < SQ_A7 || sqr > SQ_H7) && player == PLAYER_B)) {
		return 0ULL;
	}
	return pawn_double_pushes[player][sqr % 8];
}

uint64_t bitboards_get_rook_attacks(Square sqr, uint64_t occupancies) {
	return attacks_get_cross(sqr, occupancies);
}

uint64_t bitboards_get_knight_attacks(Square sqr) {
	return knight_attacks[sqr];
}

uint64_t bitboards_get_bishop_attacks(Square sqr, uint64_t occupancies) {
	return attacks_get_diagonal(sqr, occupancies);
}

uint64_t bitboards_get_queen_attacks(Square sqr, uint64_t occupancies) {
	uint64_t attacks = attacks_get_diagonal(sqr, occupancies);
	attacks |= attacks_get_cross(sqr, occupancies);
	return attacks;
}

uint64_t bitboards_get_king_attacks(Square sqr) {
	return king_attacks[sqr];
}
