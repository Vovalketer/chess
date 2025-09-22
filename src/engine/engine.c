#include <threads.h>

#include "bitboards.h"
#include "board.h"
#include "engine_types.h"
#include "hash.h"
#include "log.h"
#include "makemove.h"
#include "movegen.h"
#include "msg_queue.h"
#include "search.h"
#include "transposition.h"
#include "types.h"
#include "uci.h"
#include "uci_types.h"
#include "utils.h"

#define ENGINE_NAME	  "test_engine"
#define ENGINE_AUTHOR "test_author"

static void engine_print_info(SearchInfo *info);
static void engine_print_best_move(Move move);
static void engine_isready(void);
void		engine_uci(EngineConfig *opts);

static Move *ucimv_to_move(UciMove *ucimv);

Board		 board;
SearchInfo	 info;
EngineConfig cfg;
Move		 best_move;
bool		 quit  = false;
EngineState	 state = {0};

void init(void) {
	state.config	 = malloc(sizeof(EngineConfig));
	EngineConfig cfg = {0};
	*state.config	 = cfg;
	bitboards_init();
	ttable_init(256);
	hash_init();
	board_init(&board);
	state.board		= &board;
	state.msg_queue = msg_queue_create(1 << 5);
	assert(state.msg_queue != NULL);
	msg_queue_open(state.msg_queue);
}

int main(void) {
	init();
	thrd_t			 uci_thrd, search_thrd;
	SearchThreadArgs sta = {.config = state.config, .msg_queue = state.msg_queue};
	log_set_level(LOG_TRACE);

	thrd_create(&uci_thrd, uci_thread, &sta);
	thrd_create(&search_thrd, search_thread, &state);

	MsgQueue *mq = state.msg_queue;	 // convenience
	while (!quit) {
		Message *msg = NULL;
		log_trace("engine waiting for msg");
		msg_queue_pop_wait(mq, &msg);
		log_trace("engine received msg, type: %d", msg->type);
		switch (msg->type) {
			case MSG_UCI: {
				switch (msg->subtype) {
					case MSG_UCI_NONE:
						break;
					case MSG_UCI_ISREADY:
						engine_isready();
						break;
					case MSG_UCI_UCINEWGAME:
						break;
					case MSG_UCI_POSITION: {
						UciPosition *pos = msg->payload;
						board_from_fen(&board, pos->fen.str);
						for (size_t i = 0; i < ucimv_list_size(pos->moves); i++) {
							UciMove *uci_move = ucimv_list_at(pos->moves, i);

							Move *move = ucimv_to_move(uci_move);

							if (!move || !make_move(&board, *move)) {
								// if one move is invalid then we'll assume
								// the rest as invalid as well
								break;
							}
						}
					} break;
					case MSG_UCI_GO: {
						UciGo *go = msg->payload;
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
						UciSetOption *opt = msg->payload;
					} break;
					case MSG_UCI_DEBUG:
						break;
					case MSG_UCI_UCI:
						engine_uci(state.config);
						break;
					case MSG_UCI_QUIT:
						msg_queue_close(mq);
						uci_shutdown();
						search_shutdown();
						quit = true;
						break;
				}
			} break;
			case MSG_SEARCH: {
				switch (msg->subtype) {
					case MSG_SEARCH_SEARCH_INFO: {
						SearchInfo *search_info = msg->payload;
						if (move_list_size(search_info->pv) > 0)
							best_move = *move_list_at(search_info->pv, 0);
						else
							best_move = NO_MOVE;
						engine_print_info(search_info);
					} break;
					case MSG_SEARCH_STOP:
						engine_print_best_move(best_move);
						break;
				}
			} break;
		}
		msg->free_payload(msg);
		free(msg);
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
	printf("info depth %d seldepth %d score cp %d nodes %lu time %d\n",
		   info->depth,
		   info->seldepth,
		   info->score_cp,
		   info->nodes,
		   info->time);

	printf("pv");
	for (size_t i = 0; i < move_list_size(info->pv); i++) {
		Move *m = move_list_at(info->pv, i);
		printf(" %s%s", utils_square_to_str(m->from), utils_square_to_str(m->to));
	}
	printf("\n");
	fflush(stdout);
}

static void engine_print_best_move(Move move) {
	printf("bestmove %s%s\n", utils_square_to_str(move.from), utils_square_to_str(move.to));
}

void engine_uci(EngineConfig *opts) {
	printf("id name %s\n", ENGINE_NAME);
	printf("id author %s\n", ENGINE_AUTHOR);
	printf("\n");
	printf("option name Threads value %d\n", opts->threads);
	printf("uciok\n");
}

void engine_isready(void) {
	printf("readyok\n");
}

static Move *ucimv_to_move(UciMove *ucimv) {
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
	move_list_destroy(&moves);
	return move;
}
