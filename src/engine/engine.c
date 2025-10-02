#define _ISOC11_SOURCE
#include <threads.h>

#include "bitboards.h"
#include "board.h"
#include "engine_mq.h"
#include "engine_types.h"
#include "hash.h"
#include "log.h"
#include "makemove.h"
#include "movegen.h"
#include "search.h"
#include "transposition.h"
#include "uci.h"
#include "utils.h"

#define ENGINE_NAME	  "test_engine"
#define ENGINE_AUTHOR "test_author"

typedef struct engine_config {
	unsigned int threads;
} EngineConfig;

typedef struct engine_state {
	struct engine_config *config;
	struct board		 *board;
} EngineState;

static void engine_print_info(SearchInfo *info);
static void engine_print_best_move(Move move);
static void engine_isready(void);
static void engine_uci(EngineConfig *opts);

static Move ucimv_to_move(UciMove *ucimv);

Board		 board;
SearchInfo	 info;
EngineConfig cfg = {0};
Move		 best_move;
bool		 quit  = false;
EngineState	 state = {0};

void init(void) {
	bitboards_init();
	ttable_init(256);
	hash_init();
	board_init(&board);
	engmq_init();
	state.config = &cfg;
	state.board	 = &board;
}

int main(void) {
	init();
	thrd_t			 uci_thrd, search_thrd;
	SearchThreadArgs sta = {.config = state.config};
	log_set_level(LOG_TRACE);

	thrd_create(&uci_thrd, uci_thread, NULL);
	thrd_create(&search_thrd, search_thread, &sta);

	while (!quit) {
		struct engine_msg msg;
		log_trace("engine waiting for msg");
		if (engmq_receive(&msg) < 0) {
			log_error("failed to receive msg");
		}
		log_trace("engine received msg, type: %d", msg.type);
		switch (msg.type) {
			case MSG_UCI: {
				UciMsg uci = msg.payload.uci;
				switch (msg.payload.uci.type) {
					case MSG_UCI_ISREADY:
						engine_isready();
						break;
					case MSG_UCI_UCINEWGAME:
						break;
					case MSG_UCI_POSITION: {
						UciPosition *pos = uci.payload.position;
						board_from_fen(&board, pos->fen.str);
						for (size_t i = 0; i < ucimv_list_size(&pos->moves); i++) {
							UciMove *uci_move = ucimv_list_at(&pos->moves, i);

							Move move = ucimv_to_move(uci_move);
							log_info("move: %s", utils_move_description(move).str);
							if (move_equals(move, NO_MOVE) || !make_move(&board, move)) {
								// if one move is invalid then we'll assume
								// the rest are invalid as well
								break;
							}
						}
					} break;
					case MSG_UCI_GO: {
						UciGo *go = uci.payload.go;
						// TODO: implement searchmoves
						//  if(ucimv_list_size(go->searchmoves) > 0) {
						//  }
						SearchOptions opts = {.depth	 = go->depth,
											  .nodes	 = go->nodes,
											  .btime	 = go->btime,
											  .wtime	 = go->wtime,
											  .binc		 = go->binc,
											  .winc		 = go->winc,
											  .movetime	 = go->movetime,
											  .movestogo = go->movestogo,
											  .mate		 = go->mate,
											  .infinite	 = go->infinite,
											  .ponder	 = go->ponder};
						// parse uci move into move struct
						search_start(state.board, opts);
					} break;
					case MSG_UCI_STOP:
						search_stop();
						break;
					case MSG_UCI_SETOPTION: {
						// TODO: impement options
						UciSetOption *opt = uci.payload.set_option;
					} break;
					case MSG_UCI_DEBUG:
						break;
					case MSG_UCI_UCI:
						engine_uci(state.config);
						break;
					case MSG_UCI_QUIT:
						engmq_destroy();
						uci_shutdown();
						search_shutdown();
						quit = true;
						break;
					case MSG_UCI_PRINT:
						break;
					case MSG_UCI_NONE:
						break;
				}
				uci.free_payload(&uci);
			} break;
			case MSG_SEARCH: {
				SearchMsg sm = msg.payload.search;
				switch (sm.type) {
					case SEARCH_MSG_INFO: {
						SearchInfo search_info = sm.payload.search_info;
						engine_print_info(&search_info);
					} break;
					case SEARCH_MSG_STOP:
						engine_print_best_move(sm.payload.bestmove);
						break;
					case SEARCH_MSG_NONE:
						break;
				}
				sm.free_payload(&sm);
			} break;
		}
	}

	log_trace("Shutting down");
	log_trace("destroying uci thread");
	thrd_join(uci_thrd, NULL);
	log_trace("destroying search thread");
	thrd_join(search_thrd, NULL);
	log_trace("Shut down");

	return 0;
}

static void engine_print_info(SearchInfo *info) {
	assert(info != NULL);
	assert(info->pv.data != NULL);
	printf("info depth %d seldepth %d score cp %d nodes %lu nps %u ",
		   info->depth,
		   info->seldepth,
		   info->score_cp,
		   info->nodes,
		   info->nps);

	size_t pv_size = move_list_size(&info->pv);
	printf("pv");
	for (size_t i = 0; i < pv_size; i++) {
		Move *m = move_list_at(&info->pv, i);
		printf(" %s%s", utils_square_to_str(m->from), utils_square_to_str(m->to));
	}
	printf("\n");
	fflush(stdout);
}

static void engine_print_best_move(Move move) {
	printf("bestmove %s%s\n", utils_square_to_str(move.from), utils_square_to_str(move.to));
	fflush(stdout);
}

void engine_uci(EngineConfig *opts) {
	printf("id name %s\n", ENGINE_NAME);
	printf("id author %s\n", ENGINE_AUTHOR);
	printf("\n");
	printf("option name Threads value %d\n", opts->threads);
	printf("uciok\n");
	fflush(stdout);
}

void engine_isready(void) {
	printf("readyok\n");
	fflush(stdout);
}

static Move ucimv_to_move(UciMove *ucimv) {
	// find the complete definition of a move to be used by make_move
	MoveList *moves = movegen_generate(&board, board.side);
	// TODO: generating all the moves is inefficient
	// we could generate for a given square instead
	Move *move = NULL;
	for (size_t j = 0; j < move_list_size(moves); j++) {
		move = move_list_at(moves, j);
		if (move->from == ucimv->from && move->to == ucimv->to) {
			break;
		}
		move = NULL;
	}
	Move ret = move ? *move : NO_MOVE;
	move_list_destroy(&moves);
	return ret;
}
