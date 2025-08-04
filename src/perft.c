#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "bitboards.h"
#include "board.h"
#include "log.h"
#include "makemove.h"
#include "movegen.h"
#include "types.h"
#include "utils.h"


uint64_t perft(Board *board, int depth);

int captures   = 0;
int castling   = 0;
int ep		   = 0;
int promotions = 0;
int checks	   = 0;
int checkmates = 0;

int main(int argc, char *argv[]) {
	if (argc < 3) {
		printf("Usage: %s \"<fen>\" <depth> <divide>(optional)\n", argv[0]);
		exit(EXIT_FAILURE);
	}
	Board *board = board_create();
	bitboards_init();
	if (!board_from_fen(board, argv[1])) {
		printf("Error parsing FEN\n");
		exit(EXIT_FAILURE);
	}

	if (strcmp(argv[3], "divide") == 0) {
		printf("\t---perft divide---\n");
	}

	uint64_t nodes = perft(board, atoi(argv[2]));
	printf("\n");
	printf("Nodes: %lu\n", nodes);
	printf("Captures: %d\n", captures);
	printf("Castling: %d\n", castling);
	printf("En passant: %d\n", ep);
	printf("Promotions: %d\n", promotions);
	printf("Checks: %d\n", checks);
	printf("Checkmates: %d\n", checkmates);
	board_destroy(&board);
	return EXIT_SUCCESS;
}

uint64_t perft(Board *board, int depth) {
	if (depth == 0) {
		return 1;
	}
	uint64_t  nodes = 0;
	MoveList *ml	= movegen_generate(board, board_get_player_turn(board));
	for (size_t i = 0; i < move_list_size(ml); i++) {
		Move m;
		move_list_get(ml, i, &m);
		if (make_move(board, m)) {
			if (m.mv_type == MV_CAPTURE || m.mv_type == MV_Q_PROM_CAPTURE ||
				m.mv_type == MV_R_PROM_CAPTURE || m.mv_type == MV_B_PROM_CAPTURE ||
				m.mv_type == MV_N_PROM_CAPTURE) {
				captures++;
			}
			if (m.mv_type == MV_Q_PROM || m.mv_type == MV_R_PROM || m.mv_type == MV_N_PROM ||
				m.mv_type == MV_B_PROM || m.mv_type == MV_Q_PROM_CAPTURE ||
				m.mv_type == MV_R_PROM_CAPTURE || m.mv_type == MV_B_PROM_CAPTURE ||
				m.mv_type == MV_N_PROM_CAPTURE) {
				promotions++;
			}
			if (m.mv_type == MV_KS_CASTLE || m.mv_type == MV_QS_CASTLE) {
				castling++;
			}
			if (m.mv_type == MV_EN_PASSANT) {
				ep++;
			}
			nodes += perft(board, depth - 1);
			unmake_move(board);
		}
	}
	move_list_destroy(&ml);
	return nodes;
}

}
