#include "search.h"

#include <limits.h>

#include "board.h"
#include "eval.h"
#include "makemove.h"
#include "movegen.h"
#include "movelist.h"
#define INF		  (INT_MAX - 1)
#define CHECKMATE (INF - 1000)

int alpha_beta(Board* board, int depth, int alpha, int beta) {
	if (depth == 0) {
		return eval(board);
	}
	MoveList* moves = movegen_generate(board, board->side);

	int best_score	= -INF;
	int legal_moves = 0;
	for (size_t i = 0; i < move_list_size(moves); i++) {
		Move move;
		move_list_get(moves, i, &move);

		if (!make_move(board, move))
			continue;
		int score = -alpha_beta(board, depth - 1, -beta, -alpha);
		legal_moves++;
		unmake_move(board);

		if (score > best_score)
			best_score = score;
		if (score > alpha)
			alpha = score;
		if (alpha >= beta)
			break;
	}
	move_list_destroy(&moves);

	if (legal_moves == 0) {
		if (board_is_check(board, board->side)) {
			return -CHECKMATE + depth;
		} else {
			// stalemate
			return 0;
		}
	}
	return best_score;
}

Move search_best_move(Board* board, int depth) {
	MoveList* moves = movegen_generate(board, board->side);
	Move	  best_move;
	int		  best_score = -INF;
	for (size_t i = 0; i < move_list_size(moves); i++) {
		Move move;
		move_list_get(moves, i, &move);

		if (!make_move(board, move))
			continue;

		int score = -alpha_beta(board, depth - 1, -INF, INF);
		unmake_move(board);

		if (score > best_score) {
			best_move  = move;
			best_score = score;
		}
	}

	move_list_destroy(&moves);
	return best_move;
}
