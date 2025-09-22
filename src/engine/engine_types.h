#ifndef ENGINE_TYPES_H
#define ENGINE_TYPES_H

#include <stdbool.h>

typedef struct engine_config {
	unsigned int threads;
} EngineConfig;

typedef struct engine_state {
	struct engine_config *config;
	struct msg_queue	 *msg_queue;
	struct board		 *board;
} EngineState;

#endif	// ENGINE_TYPES_H
