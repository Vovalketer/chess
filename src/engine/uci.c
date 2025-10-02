#include "uci.h"

#include <ctype.h>

#include "engine_mq.h"
#include "fen.h"
#include "log.h"
#include "uci_types.h"
#include "utils.h"

#define INPUT_LIMIT 4096
#define MAX_TOKENS	1024

void uci_parse(char *str);
void uci_print(const char *str, ...);

void cmd_uci(void);
void cmd_quit(void);
void cmd_setoption(char **tok, int tokn);
void cmd_ucinewgame(void);
void cmd_isready(void);
void cmd_position(char **tok, int tokn);
void cmd_go(char **tok, int tokn);
void cmd_stop(void);
void cmd_debug(char **tok, int tokn);
void cmd_print(void);

// size_t isnt necessary here

int		tokenize(char *str, const char *delim, size_t max_tok, char **out_tokens);
bool	tok_eq(const char *str1, const char *str2);
int		tok_search_pos(char **tok, size_t tokn, const char *str);
bool	is_move_tok(const char *str);
UciMove tok_to_move(const char *str);

static bool shutdown = false;

int uci_thread(void *arg) {
	(void) arg;
	log_trace("uci thread started");
	shutdown = false;

	char input[INPUT_LIMIT];
	while (!shutdown) {
		if (fgets(input, INPUT_LIMIT, stdin)) {
			size_t len = strlen(input);
			if (len > 0 && input[len - 1] == '\n')
				input[strlen(input) - 1] = 0;  // remove the newline character
			uci_parse(input);
		}
	}
	log_trace("uci thread stopped");
	return 0;
}

void uci_shutdown(void) {
	shutdown = true;
}

void uci_parse(char *str) {
	char *tok[MAX_TOKENS];
	int	  tokn = tokenize(str, " \t", MAX_TOKENS, tok);	 // white space and tab insensitive
	if (tokn == 0)
		return;

	if (tok_eq(tok[0], "quit")) {
		cmd_quit();
	} else if (tok_eq(tok[0], "uci")) {
		cmd_uci();
	} else if (tok_eq(tok[0], "isready")) {
		cmd_isready();
	} else if (tok_eq(tok[0], "ucinewgame")) {
		cmd_ucinewgame();
	} else if (tok_eq(tok[0], "go")) {
		cmd_go(&tok[1], tokn - 1);
	} else if (tok_eq(tok[0], "stop")) {
		cmd_stop();
	} else if (tok_eq(tok[0], "position")) {
		cmd_position(&tok[1], tokn - 1);
	} else if (tok_eq(tok[0], "setoption")) {
		cmd_setoption(&tok[1], tokn - 1);
	} else if (tok_eq(tok[0], "print")) {
		cmd_print();
	} else if (tok_eq(tok[0], "debug")) {
		cmd_debug(&tok[1], tokn - 1);
	}
}

void uci_print(const char *str, ...) {
	va_list args;
	va_start(args, str);
	vfprintf(stdout, str, args);
	va_end(args);
	fprintf(stdout, "\n");
	fflush(stdout);
}

/*
 * Commands
 */
void msg_free(UciMsg *msg) {
	switch (msg->type) {
		case MSG_UCI_POSITION: {
			UciPosition *pos = msg->payload.position;
			ucimv_list_free(&pos->moves);
		} break;
		case MSG_UCI_GO: {
			UciGo *go = msg->payload.go;
			ucimv_list_free(&go->searchmoves);
		} break;
		case MSG_UCI_SETOPTION:
			free(msg->payload.set_option);
			break;
		case MSG_UCI_DEBUG:
			free(msg->payload.debug);
			break;
		default:
			break;
	}
}

UciMsg msg_create(UciMsgType type) {
	void  *arg		= NULL;
	size_t arg_size = 0;
	switch (type) {
		case MSG_UCI_POSITION:
			arg_size = sizeof(UciPosition);
			break;
		case MSG_UCI_GO:
			arg_size = sizeof(UciGo);
			break;
		case MSG_UCI_DEBUG:
			arg_size = sizeof(UciDebug);
			break;
		case MSG_UCI_SETOPTION:
			arg_size = sizeof(UciSetOption);
			break;
		default:
			break;
	}

	if (arg_size > 0) {
		arg = calloc(1, arg_size);
		if (!arg) {
			log_error("Failed to allocate command argument");
			exit(EXIT_FAILURE);
		}
		log_trace("msg_arg, %p", arg);
	}
	UciMsg msg;
	msg.type			 = type;
	msg.payload.position = arg;	 // union type, no need to determine which member gets the memory
	msg.free_payload	 = msg_free;
	return msg;
}

void cmd_uci(void) {
	log_trace("cmd_uci");
	UciMsg msg = msg_create(MSG_UCI_UCI);
	engmq_send_uci_msg(&msg);
	log_trace("cmd_uci done");
}

void cmd_quit(void) {
	log_trace("cmd_quit");
	UciMsg msg = msg_create(MSG_UCI_QUIT);
	engmq_send_uci_msg(&msg);
	shutdown = true;
}

void cmd_print(void) {
	log_trace("cmd_print");
	UciMsg msg = msg_create(MSG_UCI_PRINT);
	engmq_send_uci_msg(&msg);
}

void cmd_setoption(char **tok, int tokn) {
	log_trace("cmd_setoption");
	if (!tok_eq(tok[0], "name"))
		return;
	int threads_pos = tok_search_pos(tok, tokn, "Threads");
	if (threads_pos != -1) {
		if (!tok_eq(tok[threads_pos + 1], "value"))
			return;

		UciMsg msg							= msg_create(MSG_UCI_SETOPTION);
		msg.payload.set_option->type		= OPT_THREADS;
		msg.payload.set_option->opt.threads = strtoul(tok[threads_pos + 2], NULL, 10);
		engmq_send_uci_msg(&msg);
	}
}

void cmd_ucinewgame(void) {
	log_trace("cmd_ucinewgame");
	UciMsg msg = msg_create(MSG_UCI_UCINEWGAME);
	engmq_send_uci_msg(&msg);
}

void cmd_isready(void) {
	log_trace("cmd_isready");
	UciMsg msg = msg_create(MSG_UCI_ISREADY);
	engmq_send_uci_msg(&msg);
}

void cmd_position(char **tok, int tokn) {
	log_trace("cmd_position");
	FenString fen;
	int		  tok_idx = 0;
	if (tok_eq(tok[tok_idx], "startpos")) {
		strcpy(fen.str, "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
		tok_idx++;
	} else if (tok_eq(tok[tok_idx], "fen")) {
		tok_idx++;
		fen.str[0] = 0;

		// not enough tokens to form a valid fen
		if ((tok_idx + FEN_TOKEN_COUNT) > tokn) {
			uci_print("info string Invalid FEN");
			return;
		}

		for (int i = 0; i < FEN_TOKEN_COUNT; i++) {
			size_t fenlen = strlen(fen.str);
			size_t toklen = strlen(tok[tok_idx]);
			if ((fenlen + toklen + 1) >= FEN_MAX_LENGTH)
				return;

			log_debug("concatenating %s to %s", tok[tok_idx], fen.str);
			strcat(fen.str, tok[tok_idx]);
			// update the length after concatenating
			fenlen += toklen;
			if (i < FEN_TOKEN_COUNT - 1) {
				// add a space betweeen the fen arguments
				fen.str[fenlen]		= ' ';
				fen.str[fenlen + 1] = 0;
			}
			log_debug("current fen %s", fen.str);
			tok_idx++;
		}
		log_debug("fen len %d", strlen(fen.str));
		log_debug("fen string %s", fen.str);
	} else
		return;

	UciMoveList moves;
	ucimv_list_init(&moves);
	if (tok_eq(tok[tok_idx], "moves")) {
		tok_idx++;
		while (tok_idx < tokn) {
			if (!is_move_tok(tok[tok_idx]))
				break;
			UciMove move = tok_to_move(tok[tok_idx]);
			if (move.from != move.to) {
				ucimv_list_push_back(&moves, move);
			} else {
				break;
			}
			tok_idx++;
		}
	}

	UciMsg msg					= msg_create(MSG_UCI_POSITION);
	msg.payload.position->fen	= fen;
	msg.payload.position->moves = moves;
	engmq_send_uci_msg(&msg);
}

void cmd_go(char **tok, int tokn) {
	log_trace("cmd_go");
	// TODO: error handling on incorrect command arguments
	if (!tok)
		return;

	UciMsg msg = msg_create(MSG_UCI_GO);

	UciGo *search_opts = msg.payload.go;

	int searchmoves_idx = tok_search_pos(tok, tokn, "searchmoves");
	if (searchmoves_idx != -1) {
		ucimv_list_init(&search_opts->searchmoves);
		for (int i = searchmoves_idx + 1; i < tokn; i++) {
			if (!is_move_tok(tok[i]))
				break;
			UciMove move = tok_to_move(tok[i]);
			if (move.from != move.to) {
				ucimv_list_push_back(&search_opts->searchmoves, move);
			} else {
				break;
			}
		}
	}

	int depth_idx = tok_search_pos(tok, tokn, "depth");
	if (depth_idx != -1 && depth_idx + 1 < tokn) {
		search_opts->depth = strtoul(tok[depth_idx + 1], NULL, 10);
	}

	int nodes_idx = tok_search_pos(tok, tokn, "nodes");
	if (nodes_idx != -1 && nodes_idx + 1 < tokn) {
		search_opts->nodes = strtoul(tok[nodes_idx + 1], NULL, 10);
	}

	int mate_idx = tok_search_pos(tok, tokn, "mate");
	if (mate_idx != -1 && mate_idx + 1 < tokn) {
		search_opts->mate = strtoul(tok[mate_idx + 1], NULL, 10);
	}

	int wtime_idx = tok_search_pos(tok, tokn, "wtime");
	if (wtime_idx != -1 && wtime_idx + 1 < tokn) {
		search_opts->wtime = strtoul(tok[wtime_idx + 1], NULL, 10);
	}

	int btime_idx = tok_search_pos(tok, tokn, "btime");
	if (btime_idx != -1 && btime_idx + 1 < tokn) {
		search_opts->btime = strtoul(tok[btime_idx + 1], NULL, 10);
	}

	// movestogo depends on wtime and btime being set
	if (search_opts->wtime && search_opts->btime) {
		int movestogo_idx = tok_search_pos(tok, tokn, "movestogo");
		if (movestogo_idx != -1 && movestogo_idx + 1 < tokn) {
			search_opts->movestogo = strtoul(tok[movestogo_idx + 1], NULL, 10);
		}
	}

	int winc_idx = tok_search_pos(tok, tokn, "winc");
	if (winc_idx != -1 && winc_idx + 1 < tokn) {
		search_opts->winc = strtoul(tok[winc_idx + 1], NULL, 10);
	}

	int binc_idx = tok_search_pos(tok, tokn, "binc");
	if (binc_idx != -1 && binc_idx + 1 < tokn) {
		search_opts->binc = strtoul(tok[binc_idx + 1], NULL, 10);
	}

	int movetime_idx = tok_search_pos(tok, tokn, "movetime");
	if (movetime_idx != -1 && movetime_idx + 1 < tokn) {
		search_opts->movetime = strtoul(tok[movetime_idx + 1], NULL, 10);
	}

	int infinite_idx = tok_search_pos(tok, tokn, "infinite");
	if (infinite_idx != -1) {
		search_opts->infinite = true;
	}

	int ponder_idx = tok_search_pos(tok, tokn, "ponder");
	if (ponder_idx != -1) {
		search_opts->ponder = true;
	}

	engmq_send_uci_msg(&msg);
	log_trace("cmd_go done");
}

void cmd_stop(void) {
	log_trace("cmd_stop");
	UciMsg msg = msg_create(MSG_UCI_STOP);
	engmq_send_uci_msg(&msg);
}

void cmd_debug(char **tok, int tokn) {
	log_trace("cmd_debug");
	if (tokn >= 1 || !tok)
		return;
	bool debug;
	if (tok_eq(tok[0], "on")) {
		debug = true;
	} else if (tok_eq(tok[0], "off")) {
		debug = false;
	} else {
		return;
	}

	UciMsg msg				 = msg_create(MSG_UCI_DEBUG);
	msg.payload.debug->debug = debug;
	engmq_send_uci_msg(&msg);
}

/*
 * String helpers
 */

// converts a null terminated string into an array of tokens, the original string is destroyed
int tokenize(char *str, const char *delim, size_t max_tok, char **out_tokens) {
	log_trace("tokenizing string %s", str);
	char  *ptr = strtok(str, delim);
	size_t i   = 0;
	while (ptr && i < max_tok) {
		out_tokens[i] = ptr;
		ptr			  = strtok(NULL, delim);
		i++;
	}
	log_trace("tokens len: %zu", i);
	return i;
}

bool tok_eq(const char *str1, const char *str2) {
	if (str1 == NULL || str2 == NULL)
		return false;
	return strcmp(str1, str2) == 0;
}

UciMove tok_to_move(const char *str) {
	UciMove move = {0};
	int		len	 = strlen(str);
	if (len != 4 && len != 5) {
		return move;
	}
	int f1 = tolower(str[0]) - 'a';
	int r1 = tolower(str[1]) - '1';	 // index is 0-7
	int f2 = tolower(str[2]) - 'a';
	int r2 = tolower(str[3]) - '1';	 // index is 0-7
	if (f1 < 0 || f1 > 7 || r1 < 0 || r1 > 7 || f2 < 0 || f2 > 7 || r2 < 0 || r2 > 7) {
		return move;
	}
	char prom_char = 0;
	if (len == 5) {
		prom_char = tolower(str[4]);
		if (prom_char != 'q' && prom_char != 'r' && prom_char != 'b' && prom_char != 'n') {
			return move;
		}
	}

	Square		from = utils_fr_to_square(f1, r1);
	Square		to	 = utils_fr_to_square(f2, r2);
	UciPromType prom;
	switch (prom_char) {
		case 'q':
			prom = UCI_PROM_Q;
			break;
		case 'r':
			prom = UCI_PROM_R;
			break;
		case 'b':
			prom = UCI_PROM_B;
			break;
		case 'n':
			prom = UCI_PROM_N;
			break;
		default:
			prom = UCI_PROM_NONE;
			break;
	}
	UciMove um = {.from = from, .to = to, .prom_type = prom};
	return um;
}

int tok_search_pos(char **tok, size_t tokn, const char *str) {
	for (size_t i = 0; i < tokn; i++) {
		if (tok_eq(tok[i], str)) {
			return i;
		}
	}
	return -1;
}

bool is_move_tok(const char *str) {
	size_t len = strlen(str);
	if (len < 4 || len > 6) {
		return false;
	}
	int f1 = tolower(str[0]) - 'a';
	int r1 = tolower(str[1]) - '1';
	int f2 = tolower(str[2]) - 'a';
	int r2 = tolower(str[3]) - '1';
	if (f1 < 0 || f1 > 7 || r1 < 0 || r1 > 7 || f2 < 0 || f2 > 7 || r2 < 0 || r2 > 7) {
		return false;
	}
	return true;
}
