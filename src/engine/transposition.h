#ifndef TTTABLE_H
#define TTTABLE_H

#include <stdint.h>

#include "types.h"

typedef enum { BOUND_LOWER, BOUND_EXACT, BOUND_UPPER } BoundType;

typedef struct {
	uint64_t  key;
	int		  depth;
	int		  score;
	Move	  best_move;
	BoundType bound;
} TEntry;

typedef struct TTable TTable;

void ttable_init(uint32_t size_mb);
void ttable_destroy(void);
void ttable_reset(void);
bool ttable_probe(uint64_t key, TEntry *entry);
void ttable_store(uint64_t key, int depth, int score, Move best_move, BoundType bound);

#endif
