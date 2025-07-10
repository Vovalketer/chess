#include <stdint.h>
#include <stdio.h>

#include "engine.h"
#include "fen.h"

uint64_t perft(GameState *state, int depth);

int main(int argc, char *argv[]) {
	if (argc < 3) {
		printf("Usage: %s \"<fen>\" <depth>\n", argv[0]);
		exit(EXIT_FAILURE);
	}
	GameState *state = NULL;
	bool created = engine_create_match_from_fen(&state, argv[1]);
	if (!created) {
		printf("Error creating game\n");
		exit(EXIT_FAILURE);
	}

	uint64_t nodes = perft(state, atoi(argv[2]));
	printf("Nodes: %lu\n", nodes);
	engine_destroy_match(&state);
	return EXIT_SUCCESS;
}

uint64_t perft(GameState *state, int depth) {
	if (depth == 0) {
		return 1;
	}
	uint64_t nodes = 0;
	const TurnMoves *tm = engine_get_legal_moves(state);

	for (size_t i = 0; i < turn_moves_size(tm); i++) {
		TurnPieceMoves *tpm = NULL;
		turn_moves_get(tm, i, &tpm);

		for (size_t j = 0; j < move_list_size(tpm->moves); j++) {
			Move *m = NULL;
			move_list_get(tpm->moves, j, &m);

			if (engine_move_piece(state, tpm->pos, m->dst)) {
				nodes += perft(state, depth - 1);
				engine_undo_move(state);
			}
		}
	}
	return nodes;
}
