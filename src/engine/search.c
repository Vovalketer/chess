#include "search.h"

#include <limits.h>
#include <sys/time.h>

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

#define MAX(a, b) ((a) > (b) ? (a) : (b))

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
	SCORE_NONE			= 0,
	SCORE_HISTORY		= 5000,
	SCORE_KILLER_SECOND = 8000,
	SCORE_KILLER_FIRST	= 9000,
	SCORE_CAPTURE		= 10000,
	SCORE_TT			= 20000,
} ScoreType;

typedef struct {
	int		 ply;
	Player	 side;
	uint64_t nodes;
	uint32_t time_start;
	uint32_t time_limit;
} SearchContext;

int search(int			  depth,
		   int			  alpha,
		   int			  beta,
		   int			  ply,
		   Board*		  board,
		   SearchOptions* opts,
		   SearchContext* ctx,
		   bool			  is_pv);
int quiescence(SearchContext* ctx, Board* board, int alpha, int beta, int ply);

static bool is_repetition(Board* board);
static int	mvv_lva_compare(const void* x, const void* y);
static void score_move(Move* move, int ply, Player side, TEntry* tte);
static int	compare_move(const void* x, const void* y);
static void sort_moves(MoveList* moves, SearchContext* ctx, TEntry* tte);
static void gstop_cond_eval(SearchOptions* options,
							uint32_t	   nodes,
							uint32_t	   time_start,
							uint32_t	   time_limit);

static uint32_t timeval_to_ms(struct timeval tv);
static uint32_t time_now(void);
static uint32_t search_calculate_time_budget(const SearchOptions* opts, Player p);

Move		  pv_table[MAX_DEPTH][MAX_DEPTH];
uint8_t		  pv_length[MAX_DEPTH];
Move		  killer_moves[MAX_DEPTH][2];
int			  history_heuristic[PLAYER_CNT][SQ_CNT][SQ_CNT];  // player, from, to
MoveList	  root_pv;
volatile bool stop = false;

Move search_best_move(Board* board, SearchOptions* opts, EngineConfig* cfg) {
	assert(board != NULL);
	assert(opts != NULL);
	assert(cfg != NULL);
	SearchContext ctx = {0};
	stop			  = false;
	ctx.time_start	  = time_now();
	ctx.time_limit	  = search_calculate_time_budget(opts, board->side);
	log_debug(
		"search options - depth: %d, nodes: %d, movetime: %u, wtime: %u, btime: %u, winc: %u, "
		"binc: %u, movestogo: %d, infinite: %d",
		opts->depth,
		opts->nodes,
		opts->movetime,
		opts->wtime,
		opts->btime,
		opts->winc,
		opts->binc,
		opts->movestogo,
		opts->infinite);

	// if depth isnt set, iterate until MAX_DEPTH-1 at most to avoid overflows
	size_t max_depth = opts->depth != 0 ? opts->depth : MAX_DEPTH - 1;
	// iterative deepening
	for (size_t depth = 1; depth <= max_depth; depth++) {
		memset(&pv_length, 0, sizeof(pv_length));
		int score = search(depth, -INF, INF, 0, board, opts, &ctx, true);

		gstop_cond_eval(opts, ctx.nodes, ctx.time_start, ctx.time_limit);
		if (stop)
			break;

		log_info("info depth %d score %d nodes %llu", depth, score, ctx.nodes);
		if (pv_length[0] >= move_list_size(&root_pv)) {
			move_list_clear(&root_pv);
			for (size_t i = 0; i < pv_length[0]; ++i) {
				move_list_push_back(&root_pv, pv_table[0][i]);
			}
		} else {
			for (size_t i = 0; i < pv_length[0]; ++i) {
				Move* m = move_list_at(&root_pv, i);
				*m		= pv_table[0][i];
			}
		}

		log_info("PV:");
		for (size_t i = 0; i < move_list_size(&root_pv); i++) {
			log_info("%s", utils_move_description(*move_list_at(&root_pv, i)).str);
		}
	}
	assert(move_list_size(&root_pv) > 0);
	return *move_list_at(&root_pv, 0);
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
		Move move = *move_list_at(moves, i);

		if (!make_move(board, move))
			continue;
		int score = -quiescence(ctx, board, -beta, -alpha, ply + 1);
		unmake_move(board);

		best_score = MAX(best_score, score);

		if (score >= beta) {
			move_list_destroy(&moves);
			return score;
		}
		if (score > alpha)
			alpha = score;
	}
	move_list_destroy(&moves);
	return best_score;
}

// PVS
int search(int			  depth,
		   int			  alpha,
		   int			  beta,
		   int			  ply,
		   Board*		  board,
		   SearchOptions* opts,
		   SearchContext* ctx,
		   bool			  is_pv) {
	pv_length[ply] = 0;
	ctx->side	   = board->side;
	ctx->ply	   = ply;

	if (depth == 0) {
		return quiescence(ctx, board, alpha, beta, ply);
	}

	ctx->nodes++;
	gstop_cond_eval(opts, ctx->nodes, ctx->time_start, ctx->time_limit);

	if (board->halfmove_clock > 99 || is_repetition(board))
		return 0;
	if (stop)
		return 0;

	TEntry entry = {0};
	if (ttable_probe(board->hash, &entry) && entry.depth >= depth) {
		if (!is_pv) {
			if (entry.bound == BOUND_EXACT) {
				return entry.score;
			} else if (entry.bound == BOUND_LOWER && entry.score >= beta) {
				return beta;
			} else if (entry.bound == BOUND_UPPER && entry.score <= alpha) {
				return alpha;
			}
		} else {
			if (entry.bound == BOUND_EXACT) {
				pv_table[ctx->ply][0] = entry.best_move;
				pv_length[ctx->ply]	  = 1;
				return entry.score;
			}
		}
	}

	MoveList* moves = movegen_generate(board, board->side);

	Move	  best_move	  = NO_MOVE;
	int		  best_score  = -INF;
	BoundType tt_bound	  = BOUND_UPPER;  // default to score<=alpha
	int		  legal_moves = 0;
	sort_moves(moves, ctx, &entry);

	for (size_t i = 0; i < move_list_size(moves); i++) {
		if (stop) {
			move_list_destroy(&moves);
			return 0;
		}
		Move mv = *move_list_at(moves, i);
		if (!make_move(board, mv)) {
			continue;
		}
		legal_moves++;

		int score;
		if (i == 0) {
			// full width search on the first move
			score = -search(depth - 1, -beta, -alpha, ply + 1, board, opts, ctx, is_pv);
		} else {
			// reduced width search
			score = -search(depth - 1, -alpha - 1, -alpha, ply + 1, board, opts, ctx, false);
			if (score > alpha && score < beta) {
				// full width research
				score = -search(depth - 1, -beta, -alpha, ply + 1, board, opts, ctx, is_pv);
			}
		}

		unmake_move(board);
		best_score = MAX(best_score, score);

		if (score >= beta) {
			// killer heuristic
			if (mv.captured_type == EMPTY) {
				killer_moves[ply][1] = killer_moves[ply][0];
				killer_moves[ply][0] = mv;
			}
			// history heuristic
			history_heuristic[board->side][mv.from][mv.to] += depth * depth;

			// alpha	 = beta;
			tt_bound = BOUND_LOWER;
			break;
		}

		if (score > alpha) {
			alpha	  = score;
			best_move = mv;
			tt_bound  = BOUND_EXACT;

			pv_table[ply][0] = mv;
			pv_length[ply]	 = 1;
			if ((ply + 1) < MAX_DEPTH && pv_length[ply + 1] > 0) {
				// copy the child's PV
				memcpy(&pv_table[ply][1],
					   &pv_table[ply + 1][0],
					   sizeof(pv_table[ply][0]) * pv_length[ply + 1]);
				pv_length[ply] += pv_length[ply + 1];
			}
		}
	}
	move_list_destroy(&moves);

	int tt_score = best_score;
	if (legal_moves == 0) {
		if (board_is_check(board, board->side)) {
			// shorter mate preferred
			best_score = -CHECKMATE + ply;
			tt_score   = -CHECKMATE;  // store normalized score
		} else {
			// stalemate
			best_score = 0;
		}
	}

	if (!move_equals(best_move, NO_MOVE)) {
		ttable_store(board->hash, depth, tt_score, best_move, tt_bound);
	}

	assert(best_score != -INF && best_score != INF);
	return best_score;
}

/*
 * Lifetime
 */

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

void search_stop(void) {
	stop = true;
}

/*
 * Time functions
 */

uint32_t search_calculate_time_budget(const SearchOptions* opts, Player p) {
	log_trace("calculating time budget");
	uint32_t remaining = p == PLAYER_W ? opts->wtime : opts->btime;
	log_trace("remaining time: %u ms", remaining);
	uint32_t increment = p == PLAYER_W ? opts->winc : opts->binc;
	log_trace("increment: %u ms", increment);
	uint32_t movestogo = opts->movestogo ? opts->movestogo : 40;  // use 40 as fallback

	// no time control
	if (!remaining && !increment) {
		log_trace("no time control set");
		return UINT32_MAX;
	}

	// add a fraction of the increment to the budget
	uint32_t time = ((uint32_t) (remaining / movestogo)) + (increment * 0.75);

	// ensure small reserve
	if (time > 50)
		time -= 50;

	// ensure the search doesnt stop too early
	if (time < 100)
		time = 100;

	log_trace("time budget: %u ms", time);
	return time;
}

uint32_t timeval_to_ms(struct timeval tv) {
	return tv.tv_sec * 1000 + tv.tv_usec / 1000;
}

uint32_t time_now(void) {
	struct timeval tv;
	gettimeofday(&tv, NULL);
	return timeval_to_ms(tv);
}

void gstop_cond_eval(SearchOptions* options,
					 uint32_t		nodes,
					 uint32_t		time_start,
					 uint32_t		time_limit) {
	// NOTE: depth is evaluated by the function performing iterative deepening
	if (stop) {
		log_debug("search already stopped");
		return;
	}

	if (options->nodes && nodes >= options->nodes) {
		log_trace("node limit reached: set %u nodes %u", options->nodes, nodes);
		stop = true;
		return;
	}

	if (!options->infinite) {
		uint32_t timenow = time_now();
		if (timenow - time_start >= time_limit) {
			log_trace("time limit reached: elapsed %u ms, time limit: %u ms",
					  timenow - time_start,
					  time_limit);
			stop = true;
			return;
		}
	}
}

/*
 * Score functions
 */

static int compare_move(const void* m1, const void* m2) {
	const Move* move1 = (const Move*) m1;
	const Move* move2 = (const Move*) m2;
	return move2->score - move1->score;
}

static void sort_moves(MoveList* moves, SearchContext* ctx, TEntry* tte) {
	for (size_t i = 0; i < move_list_size(moves); i++) {
		score_move(move_list_at(moves, i), ctx->ply, ctx->side, tte);
	}
	move_list_sort(moves, compare_move);
}

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

static void score_move(Move* move, int ply, Player side, TEntry* tte) {
	// check if the TEntry is not empty before performing the comparison
	if (tte->key && move_equals(*move, tte->best_move))
		move->score = SCORE_TT;
	else if (move->captured_type != EMPTY)
		move->score = mvv_lva[move->piece.type][move->captured_type] + SCORE_CAPTURE;
	else if (move_equals(*move, killer_moves[ply][0]))
		move->score = SCORE_KILLER_FIRST;
	else if (move_equals(*move, killer_moves[ply][1]))
		move->score = SCORE_KILLER_SECOND;
	else if (history_heuristic[side][move->from][move->to] != 0)
		move->score = history_heuristic[side][move->from][move->to] + SCORE_HISTORY;
	else
		move->score = SCORE_NONE;
}

static bool is_repetition(Board* board) {
	int count = 0;
	for (size_t i = 0; i < history_size(board->history); i++) {
		History* h = history_at(board->history, i);
		assert(h != NULL);
		if (h->hash == board->hash) {
			count++;
		}
	}
	return count >= 3;
}
