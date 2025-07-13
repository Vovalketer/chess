#include <stdint.h>
#include <stdio.h>

#include "engine.h"

uint64_t perft(GameState *state, int depth);

int captures = 0;
int castling = 0;
int ep = 0;

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
	printf("\n");
	printf("Nodes: %lu\n", nodes);
	printf("Captures: %d\n", captures);
	printf("Castling: %d\n", castling);
	printf("En passant: %d\n", ep);
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

			if (engine_move_piece(state, m->src, m->dst)) {
				const TurnRecord *tr = engine_get_last_turn_record(state);
				if (tr->move_type == MOVE_CASTLING) {
					castling++;
				}
				if (tr->captured_piece.type != EMPTY) {
					// gstate_debug_print_state(state);
					captures++;
				}
				if (tr->move_type == MOVE_EN_PASSANT) {
					ep++;
				}
				if (tr->move_type == MOVE_PROMOTION) {
				}
				nodes += perft(state, depth - 1);
				engine_undo_move(state);
			}
		}
	}
	return nodes;
}

void _print_piece_type(PieceType type) {
	printf("Piece: ");
	switch (type) {
		case PAWN:
			printf("Pawn");
			break;
		case ROOK:
			printf("Rook");
			break;
		case KNIGHT:
			printf("Knight");
			break;
		case BISHOP:
			printf("Bishop");
			break;
		case QUEEN:
			printf("Queen");
			break;
		case KING:
			printf("King");
			break;
		case EMPTY:
			printf("Empty");
			break;
		default:
			printf("Invalid piece type");
			break;
	}
	printf("\n");
}
