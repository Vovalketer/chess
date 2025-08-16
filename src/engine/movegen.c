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

static Move move_create(
	const Board *board, Player p, Square from, Square to, PieceType pt, MoveType mv_type) {
	Player	  opponent = utils_get_opponent(p);
	PieceType captured = EMPTY;
	if (mv_type >= MV_CAPTURE) {
		for (PieceType cap = PAWN; cap <= KING; cap++) {
			if (bits_get(board->pieces[opponent][cap], to)) {
				captured = cap;
				break;
			}
		}
	}
	Move mv = {
		.from	  = from,
		.to		  = to,
		.mv_type  = mv_type,
		.captured = captured,
		.piece	  = pt,
	};
	return mv;
}

static void gen_moves_from_mask(Square		   from,
								const uint64_t bb_moves,
								PieceType	   pt,
								const Board	  *board,
								Player		   p,
								MoveList	  *ml) {
	Player	 opponent = utils_get_opponent(p);
	uint64_t moves	  = bb_moves & ~(board->occupancies[p] | board->occupancies[opponent]);
	while (moves) {
		Square to = bits_pop_lsb(&moves);
		Move   mv = move_create(board, p, from, to, pt, MV_QUIET);
		move_list_push_back(ml, mv);
	}
}

static void gen_attacks_from_mask(
	Square from, const uint64_t bb, PieceType pt, const Board *board, Player p, MoveList *ml) {
	Player	 opponent = utils_get_opponent(p);
	uint64_t captures = bb & board->occupancies[opponent];
	while (captures) {
		Square to = bits_pop_lsb(&captures);
		Move   mv = move_create(board, p, from, to, pt, MV_CAPTURE);
		move_list_push_back(ml, mv);
	}
}

void movegen_pawn_attacks(const Board *board, Player p, MoveList *ml) {
	uint64_t pieces	  = board->pieces[p][PAWN];
	Player	 opponent = utils_get_opponent(p);
	while (pieces) {
		Square	 from	  = bits_pop_lsb(&pieces);
		uint64_t captures = bitboards_get_pawn_attacks(from, p) & board->occupancies[opponent];
		bool	 is_prom  = (p == PLAYER_W && (utils_get_rank(from) == 6)) ||
					   (p == PLAYER_B && (utils_get_rank(from) == 1));
		while (captures) {
			Square to = bits_pop_lsb(&captures);
			if (!is_prom) {
				Move mv = move_create(board, p, from, to, PAWN, MV_CAPTURE);
				move_list_push_back(ml, mv);
			} else {
				Move prom_q = move_create(board, p, from, to, PAWN, MV_Q_PROM_CAPTURE);
				Move prom_r = move_create(board, p, from, to, PAWN, MV_R_PROM_CAPTURE);
				Move prom_b = move_create(board, p, from, to, PAWN, MV_B_PROM_CAPTURE);
				Move prom_n = move_create(board, p, from, to, PAWN, MV_N_PROM_CAPTURE);
				move_list_push_back(ml, prom_q);
				move_list_push_back(ml, prom_r);
				move_list_push_back(ml, prom_b);
				move_list_push_back(ml, prom_n);
			}
		}
	}
}

void movegen_pawn_moves(const Board *board, Player p, MoveList *ml) {
	assert(p != PLAYER_NONE);
	PieceType pt		  = PAWN;
	uint64_t  bb		  = board->pieces[p][pt];
	uint64_t  occupancies = board->occupancies[p] | board->occupancies[utils_get_opponent(p)];
	while (bb) {
		Square	 from	 = bits_pop_lsb(&bb);
		uint64_t pushes	 = bitboards_get_pawn_pushes(from, p) & ~occupancies;
		bool	 is_prom = (p == PLAYER_W && (utils_get_rank(from) == 6)) ||
					   (p == PLAYER_B && (utils_get_rank(from) == 1));
		while (pushes) {
			Square push_sqr = bits_pop_lsb(&pushes);
			if (!is_prom) {
				Move mv = move_create(board, p, from, push_sqr, pt, MV_QUIET);
				move_list_push_back(ml, mv);
			} else {
				Move prom_q = move_create(board, p, from, push_sqr, pt, MV_Q_PROM);
				Move prom_r = move_create(board, p, from, push_sqr, pt, MV_R_PROM);
				Move prom_b = move_create(board, p, from, push_sqr, pt, MV_B_PROM);
				Move prom_n = move_create(board, p, from, push_sqr, pt, MV_N_PROM);
				move_list_push_back(ml, prom_q);
				move_list_push_back(ml, prom_r);
				move_list_push_back(ml, prom_b);
				move_list_push_back(ml, prom_n);
			}
		}

		pushes				   = bitboards_get_pawn_pushes(from, p) & ~occupancies;
		uint64_t double_pushes = bitboards_get_pawn_double_pushes(from, p) & ~occupancies;
		if (double_pushes && pushes) {
			while (double_pushes) {
				Square push_sqr = bits_pop_lsb(&double_pushes);
				Move   mv		= move_create(board, p, from, push_sqr, pt, MV_PAWN_DOUBLE);
				move_list_push_back(ml, mv);
			}
		}
	}
}

void movegen_knight_moves(const Board *board, Player p, MoveList *ml) {
	assert(p != PLAYER_NONE);
	PieceType pt = KNIGHT;
	uint64_t  bb = board->pieces[p][pt];
	while (bb) {
		Square	 from  = bits_pop_lsb(&bb);
		uint64_t moves = bitboards_get_knight_attacks(from);
		gen_moves_from_mask(from, moves, pt, board, p, ml);
	}
}

void movegen_knight_attacks(const Board *board, Player p, MoveList *ml) {
	assert(p != PLAYER_NONE);
	PieceType pt = KNIGHT;
	uint64_t  bb = board->pieces[p][pt];
	while (bb) {
		Square	 from  = bits_pop_lsb(&bb);
		uint64_t moves = bitboards_get_knight_attacks(from);
		gen_attacks_from_mask(from, moves, pt, board, p, ml);
	}
}

void movegen_king_moves(const Board *board, Player p, MoveList *ml) {
	assert(p != PLAYER_NONE);
	PieceType pt = KING;
	uint64_t  bb = board->pieces[p][pt];
	if (!bb)
		return;
	Square	 king_sqr = bits_pop_lsb(&bb);
	uint64_t moves	  = bitboards_get_king_attacks(king_sqr);
	gen_moves_from_mask(king_sqr, moves, pt, board, p, ml);
}

void movegen_king_attacks(const Board *board, Player p, MoveList *ml) {
	assert(p != PLAYER_NONE);
	PieceType pt = KING;
	uint64_t  bb = board->pieces[p][pt];
	if (!bb)
		return;
	Square	 king_sqr = bits_pop_lsb(&bb);
	uint64_t moves	  = bitboards_get_king_attacks(king_sqr);
	gen_attacks_from_mask(king_sqr, moves, pt, board, p, ml);
}

void movegen_rook_moves(const Board *board, Player p, MoveList *ml) {
	assert(p != PLAYER_NONE);
	PieceType pt		  = ROOK;
	uint64_t  bb		  = board->pieces[p][pt];
	Player	  opponent	  = utils_get_opponent(p);
	uint64_t  occupancies = board->occupancies[p] | board->occupancies[opponent];
	while (bb) {
		Square	 from  = bits_pop_lsb(&bb);
		uint64_t moves = bitboards_get_rook_attacks(from, occupancies);
		gen_moves_from_mask(from, moves, pt, board, p, ml);
	}
}

void movegen_rook_attacks(const Board *board, Player p, MoveList *ml) {
	assert(p != PLAYER_NONE);
	PieceType pt		  = ROOK;
	uint64_t  bb		  = board->pieces[p][pt];
	Player	  opponent	  = utils_get_opponent(p);
	uint64_t  occupancies = board->occupancies[p] | board->occupancies[opponent];
	while (bb) {
		Square	 from  = bits_pop_lsb(&bb);
		uint64_t moves = bitboards_get_rook_attacks(from, occupancies);
		gen_attacks_from_mask(from, moves, pt, board, p, ml);
	}
}

void movegen_bishop_moves(const Board *board, Player p, MoveList *ml) {
	assert(p != PLAYER_NONE);
	PieceType pt		  = BISHOP;
	uint64_t  bb		  = board->pieces[p][pt];
	Player	  opponent	  = utils_get_opponent(p);
	uint64_t  occupancies = board->occupancies[p] | board->occupancies[opponent];
	while (bb) {
		Square	 from  = bits_pop_lsb(&bb);
		uint64_t moves = bitboards_get_bishop_attacks(from, occupancies);
		gen_moves_from_mask(from, moves, pt, board, p, ml);
	}
}

void movegen_bishop_attacks(const Board *board, Player p, MoveList *ml) {
	assert(p != PLAYER_NONE);
	PieceType pt		  = BISHOP;
	uint64_t  bb		  = board->pieces[p][pt];
	Player	  opponent	  = utils_get_opponent(p);
	uint64_t  occupancies = board->occupancies[p] | board->occupancies[opponent];
	while (bb) {
		Square	 from  = bits_pop_lsb(&bb);
		uint64_t moves = bitboards_get_bishop_attacks(from, occupancies);
		gen_attacks_from_mask(from, moves, pt, board, p, ml);
	}
}

void movegen_queen_moves(const Board *board, Player p, MoveList *ml) {
	assert(p != PLAYER_NONE);
	PieceType pt		  = QUEEN;
	uint64_t  bb		  = board->pieces[p][pt];
	Player	  opponent	  = utils_get_opponent(p);
	uint64_t  occupancies = board->occupancies[p] | board->occupancies[opponent];
	while (bb) {
		Square	 sqr   = bits_pop_lsb(&bb);
		uint64_t moves = bitboards_get_queen_attacks(sqr, occupancies);
		gen_moves_from_mask(sqr, moves, pt, board, p, ml);
	}
}

void movegen_queen_attacks(const Board *board, Player p, MoveList *ml) {
	assert(p != PLAYER_NONE);
	PieceType pt		  = QUEEN;
	uint64_t  bb		  = board->pieces[p][pt];
	Player	  opponent	  = utils_get_opponent(p);
	uint64_t  occupancies = board->occupancies[p] | board->occupancies[opponent];
	while (bb) {
		Square	 sqr   = bits_pop_lsb(&bb);
		uint64_t moves = bitboards_get_queen_attacks(sqr, occupancies);
		gen_attacks_from_mask(sqr, moves, pt, board, p, ml);
	}
}

void movegen_castling_moves(const Board *board, Player p, MoveList *ml) {
	assert(p != PLAYER_NONE);
	PieceType pt	   = KING;
	uint64_t  bb	   = board->pieces[p][pt];
	Square	  king_sqr = bits_pop_lsb(&bb);

	switch (p) {
		case PLAYER_W:
			if (board_has_castling_rights(board, CASTLING_WHITE_KS)) {
				Piece rook = board_get_piece(board, SQ_H1);
				if (king_sqr == SQ_E1 && rook.type == ROOK && rook.player == PLAYER_W &&
					((board->occupancies[PLAYER_W] | board->occupancies[PLAYER_B]) &
					 (W_KS_CASTLING_SQUARES)) == 0) {
					// checking for threats is deferred to makemove
					Move mv = move_create(board, p, king_sqr, SQ_G1, pt, MV_KS_CASTLE);
					move_list_push_back(ml, mv);
				}
			}
			if (board_has_castling_rights(board, CASTLING_WHITE_QS)) {
				Piece rook = board_get_piece(board, SQ_A1);
				if (king_sqr == SQ_E1 && rook.type == ROOK && rook.player == PLAYER_W &&
					((board->occupancies[PLAYER_W] | board->occupancies[PLAYER_B]) &
					 (W_QS_CASTLING_SQUARES)) == 0) {
					// checking for threats is deferred to makemove
					Move mv = move_create(board, p, king_sqr, SQ_C1, pt, MV_QS_CASTLE);
					move_list_push_back(ml, mv);
				}
			}
			break;
		case PLAYER_B:
			if (board_has_castling_rights(board, CASTLING_BLACK_KS)) {
				Piece rook = board_get_piece(board, SQ_H8);
				if (king_sqr == SQ_E8 && rook.type == ROOK && rook.player == PLAYER_B &&
					((board->occupancies[PLAYER_W] | board->occupancies[PLAYER_B]) &
					 (B_KS_CASTLING_SQUARES)) == 0) {
					// checking for threats is deferred to makemove
					Move mv = move_create(board, p, king_sqr, SQ_G8, pt, MV_KS_CASTLE);
					move_list_push_back(ml, mv);
				}
			}
			if (board_has_castling_rights(board, CASTLING_BLACK_QS)) {
				Piece rook = board_get_piece(board, SQ_A8);
				if (king_sqr == SQ_E8 && rook.type == ROOK && rook.player == PLAYER_B &&
					((board->occupancies[PLAYER_W] | board->occupancies[PLAYER_B]) &
					 (B_QS_CASTLING_SQUARES)) == 0) {
					// checking for threats is deferred to makemove
					Move mv = move_create(board, p, king_sqr, SQ_C8, pt, MV_QS_CASTLE);
					move_list_push_back(ml, mv);
				}
			}
			break;
		default:
			assert(false);
	}
}

MoveList *movegen_generate(const Board *board, Player p) {
	MoveList *ml = move_list_create();
	movegen_pawn_attacks(board, p, ml);
	movegen_rook_attacks(board, p, ml);
	movegen_bishop_attacks(board, p, ml);
	movegen_queen_attacks(board, p, ml);
	movegen_knight_attacks(board, p, ml);
	movegen_king_attacks(board, p, ml);
	movegen_pawn_moves(board, p, ml);
	movegen_rook_moves(board, p, ml);
	movegen_bishop_moves(board, p, ml);
	movegen_queen_moves(board, p, ml);
	movegen_knight_moves(board, p, ml);
	movegen_king_moves(board, p, ml);
	movegen_castling_moves(board, p, ml);
	return ml;
}

MoveList *movegen_generate_moves(const Board *board, Player p) {
	MoveList *ml = move_list_create();
	movegen_pawn_moves(board, p, ml);
	movegen_rook_moves(board, p, ml);
	movegen_bishop_moves(board, p, ml);
	movegen_queen_moves(board, p, ml);
	movegen_knight_moves(board, p, ml);
	movegen_king_moves(board, p, ml);
	return ml;
}

MoveList *movegen_generate_captures(const Board *board, Player p) {
	MoveList *ml = move_list_create();
	movegen_pawn_attacks(board, p, ml);
	movegen_rook_attacks(board, p, ml);
	movegen_bishop_attacks(board, p, ml);
	movegen_queen_attacks(board, p, ml);
	movegen_knight_attacks(board, p, ml);
	movegen_king_attacks(board, p, ml);
	return ml;
}
