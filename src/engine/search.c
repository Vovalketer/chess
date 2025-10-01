#include "search.h"

#include <limits.h>
#include <stdint.h>
#include <sys/time.h>
#include <threads.h>

#include "board.h"
#include "engine_mq.h"
#include "eval.h"
#include "log.h"
#include "makemove.h"
#include "movegen.h"
#include "search_types.h"
#include "transposition.h"
#include "types.h"

#define MAX(a, b) ((a) > (b) ? (a) : (b))

#define INF		  (INT_MAX - 100000)
#define CHECKMATE (INF - 1000000)
#define MAX_DEPTH 64

typedef struct search_context {
	struct search_info	 *info;
	struct board		 *board;
	struct search_options opts;
	mtx_t				  lock;
	cnd_t				  cond;
	bool				  searching;
} SearchContext;

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

int	 search(int			   depth,
			int			   alpha,
			int			   beta,
			int			   ply,
			Board		  *board,
			SearchOptions *opts,
			SearchInfo	  *info,
			bool		   is_pv);
int	 quiescence(Board *board, int alpha, int beta, int ply, SearchInfo *info);
void iter_deepening(struct board *board, struct search_options *opts);
bool search_should_stop(void);

static bool is_repetition(Board *board);
static int	mvv_lva_compare(const void *x, const void *y);
static void score_move(Move *move, int ply, Player side, TEntry *tte);
static int	compare_move(const void *x, const void *y);
static void sort_moves(MoveList *moves, TEntry *tte, int ply, Player side);
static void gstop_cond_eval(SearchOptions *options, SearchInfo *info);

static uint32_t timeval_to_ms(struct timeval tv);
static uint32_t time_now(void);
static uint32_t search_calculate_time_budget(const SearchOptions *opts, Player p);

static int send_msg_stop(MoveList *pv);
static int send_msg_info(SearchInfo *info);

Move		  pv_table[MAX_DEPTH][MAX_DEPTH];
uint8_t		  pv_length[MAX_DEPTH];
Move		  killer_moves[MAX_DEPTH][2];
int			  history_heuristic[PLAYER_CNT][SQ_CNT][SQ_CNT];  // player, from, to
MoveList	  root_pv;
SearchContext search_ctx = {0};

SearchThreadArgs *ctl = NULL;

void iter_deepening(struct board *board, struct search_options *opts) {
	SearchInfo info	 = {0};
	info.time_start	 = time_now();
	opts->time_limit = search_calculate_time_budget(opts, board->side);	 // relative time ie 400ms
	// if depth isnt set, iterate until MAX_DEPTH-1 at most to avoid overflows
	size_t max_depth = opts->depth != 0 ? opts->depth : MAX_DEPTH - 1;

	for (size_t depth = 1; depth <= max_depth; depth++) {
		memset(&pv_length, 0, sizeof(pv_length));
		int score			= search(depth, -INF, INF, 0, board, opts, &info, true);
		info.score_cp		= score;
		uint32_t elapsed_ms = time_now() - info.time_start;
		if (elapsed_ms == 0)
			elapsed_ms = 1;
		info.nps = info.nodes * 1000 / elapsed_ms;

		if (pv_length[0] >= move_list_size(&root_pv)) {
			move_list_clear(&root_pv);
			for (size_t i = 0; i < pv_length[0]; ++i) {
				move_list_push_back(&root_pv, pv_table[0][i]);
			}
		} else {
			for (size_t i = 0; i < pv_length[0]; ++i) {
				Move *m = move_list_at(&root_pv, i);
				*m		= pv_table[0][i];
			}
		}

		send_msg_info(&info);
		gstop_cond_eval(opts, &info);
		if (search_should_stop())
			break;
	}
	search_stop();
}

int quiescence(Board *board, int alpha, int beta, int ply, SearchInfo *info) {
	info->seldepth = ply;
	info->nodes++;

	if (is_repetition(board) || board->halfmove_clock > 99)
		return 0;

	int best_score = eval(board);
	if (best_score >= beta)
		return beta;
	if (best_score > alpha)
		alpha = best_score;

	// might be good to generate promotions as well, anything that changes the mat balance
	MoveList *moves = movegen_generate_captures(board, board->side);
	if (move_list_size(moves))
		move_list_sort(moves, mvv_lva_compare);
	for (size_t i = 0; i < move_list_size(moves); i++) {
		Move move = *move_list_at(moves, i);

		if (!make_move(board, move))
			continue;
		int score = -quiescence(board, -beta, -alpha, ply + 1, info);
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
		   Board		 *board,
		   SearchOptions *opts,
		   SearchInfo	 *info,
		   bool			  is_pv) {
	pv_length[ply] = 0;
	info->depth	   = ply;
	info->seldepth = ply;

	if (depth == 0) {
		return quiescence(board, alpha, beta, ply, info);
	}

	info->nodes++;
	gstop_cond_eval(opts, info);

	if (board->halfmove_clock > 99 || is_repetition(board))
		return 0;
	if (search_should_stop())
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
				pv_table[ply][0] = entry.best_move;
				pv_length[ply]	 = 1;
				return entry.score;
			}
		}
	}

	MoveList *moves = movegen_generate(board, board->side);

	Move	  best_move	  = NO_MOVE;
	int		  best_score  = -INF;
	BoundType tt_bound	  = BOUND_UPPER;  // default to score<=alpha
	int		  legal_moves = 0;
	sort_moves(moves, &entry, ply, board->side);

	for (size_t i = 0; i < move_list_size(moves); i++) {
		if (search_should_stop()) {
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
			score = -search(depth - 1, -beta, -alpha, ply + 1, board, opts, info, is_pv);
		} else {
			// reduced width search
			score = -search(depth - 1, -alpha - 1, -alpha, ply + 1, board, opts, info, false);
			if (score > alpha && score < beta) {
				// full width research
				score = -search(depth - 1, -beta, -alpha, ply + 1, board, opts, info, is_pv);
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

int search_thread(void *arg) {
	assert(arg != NULL);
	log_trace("search thread started");
	SearchThreadArgs *sta = arg;
	assert(sta->config != NULL);
	ctl = sta;

	ctl->shutdown = false;

	memset(pv_table, 0, sizeof(pv_table));
	memset(pv_length, 0, sizeof(pv_length));
	memset(killer_moves, 0, sizeof(killer_moves));
	memset(history_heuristic, 0, sizeof(history_heuristic));
	move_list_init_reserve(&root_pv, 32);

	mtx_init(&search_ctx.lock, mtx_plain);
	cnd_init(&search_ctx.cond);

	while (!ctl->shutdown) {
		mtx_lock(&search_ctx.lock);
		while (!search_ctx.searching && !ctl->shutdown) {
			cnd_wait(&search_ctx.cond, &search_ctx.lock);
		}
		if (ctl->shutdown)
			break;
		mtx_unlock(&search_ctx.lock);
		log_trace("searching");
		iter_deepening(search_ctx.board, &search_ctx.opts);
		// search finished, notify the main thread
		send_msg_stop(&root_pv);
		log_trace("search done");
	}
	log_trace("search thread stopped");
	return 0;
}

void search_start(struct board *board, struct search_options options) {
	assert(board != NULL);
	log_trace("starting search");
	mtx_lock(&search_ctx.lock);
	search_ctx.board	 = board;
	search_ctx.opts		 = options;
	search_ctx.searching = true;
	cnd_signal(&search_ctx.cond);
	mtx_unlock(&search_ctx.lock);
	log_trace("search start signal sent");
}

void search_shutdown(void) {
	log_trace("shutting down search");
	mtx_lock(&search_ctx.lock);
	search_ctx.searching = false;
	ctl->shutdown		 = true;
	cnd_broadcast(&search_ctx.cond);
	mtx_unlock(&search_ctx.lock);
	log_trace("search shutdown signal sent");
}

void search_reset(void) {
	memset(pv_table, 0, sizeof(pv_table));
	memset(pv_length, 0, sizeof(pv_length));
	memset(killer_moves, 0, sizeof(killer_moves));
	memset(history_heuristic, 0, sizeof(history_heuristic));
	move_list_clear(&root_pv);
}

void search_stop(void) {
	log_trace("stopping search");
	search_ctx.searching = false;
	log_trace("search stop signal sent");
}

bool search_should_stop(void) {
	return !search_ctx.searching || ctl->shutdown;
}

/*
 * Time functions
 */

uint32_t search_calculate_time_budget(const SearchOptions *opts, Player p) {
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

void gstop_cond_eval(SearchOptions *options, SearchInfo *info) {
	// NOTE: depth is evaluated by the function performing iterative deepening
	if (search_should_stop()) {
		log_debug("search already stopped");
		return;
	}

	if (options->nodes && info->nodes >= options->nodes) {
		log_trace("node limit reached: set %u nodes %u", options->nodes, info->nodes);
		search_stop();
		return;
	}

	if (!options->infinite) {
		uint32_t timenow = time_now();
		if ((timenow - info->time_start) >= options->time_limit) {
			log_trace("time limit reached: elapsed %u ms", timenow - options->time_limit);
			search_stop();
			return;
		}
	}
}

/*
 * Score functions
 */

static int compare_move(const void *m1, const void *m2) {
	const Move *move1 = (const Move *) m1;
	const Move *move2 = (const Move *) m2;
	return move2->score - move1->score;
}

static void sort_moves(MoveList *moves, TEntry *tte, int ply, Player side) {
	for (size_t i = 0; i < move_list_size(moves); i++) {
		score_move(move_list_at(moves, i), ply, side, tte);
	}
	move_list_sort(moves, compare_move);
}

static int mvv_lva_compare(const void *x, const void *y) {
	Move *xt	= (Move *) x;
	Move *yt	= (Move *) y;
	int	  x_val = mvv_lva[xt->piece.type][xt->captured_type];
	int	  y_val = mvv_lva[yt->piece.type][yt->captured_type];
	if (x_val < y_val)
		return 1;
	else if (x_val > y_val)
		return -1;
	return 0;
}

static void score_move(Move *move, int ply, Player side, TEntry *tte) {
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

static bool is_repetition(Board *board) {
	assert(board != NULL);
	assert(board->history != NULL);
	int count = 0;
	for (size_t i = 0; i < history_size(board->history); i++) {
		History *h = history_at(board->history, i);
		assert(h != NULL);
		if (h->hash == board->hash) {
			count++;
		}
	}
	return count >= 3;
}

/*
 * Messages
 */

void free_msg(SearchMsg *msg) {
	assert(msg != NULL);
	if (msg->type == SEARCH_MSG_INFO)
		move_list_free(&msg->payload.search_info.pv);
}

static int send_msg_info(SearchInfo *info) {
	assert(info != NULL);
	SearchMsg msg			= {0};
	msg.type				= SEARCH_MSG_INFO;
	msg.free_payload		= free_msg;
	msg.payload.search_info = *info;
	size_t pv_size			= move_list_size(&root_pv);
	if (pv_size > 0) {
		log_debug("info payload init");
		move_list_init(&msg.payload.search_info.pv);
		log_debug("info payload cloning pv");
		move_list_clone(&msg.payload.search_info.pv, &root_pv);
	} else {
		move_list_clear(&msg.payload.search_info.pv);
	}
	log_debug("info payload done");
	return engmq_send_search_msg(&msg);
}

static int send_msg_stop(MoveList *pv) {
	assert(pv != NULL);
	SearchMsg msg	 = {0};
	msg.type		 = SEARCH_MSG_STOP;
	msg.free_payload = free_msg;
	assert(move_list_size(&root_pv) > 0);
	msg.payload.bestmove = *move_list_at(pv, 0);
	return engmq_send_search_msg(&msg);
}
