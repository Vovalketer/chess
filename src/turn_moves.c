#include "turn_moves.h"

bool turn_moves_contains(TurnMoves* moves, Move move) {
	for (size_t i = 0; i < turn_moves_size(moves); i++) {
		TurnPieceMoves* tpm = NULL;
		turn_moves_get(moves, i, &tpm);
		if (position_eq(tpm->pos, move.src)) {
			return move_list_contains(tpm->moves, move);
		}
	}
	return false;
}
