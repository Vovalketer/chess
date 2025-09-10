#ifndef SEARCH_H
#define SEARCH_H

#include "engine.h"
#include "movelist.h"
#include "types.h"

typedef struct {
	MoveList* searchmoves;
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

typedef struct {
	uint32_t depth;
	uint32_t seldepth;
	// uint32_t multipv;
	uint32_t score_cp;	// score in centipawns
	uint64_t nodes;
	// uint32_t nps;
	// uint32_t hashfull;
	// uint32_t tbhits;
	uint32_t time;	// uint32_t can hold up to 1190 hours as ms

	MoveList* pv;
} SearchInfo;

void search_init(void);
void search_reset(void);
Move search_best_move(Board* board, SearchOptions* options, EngineConfig* cfg);
void search_stop(void);

#endif	// SEARCH_H
