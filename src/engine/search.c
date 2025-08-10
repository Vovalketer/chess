#include "search.h"

#include <limits.h>

#include "board.h"
#include "eval.h"
#include "log.h"
#include "makemove.h"
#include "movegen.h"
#include "movelist.h"
#include "types.h"
#define INF		  (INT_MAX - 1)
#define CHECKMATE (INF - 1000)

static const int mvv_lva[PIECE_TYPE_CNT][PIECE_TYPE_CNT] = {
	// Victim:  P    N    B    R    Q    K
	{105, 205, 305, 405, 505, 605}, // Attacker: P
	{104, 204, 304, 404, 504, 604}, // N
	{103, 203, 303, 403, 503, 603}, // B
	{102, 202, 302, 402, 502, 602}, // R
	{101, 201, 301, 401, 501, 601}, // Q
	{100, 200, 300, 400, 500, 600}, // K
};

static int mvv_lva_get(PieceType victim, PieceType attacker) {
	return mvv_lva[attacker][victim];
}

static int mvv_lva_compare(const void* x, const void* y) {
	Move* xt	= (Move*) x;
	Move* yt	= (Move*) y;
	int	  x_val = mvv_lva_get(xt->captured, xt->piece);
	int	  y_val = mvv_lva_get(yt->captured, yt->piece);
	if (x_val < y_val)
		return 1;
	else if (x_val > y_val)
		return -1;
	return 0;
}

int quiescence(Board* board, int alpha, int beta) {
	int stand_pat = eval(board);
	if (stand_pat >= beta)
		return beta;
	if (stand_pat > alpha)
		alpha = stand_pat;

	MoveList* moves = movegen_generate_captures(board, board->side);
	move_list_sort(moves, mvv_lva_compare);
	for (size_t i = 0; i < move_list_size(moves); i++) {
		Move move;
		move = *move_list_get(moves, i);

		if (!make_move(board, move))
			continue;
		int score = -quiescence(board, -beta, -alpha);
		unmake_move(board);
		if (score >= beta)
			return beta;
		if (score > alpha)
			alpha = score;
	}
	move_list_destroy(&moves);
	return alpha;
}

int alpha_beta(Board* board, int depth, int alpha, int beta) {
	if (depth == 0) {
		return quiescence(board, alpha, beta);
	}
	MoveList* moves = movegen_generate(board, board->side);

	int best_score	= -INF;
	int legal_moves = 0;
	for (size_t i = 0; i < move_list_size(moves); i++) {
		Move move;
		move = *move_list_get(moves, i);

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
		move = *move_list_get(moves, i);

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
