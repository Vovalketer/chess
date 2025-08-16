#include "search.h"

#include <limits.h>

#include "board.h"
#include "eval.h"
#include "log.h"
#include "makemove.h"
#include "movegen.h"
#include "movelist.h"
#include "types.h"
#include "utils.h"

#define INF		  (INT_MAX - 10000)
#define CHECKMATE (INF - 100000)
#define MAX_DEPTH 64

static const int mvv_lva[PIECE_TYPE_CNT][PIECE_TYPE_CNT] = {
	// Victim:  P    N    B    R    Q    K
	{105, 205, 305, 405, 505, 605}, // Attacker: P
	{104, 204, 304, 404, 504, 604}, // N
	{103, 203, 303, 403, 503, 603}, // B
	{102, 202, 302, 402, 502, 602}, // R
	{101, 201, 301, 401, 501, 601}, // Q
	{100, 200, 300, 400, 500, 600}, // K
};

typedef struct {
	Move	 pv_table[MAX_DEPTH][MAX_DEPTH];  // stack allocated matrix for fast access
	uint8_t	 pv_length[MAX_DEPTH];
	uint64_t nodes;
} SearchContext;

static void search_ctx_init(SearchContext* ctx) {
	memset(ctx->pv_length, 0, sizeof(ctx->pv_length));
	ctx->nodes = 0;
}

static void search_ctx_free(SearchContext* ctx) {
	memset(ctx->pv_length, 0, sizeof(ctx->pv_length));
	ctx->nodes = 0;
}

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

int alpha_beta(SearchContext* ctx, Board* board, int depth, int alpha, int beta, int ply) {
	if (depth == 0) {
		return eval(board);
	}
	ctx->pv_length[ply] = 0;
	MoveList* moves		= movegen_generate(board, board->side);

	int best_score	= -INF;
	int legal_moves = 0;
	move_list_sort(moves, mvv_lva_compare);
	for (size_t i = 0; i < move_list_size(moves); i++) {
		Move move = *move_list_get(moves, i);
		if (!make_move(board, move))
			continue;
		int score = -alpha_beta(ctx, board, depth - 1, -beta, -alpha, ply + 1);
		legal_moves++;
		ctx->nodes++;
		unmake_move(board);

		if (score > best_score) {
			best_score			  = score;
			ctx->pv_table[ply][0] = move;

			if (ctx->pv_length[ply + 1] > 0) {
				// copy the child's PV
				memcpy(&ctx->pv_table[ply][1],
					   &ctx->pv_table[ply + 1][0],
					   sizeof(move) * ctx->pv_length[ply + 1]);
			}
			ctx->pv_length[ply] = 1 + ctx->pv_length[ply + 1];
		}
		if (score > alpha)
			alpha = score;
		if (score >= beta)
			break;
	}
	move_list_destroy(&moves);

	if (legal_moves == 0) {
		if (board_is_check(board, board->side)) {
			// shorter mate preferred
			return -CHECKMATE + ply;
		} else {
			// stalemate
			return 0;
		}
	}
	return best_score;
}

Move search_best_move(Board* board, int max_depth) {
	Move	 best_move;
	MoveList root_pv;
	move_list_init(&root_pv);
	SearchContext ctx;
	search_ctx_init(&ctx);

	// iterative deepening
	for (int depth = 1; depth <= max_depth; depth++) {
		// reset pvs for this search
		memset(&ctx.pv_length, 0, sizeof(ctx.pv_length));

		int score = -alpha_beta(&ctx, board, depth, -INF, INF, 0);
		log_info("info depth %d score %d nodes %llu", depth, score, ctx.nodes);
		move_list_clear(&root_pv);
		for (int i = 0; i < ctx.pv_length[0]; i++) {
			move_list_push_back(&root_pv, ctx.pv_table[0][i]);
		}
		if (move_list_size(&root_pv) > 0) {
			best_move = *move_list_get(&root_pv, 0);
		}
		log_info("PV:");
		for (size_t i = 0; i < move_list_size(&root_pv); i++) {
			Move move = *move_list_get(&root_pv, i);
			log_info("%s", utils_move_description(board, move).str);
		}
	}
	search_ctx_free(&ctx);
	return best_move;
}
