#ifndef ENGINE_TYPES_H
#define ENGINE_TYPES_H

#include <stdbool.h>

#include "search_types.h"
#include "uci_types.h"

typedef enum {
	MSG_UCI,
	MSG_SEARCH,
} EngineMsgType;

typedef struct engine_msg {
	EngineMsgType type;

	union {
		struct uci_msg	  uci;
		struct search_msg search;
	} payload;
} EngineMsg;

#endif	// ENGINE_TYPES_H
