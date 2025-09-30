#ifndef SEARCH_TYPES_H
#define SEARCH_TYPES_H

#include "movelist.h"

typedef struct search_thread_args {
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

/*
 * Message types to communicate with the engine thread
 */

typedef enum { SEARCH_MSG_NONE, SEARCH_MSG_INFO, SEARCH_MSG_STOP } SearchMsgType;

typedef struct search_msg {
	SearchMsgType type;

	union {
		struct search_info *search_info;
	} payload;

	void (*free_payload)(struct search_msg *msg);
} SearchMsg;

#endif	// SEARCH_TYPES_H
