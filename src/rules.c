#include "../include/rules.h"

#include "../include/board.h"
#include "../include/movegen.h"

bool rules_is_valid_move(BoardState *state, Move move) {
	bool success;
	Piece src_piece = board_get_piece(state, move.src);
	if (src_piece.player != board_get_player_turn(state)) {
		success = false;
	} else {
		MoveList *moves;
		move_list_create(&moves);
		movegen_generate(state, move.src, moves);

		if (move_list_contains(moves, move)) {
			success = board_move_piece(state, move.src, move.dst);
		}
		move_list_destroy(&moves);
	}
	return success;
}
