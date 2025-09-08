#include "search.h"

#include <limits.h>

#include "board.h"
#include "engine.h"
#include "eval.h"
#include "log.h"
#include "makemove.h"
#include "movegen.h"
#include "movelist.h"
#include "transposition.h"
#include "types.h"
#include "utils.h"
#include "vector.h"

#define INF		  (INT_MAX - 100000)
#define CHECKMATE (INF - 1000000)
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

typedef enum {
	SCORE_CAPTURE		= 10000,
	SCORE_KILLER_FIRST	= 9000,
	SCORE_KILLER_SECOND = 8000,
	SCORE_HISTORY		= 5000,
} ScoreType;

typedef struct {
	Move move;
	int	 score;
} ScoredMove;

typedef struct {
	int		 ply;
	Player	 side;
	uint64_t nodes;
} SearchContext;

volatile bool  stop = false;
Move		   pv_table[MAX_DEPTH][MAX_DEPTH];
uint8_t		   pv_length[MAX_DEPTH];
Move		   killer_moves[MAX_DEPTH][2];
int			   history_heuristic[PLAYER_CNT][SQ_CNT][SQ_CNT];  // player, from, to
MoveList	   root_pv;

static int mvv_lva_compare(const void* x, const void* y) {
	Move* xt	= (Move*) x;
	Move* yt	= (Move*) y;
	int	  x_val = mvv_lva[xt->piece.type][xt->captured_type];
	int	  y_val = mvv_lva[yt->piece.type][yt->captured_type];
	if (x_val < y_val)
		return 1;
	else if (x_val > y_val)
		return -1;
	return 0;
}

static bool is_repetition(Board* board) {
	int count = 0;
	for (size_t i = 0; i < history_size(board->history); i++) {
		History* h = history_get(board->history, i);
		assert(h != NULL);
		if (h->hash == board->hash) {
			count++;
		}
	}
	return count >= 2;
}

static void score_move(Move* move, SearchContext* ctx) {
	if (move->captured_type != EMPTY)
		move->score = mvv_lva[move->piece.type][move->captured_type] + SCORE_CAPTURE;

	if (move_equals(*move, killer_moves[ctx->ply][0]))
		move->score = SCORE_KILLER_FIRST;
	else if (move_equals(*move, killer_moves[ctx->ply][1]))
		move->score = SCORE_KILLER_SECOND;
	else if (history_heuristic[ctx->side][move->from][move->to] != 0)
		move->score = history_heuristic[ctx->side][move->from][move->to] + SCORE_HISTORY;
	else
		move->score = 0;
}

static int compare_move(const void* m1, const void* m2) {
	const Move* move1 = (const Move*) m1;
	const Move* move2 = (const Move*) m2;
	return move2->score - move1->score;
}

static void sort_moves(MoveList* moves, SearchContext* ctx) {
	for (size_t i = 0; i < move_list_size(moves); i++) {
		score_move(move_list_get(moves, i), ctx);
	}
	move_list_sort(moves, compare_move);
}

int quiescence(SearchContext* ctx, Board* board, int alpha, int beta, int ply) {
	ctx->side = board->side;
	ctx->ply  = ply;
	ctx->nodes++;
	if (is_repetition(board) || board->halfmove_clock > 99)
		return 0;

	int best_score = eval(board);
	if (best_score >= beta)
		return beta;
	if (best_score > alpha)
		alpha = best_score;

	// might be good to generate promotions as well, anything that changes the mat balance
	MoveList* moves = movegen_generate_captures(board, board->side);
	if (move_list_size(moves))
		move_list_sort(moves, mvv_lva_compare);
	for (size_t i = 0; i < move_list_size(moves); i++) {
		Move move = *move_list_get(moves, i);

		if (!make_move(board, move))
			continue;
		int score = -quiescence(ctx, board, -beta, -alpha, ply + 1);
		unmake_move(board);

		if (score >= beta)
			break;
		if (score > best_score)
			best_score = score;
		if (score > alpha)
			alpha = score;
	}
	move_list_destroy(&moves);
	return best_score;
}

int alpha_beta(SearchContext* ctx, Board* board, int depth, int alpha, int beta, int ply) {
	pv_length[ply]		 = 0;
	ctx->side			 = board->side;
	ctx->ply			 = ply;
	killer_moves[ply][0] = NO_MOVE;
	killer_moves[ply][1] = NO_MOVE;

	TEntry entry;
	if (ttable_probe(board->hash, &entry) && entry.depth >= depth) {
		if (entry.bound == BOUND_EXACT || (entry.bound == BOUND_LOWER && entry.score >= beta) ||
			(entry.bound == BOUND_UPPER && entry.score <= alpha)) {
			// TT hit, dont need to analyze any further
			pv_table[ctx->ply][0] = entry.best_move;
			pv_length[0]		  = 1;

			return entry.score;
		}
	}

	if (depth == 0) {
		return quiescence(ctx, board, alpha, beta, ply);
	}

	if (board->halfmove_clock > 99 || is_repetition(board))
		return 0;

	MoveList* moves = movegen_generate(board, board->side);

	int	 original_alpha = alpha;
	int	 best_score		= -INF;
	Move best_move		= NO_MOVE;
	int	 legal_moves	= 0;
	sort_moves(moves, ctx);

	for (size_t i = 0; i < move_list_size(moves); i++) {
		Move mv = *move_list_get(moves, i);
		if (!make_move(board, mv)) {
			continue;
		}
		int score = -alpha_beta(ctx, opts, board, depth - 1, -beta, -alpha, ply + 1);
		legal_moves++;
		unmake_move(board);

		if (score > best_score) {
			best_score = score;
			best_move  = mv;

			pv_table[ply][0] = mv;
			pv_length[ply]	 = 1;
			if (pv_length[ply + 1] > 0 && (ply + 1) < MAX_DEPTH) {
				// copy the child's PV
				memcpy(&pv_table[ply][1],
					   &pv_table[ply + 1][0],
					   sizeof(pv_table[ply][0]) * pv_length[ply + 1]);
				pv_length[ply] += pv_length[ply + 1];
			}
		}
		if (score > alpha) {
			alpha = score;
		}
		if (score >= beta) {
			// killer heuristic
			if (mv.captured_type == EMPTY) {
				killer_moves[ply][1] = killer_moves[ply][0];
				killer_moves[ply][0] = mv;
			}
			// history heuristic
			history_heuristic[board->side][mv.from][mv.to] += depth * depth;
			break;
		}
	}
	move_list_destroy(&moves);

	if (legal_moves == 0) {
		if (board_is_check(board, board->side)) {
			// shorter mate preferred
			best_score = -CHECKMATE + ply;
		} else {
			// stalemate
			best_score = 0;
		}
	}

	if (!move_equals(best_move, NO_MOVE)) {
		BoundType bound;
		if (best_score <= original_alpha) {
			bound = BOUND_UPPER;
		} else if (best_score >= beta) {
			bound = BOUND_LOWER;
		} else {
			bound = BOUND_EXACT;
		}
		// normalize checkmate score by removing the bias for shorter mates
		int tt_score = best_score;
		if (tt_score > CHECKMATE)
			tt_score -= CHECKMATE - depth;
		else if (tt_score < -CHECKMATE)
			tt_score += CHECKMATE - depth;
		ttable_store(board->hash, depth, tt_score, best_move, bound);
	}
	return best_score;
}

void search_init(void) {
	memset(pv_table, 0, sizeof(pv_table));
	memset(pv_length, 0, sizeof(pv_length));
	memset(killer_moves, 0, sizeof(killer_moves));
	memset(history_heuristic, 0, sizeof(history_heuristic));
	move_list_init_reserve(&root_pv, 32);
}

void search_reset(void) {
	memset(pv_table, 0, sizeof(pv_table));
	memset(pv_length, 0, sizeof(pv_length));
	memset(killer_moves, 0, sizeof(killer_moves));
	memset(history_heuristic, 0, sizeof(history_heuristic));
	move_list_clear(&root_pv);
}

Move search_best_move(Board* board, SearchOptions* options, EngineConfig* cfg) {
	SearchContext ctx = {0};
	stop			  = false;

	// iterative deepening
	for (size_t depth = 1; depth <= options->depth; depth++) {
		if (stop) {
			break;
		}
		// reset pvs for this search
		memset(&pv_length, 0, sizeof(pv_length));

		int score = alpha_beta(&ctx, board, depth, -INF, INF, 0);
		log_info("info depth %d score %d nodes %llu", depth, score, ctx.nodes);
		move_list_clear(&root_pv);
		for (size_t i = 0; i < pv_length[0]; ++i) {
			move_list_push_back(&root_pv, pv_table[0][i]);
		}

		log_info("PV:");
		for (size_t i = 0; i < move_list_size(&root_pv); i++) {
			log_info("%s", utils_move_description(*move_list_get(&root_pv, i)).str);
		}
	}
	assert(move_list_size(&root_pv) > 0);
	return *move_list_get(&root_pv, 0);
}

void search_stop(void) {
	stop = true;
}

