
#include "msg_queue.h"
#include "types.h"
#include "vector.h"

typedef struct uci_thread_args {
	struct msg_queue *msg_queue;
} UciThreadArgs;

typedef enum { UCI_PROM_NONE, UCI_PROM_N, UCI_PROM_B, UCI_PROM_R, UCI_PROM_Q } UciPromType;

typedef struct {
	Square		from;
	Square		to;
	UciPromType prom_type;
} UciMove;

VECTOR_DEFINE_TYPE(UciMove, UciMoveList, ucimv_list);

typedef enum set_option_type {
	OPT_NONE,
	OPT_THREADS,
} UciSetOptionType;

typedef struct {
	FenString	 fen;
	UciMoveList *moves;
} UciPosition;

typedef struct {
	UciMoveList searchmoves;
	uint32_t	depth;
	uint32_t	nodes;
	uint32_t	movetime;
	uint32_t	wtime;
	uint32_t	btime;
	uint32_t	winc;
	uint32_t	binc;
	uint32_t	movestogo;
	uint32_t	mate;  // mate in x moves
	bool		ponder;
	bool		infinite;
} UciGo;

typedef struct {
	bool debug;
} UciDebug;

typedef struct {
	UciSetOptionType type;

	union {
		int threads;
	} opt;
} UciSetOption;

typedef struct uci_args {
	struct msg_queue *msg_queue;
} UciArgs;
