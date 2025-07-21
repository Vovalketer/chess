#include "movegen.h"

#include <assert.h>
#include <stdbool.h>
#include <stdint.h>

#include "bits.h"
#include "board.h"
#include "log.h"
#include "movelist.h"
#include "types.h"
#define NOT_FILE_A			  0xFEFEFEFEFEFEFEFEULL	 // zeros out file H
#define NOT_FILE_H			  0x7F7F7F7F7F7F7F7FULL	 // zeros out file A
#define NOT_FILE_AB			  0xFCFCFCFCFCFCFCFCULL	 // zeros out file AB
#define NOT_FILE_GH			  0x3F3F3F3F3F3F3F3FULL	 // zeros out file GH
#define RANK_1				  0x00000000000000FFULL
#define RANK_2				  0xFF00ULL
#define RANK_7				  0xFF000000000000ULL
#define RANK_8				  0xFF00000000000000ULL
#define NOT_RANK_1			  0xFFFFFFFFFFFFFF00ULL
#define NOT_RANK_8			  0x00FFFFFFFFFFFFFFULL
#define W_QS_CASTLING_SQUARES 0xEULL
#define B_QS_CASTLING_SQUARES 0xE00000000000000ULL
#define W_KS_CASTLING_SQUARES 0x60ULL
#define B_KS_CASTLING_SQUARES 0x6000000000000000ULL
static void init_pawn_attacks(void);
static void init_knight_attacks(void);
static void init_king_attacks(void);
static void init_pawn_pushes(void);

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

void movegen_init(void) {
	init_pawn_attacks();
	init_knight_attacks();
	init_king_attacks();
	init_pawn_pushes();
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
		if (bb & NOT_FILE_A) {
			attacks |= bits_shift_copy(bb, DIR_N) | bits_shift_copy(bb, DIR_NW) |
					   bits_shift_copy(bb, DIR_W) | bits_shift_copy(bb, DIR_SW) |
					   bits_shift_copy(bb, DIR_S);
		}
		if (bb & NOT_FILE_H) {
			attacks |= bits_shift_copy(bb, DIR_N) | bits_shift_copy(bb, DIR_NE) |
					   bits_shift_copy(bb, DIR_E) | bits_shift_copy(bb, DIR_SE) |
					   bits_shift_copy(bb, DIR_S);
		}

		king_attacks[sqr] = attacks;
	}
}

static void init_pawn_pushes(void) {
	for (Square sqr = SQ_A1; sqr < SQ_CNT; sqr++) {
		uint64_t bb = 0ULL;
		bits_set(&bb, sqr);
		uint64_t w_pushes = 0ULL;

		// white
		if (bb & RANK_2) {
			w_pushes |= bits_shift_copy(bb, DIR_N * 2);
		}
		w_pushes |= bits_shift_copy(bb, DIR_N);
		pawn_pushes[PLAYER_W][sqr] = w_pushes;

		// black
		uint64_t b_pushes = 0ULL;
		if (bb & RANK_7) {
			b_pushes |= bits_shift_copy(bb, DIR_S * 2);
		}
		b_pushes |= bits_shift_copy(bb, DIR_S);
		pawn_pushes[PLAYER_B][sqr] = b_pushes;
	}
}

void movegen_pawns(const Board *board, PieceType pt, Player p, MoveList *ml) {
	assert(p != PLAYER_NONE);
	uint64_t bb = board->pieces[p][pt];	 // list of pawns from a given player
	while (bb) {
		Square	 sqr	 = bits_pop_lsb(&bb);  // pop and get the index, transform into a square
		uint64_t attk	 = pawn_attacks[p][sqr] & board->occupancies[board_get_opponent(p)];
		Piece	 piece	 = board_create_piece(p, pt);
		bool	 is_prom = sqr > SQ_H6;
		while (attk) {
			Square attk_sqr = bits_pop_lsb(&attk);
			if (!is_prom) {
				Move mv = {
					.from	 = sqr,
					.to		 = attk_sqr,
					.mv_type = MV_CAPTURE,
					.piece	 = piece,
				};
				move_list_append(ml, mv);
			} else {
				Move prom_q = {
					.from	 = sqr,
					.to		 = attk_sqr,
					.mv_type = MV_Q_PROM_CAPTURE,
					.piece	 = piece,
				};
				Move prom_r = {
					.from	 = sqr,
					.to		 = attk_sqr,
					.mv_type = MV_R_PROM_CAPTURE,
					.piece	 = piece,
				};
				Move prom_b = {
					.from	 = sqr,
					.to		 = attk_sqr,
					.mv_type = MV_B_PROM_CAPTURE,
					.piece	 = piece,
				};
				Move prom_n = {
					.from	 = sqr,
					.to		 = attk_sqr,
					.mv_type = MV_N_PROM_CAPTURE,
					.piece	 = piece,
				};
				move_list_append(ml, prom_q);
				move_list_append(ml, prom_r);
				move_list_append(ml, prom_b);
				move_list_append(ml, prom_n);
			}
		}
		uint64_t pushes =
			pawn_pushes[p][sqr] & ~(board->occupancies[PLAYER_W] | board->occupancies[PLAYER_B]);
		while (pushes) {
			Square push_sqr = bits_pop_lsb(&pushes);
			if (!is_prom) {
				Move mv = {
					.from	 = sqr,
					.to		 = push_sqr,
					.mv_type = MV_QUIET,
					.piece	 = piece,
				};
				move_list_append(ml, mv);
			} else {
				Move prom_q = {
					.from	 = sqr,
					.to		 = push_sqr,
					.mv_type = MV_Q_PROM,
					.piece	 = piece,
				};
				Move prom_r = {
					.from	 = sqr,
					.to		 = push_sqr,
					.mv_type = MV_R_PROM,
					.piece	 = piece,
				};
				Move prom_b = {
					.from	 = sqr,
					.to		 = push_sqr,
					.mv_type = MV_B_PROM,
					.piece	 = piece,
				};
				Move prom_n = {
					.from	 = sqr,
					.to		 = push_sqr,
					.mv_type = MV_N_PROM,
					.piece	 = piece,
				};
				move_list_append(ml, prom_q);
				move_list_append(ml, prom_r);
				move_list_append(ml, prom_b);
				move_list_append(ml, prom_n);
			}
		}
	}
}

void movegen_knights(const Board *board, PieceType pt, Player p, MoveList *ml) {
	assert(p != PLAYER_NONE);
	uint64_t bb = board->pieces[p][pt];
	while (bb) {
		Square	 sqr   = bits_pop_lsb(&bb);
		Piece	 piece = board_create_piece(p, pt);
		uint64_t moves = knight_attacks[sqr] & ~board->occupancies[p];
		MoveType mt	   = board_has_enemy(board, sqr, p) ? MV_CAPTURE : MV_QUIET;
		while (moves) {
			Square to = bits_pop_lsb(&moves);
			Move   mv = {
				  .from	   = sqr,
				  .to	   = to,
				  .mv_type = mt,
				  .piece   = piece,
			  };
			move_list_append(ml, mv);
		}
	}
}

void movegen_king(const Board *board, PieceType pt, Player p, MoveList *ml) {
	assert(p != PLAYER_NONE);
	uint64_t bb		  = board->pieces[p][pt];
	Square	 king_sqr = bits_pop_lsb(&bb);
	Piece	 piece	  = board_create_piece(p, pt);
	uint64_t moves	  = king_attacks[king_sqr] & ~board->occupancies[p];
	while (moves) {
		Square	 to = bits_pop_lsb(&moves);
		MoveType mt = board_has_enemy(board, king_sqr, p) ? MV_CAPTURE : MV_QUIET;
		Move	 mv = {
				.from	 = king_sqr,
				.to		 = to,
				.mv_type = mt,
				.piece	 = piece,
		};
		move_list_append(ml, mv);
	}

	switch (p) {
		case PLAYER_W:
			if (board_has_castling_rights(board, CASTLE_W_KS)) {
				if (king_sqr == SQ_E1 && board_get_piece(board, SQ_H1) == W_ROOK &&
					(board->occupancies[PLAYER_W] | board->occupancies[PLAYER_B]) &
						~(W_KS_CASTLING_SQUARES)) {
					// checking for threats is deferred to makemove
					Move mv = {.from			= king_sqr,
							   .to				= SQ_G1,
							   .mv_type			= MV_KS_CASTLE,
							   .piece			= piece,
							   };
					move_list_append(ml, mv);
				}
			}
			if (board_has_castling_rights(board, CASTLE_W_QS)) {
				if (king_sqr == SQ_E1 && board_get_piece(board, SQ_A1) == W_ROOK &&
					(board->occupancies[PLAYER_W] | board->occupancies[PLAYER_B]) &
						~(W_QS_CASTLING_SQUARES)) {
					// checking for threats is deferred to makemove
					Move mv = {.from			= king_sqr,
							   .to				= SQ_C1,
							   .mv_type			= MV_QS_CASTLE,
							   .piece			= piece,
							   };
					move_list_append(ml, mv);
				}
			}
			break;
		case PLAYER_B:
			if (board_has_castling_rights(board, CASTLE_B_KS)) {
				if (king_sqr == SQ_E8 && board_get_piece(board, SQ_H8) == W_ROOK &&
					(board->occupancies[PLAYER_W] | board->occupancies[PLAYER_B]) &
						~(B_KS_CASTLING_SQUARES)) {
					// checking for threats is deferred to makemove
					Move mv = {.from			= king_sqr,
							   .to				= SQ_C8,
							   .mv_type			= MV_KS_CASTLE,
							   .piece			= piece,
							   };
					move_list_append(ml, mv);
				}
			}
			if (board_has_castling_rights(board, CASTLE_B_QS)) {
				if (king_sqr == SQ_E8 && board_get_piece(board, SQ_A8) == B_ROOK &&
					(board->occupancies[PLAYER_W] | board->occupancies[PLAYER_B]) &
						~(B_QS_CASTLING_SQUARES)) {
					// checking for threats is deferred to makemove
					Move mv = {.from			= king_sqr,
							   .to				= SQ_D8,
							   .mv_type			= MV_QS_CASTLE,
							   .piece			= piece,
							   };
					move_list_append(ml, mv);
				}
			}
			break;
		default:
			assert(false);
	}
}

void movegen_cross(const Board *board, PieceType pt, Player p, MoveList *ml) {
	assert(p != PLAYER_NONE);
	assert(pt == ROOK || pt == QUEEN);
	uint64_t bb = board->pieces[p][pt];
	while (bb) {
		Square sqr	= bits_pop_lsb(&bb);
		int	   rank = sqr / 8;
		int	   file = sqr % 8;
		// up
		for (int r = rank + 1; r < 8; r++) {
			Square up_sqr	= file + 8 * r;
			Player occupant = board_get_occupant(board, up_sqr);
			if (occupant == PLAYER_NONE) {
				Move mv = {.from			= sqr,
						   .to				= up_sqr,
						   .mv_type			= MV_QUIET,
						   .piece			= board_create_piece(p, pt),
						   };
				move_list_append(ml, mv);
			} else if (occupant == p) {
				break;
			} else {
				// enemy
				Move mv = {.from			= sqr,
						   .to				= up_sqr,
						   .mv_type			= MV_CAPTURE,
						   .piece			= board_create_piece(p, pt),
						   };
				move_list_append(ml, mv);
				break;
			}
		}
		// down
		for (int r = rank - 1; r >= 0; r--) {
			Square down_sqr = file + 8 * r;
			Player occupant = board_get_occupant(board, down_sqr);
			if (occupant == PLAYER_NONE) {
				Move mv = {.from			= sqr,
						   .to				= down_sqr,
						   .mv_type			= MV_QUIET,
						   .piece			= board_create_piece(p, pt),
						   };
				move_list_append(ml, mv);
			} else if (occupant == p) {
				break;
			} else {
				// enemy
				Move mv = {.from			= sqr,
						   .to				= down_sqr,
						   .mv_type			= MV_CAPTURE,
						   .piece			= board_create_piece(p, pt),
						   };
				move_list_append(ml, mv);
				break;
			}
		}
		// left
		for (int f = file - 1; f >= 0; f--) {
			Square left_sqr = 8 * rank + f;
			Player occupant = board_get_occupant(board, left_sqr);
			if (occupant == PLAYER_NONE) {
				Move mv = {.from			= sqr,
						   .to				= left_sqr,
						   .mv_type			= MV_QUIET,
						   .piece			= board_create_piece(p, pt),
						   };
				move_list_append(ml, mv);
			} else if (occupant == p) {
				break;
			} else {
				// enemy
				Move mv = {.from			= sqr,
						   .to				= left_sqr,
						   .mv_type			= MV_CAPTURE,
						   .piece			= board_create_piece(p, pt),
						   };
				move_list_append(ml, mv);
				break;
			}
		}
		// right
		for (int f = file + 1; f < 8; f++) {
			Square right_sqr = 8 * rank + f;
			Player occupant	 = board_get_occupant(board, right_sqr);
			if (occupant == PLAYER_NONE) {
				Move mv = {.from			= sqr,
						   .to				= right_sqr,
						   .mv_type			= MV_QUIET,
						   .piece			= board_create_piece(p, pt),
						   };
				move_list_append(ml, mv);
			} else if (occupant == p) {
				break;
			} else {
				// enemy
				Move mv = {.from			= sqr,
						   .to				= right_sqr,
						   .mv_type			= MV_CAPTURE,
						   .piece			= board_create_piece(p, pt),
						   };
				move_list_append(ml, mv);
				break;
			}
		}
	}
}

void movegen_diagonal(const Board *board, PieceType pt, Player p, MoveList *ml) {
	assert(p != PLAYER_NONE);
	assert(pt == BISHOP || pt == QUEEN);
	uint64_t bb = board->pieces[p][pt];
	while (bb) {
		Square sqr	= bits_pop_lsb(&bb);
		int	   rank = sqr / 8;
		int	   file = sqr % 8;
		// up-right
		for (int r = rank + 1, f = file + 1; r < 8 && f < 8; r++, f++) {
			Square up_right_sqr = r * 8 + f;
			Player occupant		= board_get_occupant(board, up_right_sqr);
			if (occupant == PLAYER_NONE) {
				Move mv = {.from			= sqr,
						   .to				= up_right_sqr,
						   .mv_type			= MV_QUIET,
						   .piece			= board_create_piece(p, pt),
						   };
				move_list_append(ml, mv);
			} else if (occupant == p) {
				break;
			} else {
				// enemy
				Move mv = {.from			= sqr,
						   .to				= up_right_sqr,
						   .mv_type			= MV_CAPTURE,
						   .piece			= board_create_piece(p, pt),
						   };
				move_list_append(ml, mv);
				break;
			}
		}
		// down-right
		for (int r = rank - 1, f = file + 1; r >= 0 && f >= 0; r--, f++) {
			Square down_right_sqr = r * 8 + f;
			Player occupant		  = board_get_occupant(board, down_right_sqr);
			if (occupant == PLAYER_NONE) {
				Move mv = {.from			= sqr,
						   .to				= down_right_sqr,
						   .mv_type			= MV_QUIET,
						   .piece			= board_create_piece(p, pt),
						   };
				move_list_append(ml, mv);
			} else if (occupant == p) {
				break;
			} else {
				// enemy
				Move mv = {.from			= sqr,
						   .to				= down_right_sqr,
						   .mv_type			= MV_CAPTURE,
						   .piece			= board_create_piece(p, pt),
						   };
				move_list_append(ml, mv);
				break;
			}
		}
		// up-left
		for (int r = rank + 1, f = file - 1; r < 8 && f >= 0; r++, f--) {
			Square up_left_sqr = r * 8 + file;
			Player occupant	   = board_get_occupant(board, up_left_sqr);
			if (occupant == PLAYER_NONE) {
				Move mv = {.from			= sqr,
						   .to				= up_left_sqr,
						   .mv_type			= MV_QUIET,
						   .piece			= board_create_piece(p, pt),
						   };
				move_list_append(ml, mv);
			} else if (occupant == p) {
				break;
			} else {
				// enemy
				Move mv = {.from			= sqr,
						   .to				= up_left_sqr,
						   .mv_type			= MV_CAPTURE,
						   .piece			= board_create_piece(p, pt),
						   };
				move_list_append(ml, mv);
				break;
			}
		}
		// down-left
		for (int r = rank - 1, f = file - 1; r >= 0 && f >= 0; r--, f--) {
			Square down_left_sqr = r * 8 + file;
			Player occupant		 = board_get_occupant(board, down_left_sqr);
			if (occupant == PLAYER_NONE) {
				Move mv = {.from			= sqr,
						   .to				= down_left_sqr,
						   .mv_type			= MV_QUIET,
						   .piece			= board_create_piece(p, pt),
						   };
				move_list_append(ml, mv);
			} else if (occupant == p) {
				break;
			} else {
				// enemy
				Move mv = {.from	= sqr,
						   .to		= down_left_sqr,
						   .mv_type = MV_CAPTURE,
						   .piece	= board_create_piece(p, pt)};
				move_list_append(ml, mv);
				break;
			}
		}
	}
}

MoveList *movegen_generate(const Board *board, Player p) {
	MoveList *ml = NULL;
	move_list_create(&ml);
	movegen_pawns(board, PAWN, p, ml);
	movegen_knights(board, KNIGHT, p, ml);
	movegen_king(board, KING, p, ml);
	movegen_cross(board, ROOK, p, ml);
	movegen_diagonal(board, BISHOP, p, ml);
	movegen_diagonal(board, QUEEN, p, ml);
	movegen_cross(board, QUEEN, p, ml);
	return ml;
}
