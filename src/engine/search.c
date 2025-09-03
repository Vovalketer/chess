#include "search.h"

#include <limits.h>

#include "board.h"
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

VECTOR_DEFINE_TYPE(ScoredMove, ScoredMoveList, sm)

typedef struct {
	Move		   pv_table[MAX_DEPTH][MAX_DEPTH];	// stack allocated matrix for fast access
	uint8_t		   pv_length[MAX_DEPTH];
	Move		   killer_moves[MAX_DEPTH][2];
	int			   history_heuristic[2][SQ_CNT][SQ_CNT];  // player, from, to
	ScoredMoveList scored_moves;						  // meant to be used as cache for sorting
	int			   ply;
	Player		   side;
	uint64_t	   nodes;
} SearchContext;

static void search_ctx_init(SearchContext* ctx) {
	memset(ctx, 0, sizeof(*ctx));
	sm_init_reserve(&ctx->scored_moves, 128);
}

static void search_ctx_free(SearchContext* ctx) {
	sm_free(&ctx->scored_moves);
}

static int mvv_lva_get(PieceType victim, PieceType attacker) {
	return mvv_lva[attacker][victim];
}

static int mvv_lva_compare(const void* x, const void* y) {
	Move* xt	= (Move*) x;
	Move* yt	= (Move*) y;
	int	  x_val = mvv_lva_get(xt->captured_type, xt->piece.type);
	int	  y_val = mvv_lva_get(yt->captured_type, yt->piece.type);
	if (x_val < y_val)
		return 1;
	else if (x_val > y_val)
		return -1;
	return 0;
}

static int scored_move_compare(const void* x, const void* y) {
	ScoredMove* xt = (ScoredMove*) x;
	ScoredMove* yt = (ScoredMove*) y;
	if (xt->score < yt->score)
		return 1;
	else if (xt->score > yt->score)
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

static int score_move(Move move, SearchContext* ctx) {
	if (move.captured_type != EMPTY)
		return mvv_lva_get(move.captured_type, move.piece.type) + SCORE_CAPTURE;

	if (move_equals(move, ctx->killer_moves[ctx->ply][0]))
		return SCORE_KILLER_FIRST;
	else if (move_equals(move, ctx->killer_moves[ctx->ply][1]))
		return SCORE_KILLER_SECOND;
	else
		return ctx->history_heuristic[ctx->side][move.from][move.to] + SCORE_HISTORY;
}

static ScoredMoveList get_scored_moves(MoveList* moves, SearchContext* ctx) {
	ScoredMoveList scored_moves;
	sm_init(&scored_moves);
	for (size_t i = 0; i < move_list_size(moves); i++) {
		Move move = *move_list_get(moves, i);
		assert(move.from != move.to);
		ScoredMove scored_move = {move, score_move(move, ctx)};
		sm_push_back(&scored_moves, scored_move);
	}
	sm_sort(&scored_moves, scored_move_compare);
	return scored_moves;
}

void copy_pv(SearchContext* ctx, Move best_move, int ply) {
	ctx->pv_table[ply][0] = best_move;

	if (ctx->pv_length[ply + 1] > 0) {
		memcpy(&ctx->pv_table[ply][1],
			   &ctx->pv_table[ply + 1][0],
			   sizeof(Move) * (ctx->pv_length[ply + 1]));
	}
	ctx->pv_length[ply] = 1 + ctx->pv_length[ply + 1];
}

int quiescence(SearchContext* ctx, Board* board, int alpha, int beta, int ply) {
	ctx->side = board->side;
	ctx->ply  = ply;
	ctx->nodes++;
	if (is_repetition(board) || board->halfmove_clock > 99)
		return 0;
	int val = eval(board);
	if (val >= beta)
		return val;
	if (val > alpha)
		alpha = val;

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
			return score;
		if (score > val)
			val = score;
		if (score > alpha)
			alpha = score;
	}
	move_list_destroy(&moves);
	return val;
}

void copy_tt_entry(SearchContext* ctx, TEntry* entry) {
	ctx->pv_table[ctx->ply][0] = entry->best_move;
	ctx->pv_length[0]		   = 1;
}

int alpha_beta(SearchContext* ctx, Board* board, int depth, int alpha, int beta, int ply) {
	ctx->pv_length[ply]		  = 0;
	ctx->side				  = board->side;
	ctx->ply				  = ply;
	ctx->killer_moves[ply][0] = NO_MOVE;
	ctx->killer_moves[ply][1] = NO_MOVE;
	sm_clear(&ctx->scored_moves);

	TEntry entry;
	if (ttable_probe(board->hash, &entry)) {
		if (entry.key == board->hash) {
			if (entry.depth >= depth) {
				switch (entry.bound) {
					case BOUND_LOWER:
						if (entry.score >= beta) {
							copy_tt_entry(ctx, &entry);
							return entry.score;
						}
						break;
					case BOUND_EXACT:
						copy_tt_entry(ctx, &entry);
						return entry.score;
						break;
					case BOUND_UPPER:
						if (entry.score <= alpha) {
							copy_tt_entry(ctx, &entry);
							return entry.score;
						}
						break;
				}
			}
		}
	}

	if (depth == 0) {
		return quiescence(ctx, board, alpha, beta, ply);
	}

	if (board->halfmove_clock > 99 || is_repetition(board))
		return 0;

	MoveList* moves = movegen_generate(board, board->side);

	int	 original_alpha			= alpha;
	int	 best_score				= -INF;
	Move best_move				= NO_MOVE;
	int	 legal_moves			= 0;
	ctx->scored_moves			= get_scored_moves(moves, ctx);
	ScoredMoveList scored_moves = ctx->scored_moves;

	for (size_t i = 0; i < sm_size(&scored_moves); i++) {
		ScoredMove sm = *sm_get(&scored_moves, i);
		assert(sm.move.from != sm.move.to);
		if (!make_move(board, sm.move))
			continue;

		int score = -alpha_beta(ctx, board, depth - 1, -beta, -alpha, ply + 1);
		legal_moves++;
		ctx->nodes++;
		unmake_move(board);

		if (score > best_score) {
			best_score = score;
			best_move  = sm.move;

			ctx->pv_table[ply][0] = sm.move;
			if (ctx->pv_length[ply + 1] > 0) {
				// copy the child's PV
				memcpy(&ctx->pv_table[ply][1],
					   &ctx->pv_table[ply + 1][0],
					   sizeof(ctx->pv_table[ply][0]) * ctx->pv_length[ply + 1]);
			}
			ctx->pv_length[ply] = 1 + ctx->pv_length[ply + 1];
		}
		if (score > alpha) {
			alpha = score;
		}
		if (score >= beta) {
			// killer heuristic
			if (sm.move.captured_type == EMPTY) {
				ctx->killer_moves[ply][1] = ctx->killer_moves[ply][0];
				ctx->killer_moves[ply][0] = sm.move;
			}
			// history heuristic
			ctx->history_heuristic[board->side][sm.move.from][sm.move.to] += depth * depth;
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

		int score = alpha_beta(&ctx, board, depth, -INF, INF, 0);
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
			log_info("%s", utils_move_description(*move_list_get(&root_pv, i)).str);
		}
	}
	search_ctx_free(&ctx);
	return best_move;
}
