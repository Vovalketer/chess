#include "bits.h"
#include "board.h"
#include "utils.h"

typedef enum {
	MAT_PAWN   = 100,
	MAT_KNIGHT = 320,
	MAT_BISHOP = 330,
	MAT_ROOK   = 500,
	MAT_QUEEN  = 900,
	MAT_KING   = 20000
} MaterialVal;

int eval(Board* board) {
	Player player	= board->side;
	Player opponent = utils_get_opponent(board->side);

	int mat_score = 0;
	mat_score += (bits_get_popcount(board->pieces[player][PAWN]) -
				  bits_get_popcount(board->pieces[opponent][PAWN])) *
				 MAT_PAWN;
	mat_score += (bits_get_popcount(board->pieces[player][KNIGHT]) -
				  bits_get_popcount(board->pieces[opponent][KNIGHT])) *
				 MAT_KNIGHT;
	mat_score += (bits_get_popcount(board->pieces[player][BISHOP]) -
				  bits_get_popcount(board->pieces[opponent][BISHOP])) *
				 MAT_BISHOP;
	mat_score += (bits_get_popcount(board->pieces[player][ROOK]) -
				  bits_get_popcount(board->pieces[opponent][ROOK])) *
				 MAT_ROOK;
	mat_score += (bits_get_popcount(board->pieces[player][QUEEN]) -
				  bits_get_popcount(board->pieces[opponent][QUEEN])) *
				 MAT_QUEEN;
	mat_score += (bits_get_popcount(board->pieces[player][KING]) -
				  bits_get_popcount(board->pieces[opponent][KING])) *
				 MAT_KING;

	return mat_score;
}
