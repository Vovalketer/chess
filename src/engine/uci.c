#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "bitboards.h"
#include "board.h"
#include "engine.h"
#include "fen.h"
#include "hash.h"
#include "log.h"
#include "makemove.h"
#include "movegen.h"
#include "movelist.h"
#include "search.h"
#include "transposition.h"
#include "types.h"
#include "utils.h"

#define INPUT_LIMIT			 4096
#define MAX_TOKENS			 1024
#define ENGINE_NAME			 "test_engine"
#define ENGINE_AUTHOR		 "test_author"
#define OPTS_DEFAULT_THREADS 1

void uci_parse(char *str);
void uci_print(const char *str, ...);
void uci_init_opts(EngineConfig *opts);

void cmd_uci(EngineConfig opts);
void cmd_setoption(EngineConfig *cfg, char **tok, int tokn);
void cmd_ucinewgame(void);
void cmd_isready(void);
void cmd_position(char **tok, int tokn);
void cmd_go(char **tok, int tokn);
void cmd_stop(void);
void cmd_debug(char *arg);

// size_t isnt necessary here

int	 tokenize(char *str, const char *delim, size_t max_tok, char **out_tokens);
bool tok_eq(const char *str1, const char *str2);
int	 tok_search_pos(char **tok, size_t tokn, const char *str);
bool is_move_tok(const char *str);
Move tok_to_move(const char *str);

Board		 *board		   = NULL;
bool		  is_board_set = false;
SearchOptions search_opts  = {0};
EngineConfig  engine_cfg   = {0};
Move		  best_move;

void init(void) {
	search_init();
	bitboards_init();
	ttable_init(256);
	hash_init();
}

int main(void) {
	log_set_level(LOG_TRACE);
	char input[INPUT_LIMIT];
	init();
	board = board_create();
	uci_init_opts(&engine_cfg);
	while (1) {
		if (fgets(input, INPUT_LIMIT, stdin)) {
			size_t len = strlen(input);
			if (len > 0 && input[len - 1] == '\n')
				input[strlen(input) - 1] = 0;  // remove the newline character
			uci_parse(input);
		}
	}
	return 0;
}

void uci_parse(char *str) {
	char *tok[MAX_TOKENS];
	int	  tokn = tokenize(str, " \t", MAX_TOKENS, tok);	 // white space and tab insensitive
	if (tokn == 0)
		return;

	if (tok_eq(tok[0], "quit")) {
		uci_print("bye");
		exit(EXIT_SUCCESS);
	} else if (tok_eq(tok[0], "uci")) {
		cmd_uci(engine_cfg);
	} else if (tok_eq(tok[0], "isready")) {
		// TODO: wait until the engine has finished setting up and respond
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
		cmd_setoption(&engine_cfg, &tok[1], tokn - 1);
	} else if (tok_eq(tok[0], "print")) {
		board_print(board);
	} else if (tok_eq(tok[0], "debug")) {
		cmd_debug(tok[1]);
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

void cmd_uci(EngineConfig opts) {
	uci_print("id name %s", ENGINE_NAME);
	uci_print("id author %s", ENGINE_AUTHOR);
	uci_print("");
	uci_print("option name Threads value %d", opts.threads);
	uci_print("uciok");
}

void cmd_setoption(EngineConfig *cfg, char **tok, int tokn) {
	if (!tok_eq(tok[0], "name"))
		return;
	int threads_pos = tok_search_pos(tok, tokn, "Threads");
	if (threads_pos != -1) {
		if (!tok_eq(tok[threads_pos + 1], "value"))
			return;

		cfg->threads = strtoul(tok[threads_pos + 2], NULL, 10);
		uci_print("info string using %d threads", cfg->threads);
	}
}

void cmd_ucinewgame(void) {
	// TODO: reset search state, including history heuristics
	board_destroy(&board);
	board = board_create();
	hash_reset();
	ttable_reset();
	search_reset();
}

void cmd_isready(void) {
	// TODO: check hash, bitboards, TT inits, delay response until they're ready
	uci_print("readyok");
}

void cmd_position(char **tok, int tokn) {
	// tok=0 == position, ignore
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

			strcat(fen.str, tok[tok_idx]);
			if (i < FEN_TOKEN_COUNT - 1) {
				// add a space betweeen the fen arguments
				fen.str[fenlen]		= ' ';
				fen.str[fenlen + 1] = 0;
			}
			tok_idx++;
		}
	} else
		return;

	if (!board_from_fen(board, fen.str)) {
		uci_print("info string Invalid FEN");
		return;
	}

	if (tok_eq(tok[tok_idx], "moves")) {
		tok_idx++;
		while (tok_idx < tokn) {
			if (!is_move_tok(tok[tok_idx]))
				break;
			Move move = tok_to_move(tok[tok_idx]);
			if (move.piece.type != EMPTY) {
				if (!make_move(board, move))
					break;
			} else {
				break;
			}
			tok_idx++;
		}
	}
}

void cmd_go(char **tok, int tokn) {
	// TODO: error handling on incorrect command arguments
	if (!tok)
		return;

	search_opts = (SearchOptions) {0};

	int searchmoves_idx = tok_search_pos(tok, tokn, "searchmoves");
	if (searchmoves_idx != -1) {
		if (!search_opts.searchmoves)
			search_opts.searchmoves = move_list_create();
		move_list_clear(search_opts.searchmoves);
		for (int i = searchmoves_idx + 1; i < tokn; i++) {
			if (!is_move_tok(tok[i]))
				break;
			Move move = tok_to_move(tok[i]);
			if (move.piece.type != EMPTY) {
				move_list_push_back(search_opts.searchmoves, move);
			} else {
				break;
			}
		}
	}

	int depth_idx = tok_search_pos(tok, tokn, "depth");
	if (depth_idx != -1 && depth_idx + 1 < tokn) {
		search_opts.depth = strtoul(tok[depth_idx + 1], NULL, 10);
	}

	int nodes_idx = tok_search_pos(tok, tokn, "nodes");
	if (nodes_idx != -1 && nodes_idx + 1 < tokn) {
		search_opts.nodes = strtoul(tok[nodes_idx + 1], NULL, 10);
	}

	int mate_idx = tok_search_pos(tok, tokn, "mate");
	if (mate_idx != -1 && mate_idx + 1 < tokn) {
		search_opts.mate = strtoul(tok[mate_idx + 1], NULL, 10);
	}

	int wtime_idx = tok_search_pos(tok, tokn, "wtime");
	if (wtime_idx != -1 && wtime_idx + 1 < tokn) {
		search_opts.wtime = strtoul(tok[wtime_idx + 1], NULL, 10);
	}

	int btime_idx = tok_search_pos(tok, tokn, "btime");
	if (btime_idx != -1 && btime_idx + 1 < tokn) {
		search_opts.btime = strtoul(tok[btime_idx + 1], NULL, 10);
	}

	// movestogo depends on wtime and btime being set
	if (search_opts.wtime && search_opts.btime) {
		int movestogo_idx = tok_search_pos(tok, tokn, "movestogo");
		if (movestogo_idx != -1 && movestogo_idx + 1 < tokn) {
			search_opts.movestogo = strtoul(tok[movestogo_idx + 1], NULL, 10);
		}
	}

	int winc_idx = tok_search_pos(tok, tokn, "winc");
	if (winc_idx != -1 && winc_idx + 1 < tokn) {
		search_opts.winc = strtoul(tok[winc_idx + 1], NULL, 10);
	}

	int binc_idx = tok_search_pos(tok, tokn, "binc");
	if (binc_idx != -1 && binc_idx + 1 < tokn) {
		search_opts.binc = strtoul(tok[binc_idx + 1], NULL, 10);
	}

	int movetime_idx = tok_search_pos(tok, tokn, "movetime");
	if (movetime_idx != -1 && movetime_idx + 1 < tokn) {
		search_opts.movetime = strtoul(tok[movetime_idx + 1], NULL, 10);
	}

	int infinite_idx = tok_search_pos(tok, tokn, "infinite");
	if (infinite_idx != -1) {
		search_opts.infinite = true;
	}

	int ponder_idx = tok_search_pos(tok, tokn, "ponder");
	if (ponder_idx != -1) {
		search_opts.ponder = true;
	}

	best_move		 = search_best_move(board, &search_opts, &engine_cfg);
	const char *prom = utils_prom_to_str(best_move.mv_type);
	uci_print("bestmove %s%s%s",
			  utils_square_to_str(best_move.from),
			  utils_square_to_str(best_move.to),
			  prom ? prom : "");
}

void cmd_stop(void) {
	// TODO: search is a blocking function, stop wont perform any action until search is done
	search_stop();
	const char *prom = utils_prom_to_str(best_move.mv_type);
	// TODO: ponder
	uci_print("bestmove %s%s%s",
			  utils_square_to_str(best_move.from),
			  utils_square_to_str(best_move.to),
			  prom ? prom : "");
}

void cmd_debug(char *arg) {
	// TODO set and print debug data meant for the UCI
	(void) arg;
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

Move tok_to_move(const char *str) {
	Move move = NO_MOVE;
	int	 len  = strlen(str);
	if (len != 4 && len != 5) {
		return NO_MOVE;
	}
	int f1 = tolower(str[0]) - 'a';
	int r1 = tolower(str[1]) - '1';	 // index is 0-7
	int f2 = tolower(str[2]) - 'a';
	int r2 = tolower(str[3]) - '1';	 // index is 0-7
	if (f1 < 0 || f1 > 7 || r1 < 0 || r1 > 7 || f2 < 0 || f2 > 7 || r2 < 0 || r2 > 7) {
		return NO_MOVE;
	}
	char prom = 0;
	if (len == 5) {
		prom = tolower(str[4]);
		if (prom != 'q' && prom != 'r' && prom != 'b' && prom != 'n') {
			return NO_MOVE;
		}
	}

	MoveList *moves = movegen_generate(board, board->side);
	Square	  from	= utils_fr_to_square(f1, r1);
	Square	  to	= utils_fr_to_square(f2, r2);
	for (size_t i = 0; i < move_list_size(moves); i++) {
		Move *m = move_list_at(moves, i);
		if (m->from == from && m->to == to) {
			if (prom == 0) {
				move = *m;
				break;
			} else {
				if (prom == 'q' && (m->mv_type == MV_Q_PROM || m->mv_type == MV_Q_PROM_CAPTURE)) {
					move = *m;
					break;
				}
				if (prom == 'r' && (m->mv_type == MV_R_PROM || m->mv_type == MV_R_PROM_CAPTURE)) {
					move = *m;
					break;
				}
				if (prom == 'b' && (m->mv_type == MV_B_PROM || m->mv_type == MV_B_PROM_CAPTURE)) {
					move = *m;
					break;
				}
				if (prom == 'n' && (m->mv_type == MV_N_PROM || m->mv_type == MV_N_PROM_CAPTURE)) {
					move = *m;
					break;
				}
			}
		}
	}
	move_list_destroy(&moves);
	return move;
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

void uci_init_opts(EngineConfig *opts) {
	opts->threads = OPTS_DEFAULT_THREADS;
}
