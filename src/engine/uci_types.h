#ifndef UCI_TYPES_H
#define UCI_TYPES_H

#include "types.h"
#include "vector.h"

typedef struct uci_thread_args {
	struct eng_mq *msg_queue;
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
	FenString	fen;
	UciMoveList moves;
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
	struct eng_mq *msg_queue;
} UciArgs;

/*
 * Message types to communicate with the engine thread
 */

typedef enum uci_msg_type {
	MSG_UCI_NONE,
	MSG_UCI_QUIT,
	MSG_UCI_UCI,
	MSG_UCI_ISREADY,
	MSG_UCI_UCINEWGAME,
	MSG_UCI_POSITION,
	MSG_UCI_GO,
	MSG_UCI_STOP,
	MSG_UCI_SETOPTION,
	MSG_UCI_DEBUG,
	MSG_UCI_PRINT
} UciMsgType;

typedef struct uci_msg {
	UciMsgType type;

	union {
		UciPosition	 *position;
		UciGo		 *go;
		UciSetOption *set_option;
		UciDebug	 *debug;
	} payload;

	void (*free_payload)(struct uci_msg *msg);
} UciMsg;

#endif
