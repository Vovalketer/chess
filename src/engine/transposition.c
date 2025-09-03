#include "transposition.h"

#include <assert.h>
#include <stdlib.h>

#include "log.h"

struct TTable {
	TEntry*	 data;
	uint32_t size;
	uint32_t capacity;
};

TTable ttable;

void ttable_init(uint32_t size_mb) {
	size_t size = size_mb * 1024 * 1024;
	log_info("Allocating %zu bytes for transposition table", size);
	ttable.capacity = size / sizeof(*ttable.data);
	ttable.data		= calloc(ttable.capacity, sizeof(*ttable.data));
	assert(ttable.data != NULL);
}

void ttable_reset(void) {
	ttable.size = 0;
}

void ttable_destroy(void) {
	free(ttable.data);
}

bool ttable_probe(uint64_t key, TEntry* out_entry) {
	TEntry e = ttable.data[key % ttable.capacity];
	if (e.key == key) {
		// log_info("Found %llu", key);
		*out_entry = e;
		return true;
	}
	return false;
}

void ttable_store(uint64_t key, int depth, int score, Move best_move, BoundType bound) {
	TEntry* e = &ttable.data[key % ttable.capacity];
	if (e->depth < depth) {
		*e = (TEntry) {
			.key = key, .depth = depth, .score = score, .best_move = best_move, .bound = bound};
		ttable.size++;
	}
}
