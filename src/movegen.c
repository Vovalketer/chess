#include "movegen.h"

#include <assert.h>
#include <stdbool.h>
#include <stdint.h>

#include "bitboards.h"
#include "bits.h"
#include "board.h"
#include "log.h"
#include "movelist.h"
#include "types.h"
#include "utils.h"
#define RANK_1				  0x00000000000000FFULL
#define RANK_2				  0xFF00ULL
#define RANK_3				  0xFF0000ULL
#define RANK_4				  0xFF000000ULL
#define RANK_5				  0xFF00000000ULL
#define RANK_6				  0xFF0000000000ULL
#define RANK_7				  0xFF000000000000ULL
#define RANK_8				  0xFF00000000000000ULL
#define NOT_RANK_1			  0xFFFFFFFFFFFFFF00ULL
#define NOT_RANK_8			  0x00FFFFFFFFFFFFFFULL
#define W_QS_CASTLING_SQUARES 0xEULL
#define B_QS_CASTLING_SQUARES 0xE00000000000000ULL
#define W_KS_CASTLING_SQUARES 0x60ULL
#define B_KS_CASTLING_SQUARES 0x6000000000000000ULL

void movegen_pawns(const Board *board, PieceType pt, Player p, MoveList *ml) {
	assert(p != PLAYER_NONE);
	uint64_t bb			 = board->pieces[p][pt];
	Player	 opponent	 = utils_get_opponent(p);
	uint64_t occupancies = board->occupancies[p] | board->occupancies[utils_get_opponent(p)];
	while (bb) {
		Square	 sqr	 = bits_pop_lsb(&bb);  // pop and get the index, transform into a square
		uint64_t attk	 = bitboards_get_pawn_attacks(sqr, p) & board->occupancies[opponent];
		bool	 is_prom = (p == PLAYER_W && sqr > SQ_H7) || (p == PLAYER_B && sqr < SQ_A2);
		while (attk) {
			Square attk_sqr = bits_pop_lsb(&attk);
			if (!is_prom) {
				Move mv = {
					.from	 = sqr,
					.to		 = attk_sqr,
					.mv_type = MV_CAPTURE,
					.piece	 = pt,
				};
				move_list_append(ml, mv);
			} else {
				Move prom_q = {
					.from	 = sqr,
					.to		 = attk_sqr,
					.mv_type = MV_Q_PROM_CAPTURE,
					.piece	 = pt,
				};
				Move prom_r = {
					.from	 = sqr,
					.to		 = attk_sqr,
					.mv_type = MV_R_PROM_CAPTURE,
					.piece	 = pt,
				};
				Move prom_b = {
					.from	 = sqr,
					.to		 = attk_sqr,
					.mv_type = MV_B_PROM_CAPTURE,
					.piece	 = pt,
				};
				Move prom_n = {
					.from	 = sqr,
					.to		 = attk_sqr,
					.mv_type = MV_N_PROM_CAPTURE,
					.piece	 = pt,
				};
				move_list_append(ml, prom_q);
				move_list_append(ml, prom_r);
				move_list_append(ml, prom_b);
				move_list_append(ml, prom_n);
			}
		}
		uint64_t pushes = bitboards_get_pawn_pushes(sqr, p) & ~occupancies;
		while (pushes) {
			Square push_sqr = bits_pop_lsb(&pushes);
			if (!is_prom) {
				Move mv = {
					.from	 = sqr,
					.to		 = push_sqr,
					.mv_type = MV_QUIET,
					.piece	 = pt,
				};
				move_list_append(ml, mv);
			} else {
				Move prom_q = {
					.from	 = sqr,
					.to		 = push_sqr,
					.mv_type = MV_Q_PROM,
					.piece	 = pt,
				};
				Move prom_r = {
					.from	 = sqr,
					.to		 = push_sqr,
					.mv_type = MV_R_PROM,
					.piece	 = pt,
				};
				Move prom_b = {
					.from	 = sqr,
					.to		 = push_sqr,
					.mv_type = MV_B_PROM,
					.piece	 = pt,
				};
				Move prom_n = {
					.from	 = sqr,
					.to		 = push_sqr,
					.mv_type = MV_N_PROM,
					.piece	 = pt,
				};
				move_list_append(ml, prom_q);
				move_list_append(ml, prom_r);
				move_list_append(ml, prom_b);
				move_list_append(ml, prom_n);
			}
		}

		uint64_t double_pushes =
			bitboards_get_pawn_pushes(sqr, p) & ~occupancies & ~(p == PLAYER_W ? RANK_3 : RANK_6);
		while (double_pushes) {
			Square push_sqr = bits_pop_lsb(&double_pushes);
			Move   mv		= {
						.from	 = sqr,
						.to		 = push_sqr,
						.mv_type = MV_PAWN_DOUBLE,
						.piece	 = pt,
			};
			move_list_append(ml, mv);
		}
		uint64_t ep_eval = 0;
		bits_set(&ep_eval, board->ep_target);
		if (ep_eval & bitboards_get_pawn_attacks(sqr, p)) {
			Move mv = {
				.from	 = sqr,
				.to		 = board->ep_target,
				.mv_type = MV_EN_PASSANT,
				.piece	 = pt,
			};
			move_list_append(ml, mv);
		}
	}
}

void movegen_knights(const Board *board, PieceType pt, Player p, MoveList *ml) {
	assert(p != PLAYER_NONE);
	uint64_t bb = board->pieces[p][pt];
	while (bb) {
		Square	 sqr   = bits_pop_lsb(&bb);
		uint64_t moves = bitboards_get_knight_attacks(sqr) & ~board->occupancies[p];
		MoveType mt	   = board_has_enemy(board, sqr, p) ? MV_CAPTURE : MV_QUIET;
		while (moves) {
			Square to = bits_pop_lsb(&moves);
			Move   mv = {
				  .from	   = sqr,
				  .to	   = to,
				  .mv_type = mt,
				  .piece   = pt,
			  };
			move_list_append(ml, mv);
		}
	}
}

void movegen_king(const Board *board, PieceType pt, Player p, MoveList *ml) {
	assert(p != PLAYER_NONE);
	uint64_t bb		  = board->pieces[p][pt];
	Square	 king_sqr = bits_pop_lsb(&bb);
	uint64_t moves	  = bitboards_get_king_attacks(king_sqr) & ~board->occupancies[p];
	while (moves) {
		Square	 to = bits_pop_lsb(&moves);
		MoveType mt = board_has_enemy(board, king_sqr, p) ? MV_CAPTURE : MV_QUIET;
		Move	 mv = {
				.from	 = king_sqr,
				.to		 = to,
				.mv_type = mt,
				.piece	 = pt,
		};
		move_list_append(ml, mv);
	}

	switch (p) {
		case PLAYER_W:
			if (board_has_castling_rights(board, CASTLING_WHITE_KS)) {
				if (king_sqr == SQ_E1 && board_get_piece_type(board, SQ_H1) == ROOK &&
					bits_get(board->occupancies[PLAYER_W], SQ_H1) &&
					(board->occupancies[PLAYER_W] | board->occupancies[PLAYER_B]) &
						~(W_KS_CASTLING_SQUARES)) {
					// checking for threats is deferred to makemove
					Move mv = {
						.from	 = king_sqr,
						.to		 = SQ_G1,
						.mv_type = MV_KS_CASTLE,
						.piece	 = pt,
					};
					move_list_append(ml, mv);
				}
			}
			if (board_has_castling_rights(board, CASTLING_WHITE_QS)) {
				if (king_sqr == SQ_E1 && board_get_piece_type(board, SQ_A1) == ROOK &&
					bits_get(board->occupancies[PLAYER_W], SQ_A1) &&
					(board->occupancies[PLAYER_W] | board->occupancies[PLAYER_B]) &
						~(W_QS_CASTLING_SQUARES)) {
					// checking for threats is deferred to makemove
					Move mv = {
						.from	 = king_sqr,
						.to		 = SQ_C1,
						.mv_type = MV_QS_CASTLE,
						.piece	 = pt,
					};
					move_list_append(ml, mv);
				}
			}
			break;
		case PLAYER_B:
			if (board_has_castling_rights(board, CASTLING_BLACK_KS)) {
				if (king_sqr == SQ_E8 && board_get_piece_type(board, SQ_H8) == ROOK &&
					bits_get(board->occupancies[PLAYER_W], SQ_H8) &&
					(board->occupancies[PLAYER_W] | board->occupancies[PLAYER_B]) &
						~(B_KS_CASTLING_SQUARES)) {
					// checking for threats is deferred to makemove
					Move mv = {
						.from	 = king_sqr,
						.to		 = SQ_C8,
						.mv_type = MV_KS_CASTLE,
						.piece	 = pt,
					};
					move_list_append(ml, mv);
				}
			}
			if (board_has_castling_rights(board, CASTLING_BLACK_QS)) {
				if (king_sqr == SQ_E8 && board_get_piece_type(board, SQ_A8) == ROOK &&
					bits_get(board->occupancies[PLAYER_W], SQ_A8) &&
					(board->occupancies[PLAYER_W] | board->occupancies[PLAYER_B]) &
						~(B_QS_CASTLING_SQUARES)) {
					// checking for threats is deferred to makemove
					Move mv = {
						.from	 = king_sqr,
						.to		 = SQ_D8,
						.mv_type = MV_QS_CASTLE,
						.piece	 = pt,
					};
					move_list_append(ml, mv);
				}
			}
			break;
		default:
			assert(false);
	}
}

void movegen_rooks(const Board *board, PieceType pt, Player p, MoveList *ml) {
	assert(p != PLAYER_NONE);
	uint64_t bb			 = board->pieces[p][pt];
	Player	 opponent	 = utils_get_opponent(p);
	uint64_t occupancies = board->occupancies[p] | board->occupancies[opponent];
	while (bb) {
		Square	 sqr	  = bits_pop_lsb(&bb);
		uint64_t attacks  = bitboards_get_rook_attacks(sqr, occupancies);
		uint64_t moves	  = attacks & ~occupancies;
		uint64_t captures = attacks & board->occupancies[opponent];
		while (moves) {
			Square to = bits_pop_lsb(&moves);
			Move   mv = {
				  .from	   = sqr,
				  .to	   = to,
				  .mv_type = MV_QUIET,
				  .piece   = pt,
			  };
			move_list_append(ml, mv);
		}
		while (captures) {
			Square to = bits_pop_lsb(&captures);
			Move   mv = {
				  .from	   = sqr,
				  .to	   = to,
				  .mv_type = MV_CAPTURE,
				  .piece   = pt,
			  };
			move_list_append(ml, mv);
		}
	}
}

void movegen_bishops(const Board *board, PieceType pt, Player p, MoveList *ml) {
	assert(p != PLAYER_NONE);
	uint64_t bb			 = board->pieces[p][pt];
	Player	 opponent	 = utils_get_opponent(p);
	uint64_t occupancies = board->occupancies[p] | board->occupancies[opponent];
	while (bb) {
		Square	 sqr	  = bits_pop_lsb(&bb);
		uint64_t attacks  = bitboards_get_bishop_attacks(sqr, occupancies);
		uint64_t moves	  = attacks & ~occupancies;
		uint64_t captures = attacks & board->occupancies[opponent];
		while (moves) {
			Square to = bits_pop_lsb(&moves);
			Move   mv = {
				  .from	   = sqr,
				  .to	   = to,
				  .mv_type = MV_QUIET,
				  .piece   = pt,
			  };
			move_list_append(ml, mv);
		}
		while (captures) {
			Square to = bits_pop_lsb(&captures);
			Move   mv = {
				  .from	   = sqr,
				  .to	   = to,
				  .mv_type = MV_CAPTURE,
				  .piece   = pt,
			  };
			move_list_append(ml, mv);
		}
	}
}

void movegen_queens(const Board *board, PieceType pt, Player p, MoveList *ml) {
	assert(p != PLAYER_NONE);
	uint64_t bb			 = board->pieces[p][pt];
	Player	 opponent	 = utils_get_opponent(p);
	uint64_t occupancies = board->occupancies[p] | board->occupancies[opponent];
	while (bb) {
		Square	 sqr	  = bits_pop_lsb(&bb);
		uint64_t attacks  = bitboards_get_queen_attacks(sqr, occupancies);
		uint64_t moves	  = attacks & ~occupancies;
		uint64_t captures = attacks & board->occupancies[opponent];
		while (moves) {
			Square to = bits_pop_lsb(&moves);
			Move   mv = {
				  .from	   = sqr,
				  .to	   = to,
				  .mv_type = MV_QUIET,
				  .piece   = pt,
			  };
			move_list_append(ml, mv);
		}
		while (captures) {
			Square to = bits_pop_lsb(&captures);
			Move   mv = {
				  .from	   = sqr,
				  .to	   = to,
				  .mv_type = MV_CAPTURE,
				  .piece   = pt,
			  };
			move_list_append(ml, mv);
		}
	}
}

MoveList *movegen_generate(const Board *board, Player p) {
	MoveList *ml = NULL;
	move_list_create(&ml);
	movegen_pawns(board, PAWN, p, ml);
	movegen_knights(board, KNIGHT, p, ml);
	movegen_king(board, KING, p, ml);
	movegen_rooks(board, ROOK, p, ml);
	movegen_bishops(board, BISHOP, p, ml);
	movegen_queens(board, QUEEN, p, ml);
	return ml;
}
