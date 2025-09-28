#ifndef SEARCH_H
#define SEARCH_H

#include "engine_types.h"
#include "movelist.h"

typedef struct search_thread_args {
	struct msg_queue	 *msg_queue;
	struct engine_config *config;
	bool				  shutdown;
} SearchThreadArgs;

typedef struct search_options {
	MoveList *searchmoves;
	uint32_t  depth;
	uint32_t  nodes;
	uint32_t  movetime;
	uint32_t  wtime;
	uint32_t  btime;
	uint32_t  winc;
	uint32_t  binc;
	uint32_t  movestogo;
	uint32_t  mate;	 // mate in x moves
	bool	  ponder;
	bool	  infinite;
} SearchOptions;

typedef struct search_info {
	uint32_t depth;
	uint32_t seldepth;
	// uint32_t multipv;
	uint32_t score_cp;	// score in centipawns
	uint64_t nodes;
	// uint32_t nps;
	// uint32_t hashfull;
	// uint32_t tbhits;
	uint32_t time;	// uint32_t can hold up to 1190 hours as ms

	MoveList *pv;
} SearchInfo;

int	 search_thread(void *arg);
void search_shutdown(void);
void search_reset(void);

void search_start(struct board *board, struct search_options options);
void search_stop(void);

#endif	// SEARCH_H
