#include <stdint.h>

#include "bits.h"
#include "board.h"
#include "types.h"
#include "utils.h"

typedef enum {
	MAT_PAWN   = 100,
	MAT_KNIGHT = 320,
	MAT_BISHOP = 330,
	MAT_ROOK   = 500,
	MAT_QUEEN  = 900,
	MAT_KING   = 20000
} MaterialVal;

const int material_values[] = {MAT_PAWN, MAT_ROOK, MAT_KNIGHT, MAT_BISHOP, MAT_QUEEN, MAT_KING};

const int pawn_pos_table[] = {
	// clang-format off
	 0,  0,  0,  0,  0,  0,  0,  0,
	50, 50, 50, 50, 50, 50, 50, 50,
	10, 10, 20, 30, 30, 20, 10, 10,
	 5,  5, 10, 25, 25, 10,  5,  5,
	 0,  0,  0, 20, 20,  0,  0,  0,
	 5, -5,-10,  0,  0,-10, -5,  5,
	 5, 10, 10,-20,-20, 10, 10,  5,
	 0,  0,  0,  0,  0,  0,  0,  0,
	// clang-format on
};

const int knight_pos_table[] = {
	// clang-format off
	-50,-40,-30,-30,-30,-30,-40,-50,
	-40,-20,  0,  0,  0,  0,-20,-40,
	-30,  0, 10, 15, 15, 10,  0,-30,
	-30,  5, 15, 20, 20, 15,  5,-30,
	-30,  0, 15, 20, 20, 15,  0,-30,
	-30,  5, 10, 15, 15, 10,  5,-30,
	-40,-20,  0,  5,  5,  0,-20,-40,
	-50,-40,-30,-30,-30,-30,-40,-50,
	// clang-format on
};

const int bishop_pos_table[] = {
	// clang-format off
	-20,-10,-10,-10,-10,-10,-10,-20,
	-10,  0,  0,  0,  0,  0,  0,-10,
	-10,  0,  5, 10, 10,  5,  0,-10,
	-10,  5,  5, 10, 10,  5,  5,-10,
	-10,  0, 10, 10, 10, 10,  0,-10,
	-10, 10, 10, 10, 10, 10, 10,-10,
	-10,  5,  0,  0,  0,  0,  5,-10,
	-20,-10,-10,-10,-10,-10,-10,-20,
	// clang-format on
};

const int rook_pos_table[] = {
	// clang-format off
	 0,  0,  0,  0,  0,  0,  0,  0,
	 5, 10, 10, 10, 10, 10, 10,  5,
	-5,  0,  0,  0,  0,  0,  0, -5,
	-5,  0,  0,  0,  0,  0,  0, -5,
	-5,  0,  0,  0,  0,  0,  0, -5,
	-5,  0,  0,  0,  0,  0,  0, -5,
	-5,  0,  0,  0,  0,  0,  0, -5,
	 0,  0,  0,  5,  5,  0,  0,  0,
	// clang-format on
};

const int queen_pos_table[] = {
	// clang-format off
	-20,-10,-10, -5, -5,-10,-10,-20,
	-10,  0,  0,  0,  0,  0,  0,-10,
	-10,  0,  5,  5,  5,  5,  0,-10,
	 -5,  0,  5,  5,  5,  5,  0, -5,
	  0,  0,  5,  5,  5,  5,  0, -5,
	-10,  5,  5,  5,  5,  5,  0,-10,
	-10,  0,  5,  0,  0,  0,  0,-10,
	-20,-10,-10, -5, -5,-10,-10,-20
	// clang-format on
};

const int king_pos_midgame_table[] = {
	// clang-format off
	-30,-40,-40,-50,-50,-40,-40,-30,
	-30,-40,-40,-50,-50,-40,-40,-30,
	-30,-40,-40,-50,-50,-40,-40,-30,
	-30,-40,-40,-50,-50,-40,-40,-30,
	-20,-30,-30,-40,-40,-30,-30,-20,
	-10,-20,-20,-20,-20,-20,-20,-10,
	 20, 20,  0,  0,  0,  0, 20, 20,
	 20, 30, 10,  0,  0, 10, 30, 20,
	// clang-format on
};

const int king_pos_endgame_table[] = {
	// clang-format off
	-50,-40,-30,-20,-20,-30,-40,-50,
	-30,-20,-10,  0,  0,-10,-20,-30,
	-30,-10, 20, 30, 30, 20,-10,-30,
	-30,-10, 30, 40, 40, 30,-10,-30,
	-30,-10, 30, 40, 40, 30,-10,-30,
	-30,-10, 20, 30, 30, 20,-10,-30,
	-30,-30,  0,  0,  0,  0,-30,-30,
	-50,-30,-30,-30,-30,-30,-30,-50
	// clang-format on
};

static int get_pos_value(PieceType type, Player player, Square sqr) {
	Square b_sqr = (7 - utils_get_rank(sqr)) * 7 + utils_get_rank(sqr);
	switch (type) {
		case PAWN:
			return player == PLAYER_W ? pawn_pos_table[sqr] : pawn_pos_table[b_sqr];
		case KNIGHT:
			return player == PLAYER_W ? knight_pos_table[sqr] : knight_pos_table[b_sqr];
		case BISHOP:
			return player == PLAYER_W ? bishop_pos_table[sqr] : bishop_pos_table[b_sqr];
		case ROOK:
			return player == PLAYER_W ? rook_pos_table[sqr] : rook_pos_table[b_sqr];
		case QUEEN:
			return player == PLAYER_W ? queen_pos_table[sqr] : queen_pos_table[b_sqr];
		case KING:
			return player == PLAYER_W ? king_pos_midgame_table[sqr] : king_pos_endgame_table[b_sqr];
		default:
			return 0;
	}
}

int get_score_for_piecetype(Board* board, PieceType type, Player player) {
	uint64_t pieces = board->pieces[player][type];
	int		 val	= 0;
	int		 count	= 0;

	while (pieces) {
		Square sqr = bits_pop_lsb(&pieces);
		val += get_pos_value(type, player, sqr);
		count++;
	}
	return val + count * material_values[type];
}

int eval(Board* board) {
	Player player	= board->side;
	Player opponent = utils_get_opponent(board->side);

	int score = 0;
	score += (get_score_for_piecetype(board, PAWN, player)) -
			 (get_score_for_piecetype(board, PAWN, opponent));
	score += (get_score_for_piecetype(board, KNIGHT, player)) -
			 (get_score_for_piecetype(board, KNIGHT, opponent));
	score += (get_score_for_piecetype(board, BISHOP, player)) -
			 (get_score_for_piecetype(board, BISHOP, opponent));
	score += (get_score_for_piecetype(board, ROOK, player)) -
			 (get_score_for_piecetype(board, ROOK, opponent));
	score += (get_score_for_piecetype(board, QUEEN, player)) -
			 (get_score_for_piecetype(board, QUEEN, opponent));
	score += (get_score_for_piecetype(board, KING, player)) -
			 (get_score_for_piecetype(board, KING, opponent));

	return score;
}
