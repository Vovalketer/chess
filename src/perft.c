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
uint64_t perft_divide(Board *board, int depth);

int main(int argc, char *argv[]) {
	if (argc < 3) {
		printf("Usage: %s \"<fen>\" <depth>\n", argv[0]);
		exit(EXIT_FAILURE);
	}
	Board *board = board_create();
	bitboards_init();
	if (!board_from_fen(board, argv[1])) {
		printf("Error parsing FEN\n");
		exit(EXIT_FAILURE);
	}

	uint64_t total = perft_divide(board, atoi(argv[2]));
	printf("Total nodes: %lu\n", total);

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
			nodes += perft(board, depth - 1);
			unmake_move(board);
		}
	}

	move_list_destroy(&ml);
	return nodes;
}

uint64_t perft_divide(Board *board, int depth) {
	MoveList *ml	= movegen_generate(board, board_get_player_turn(board));
	uint64_t  total = 0;

	for (size_t i = 0; i < move_list_size(ml); i++) {
		Move m;
		move_list_get(ml, i, &m);

		if (!make_move(board, m))
			continue;

		uint64_t nodes = perft(board, depth - 1);
		unmake_move(board);

		printf("%s%s: %lu\n", utils_square_to_str(m.from), utils_square_to_str(m.to), nodes);
		total += nodes;
	}
	move_list_destroy(&ml);
	return total;
}
