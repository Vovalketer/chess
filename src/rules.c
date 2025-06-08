#include "../include/rules.h"

#include <assert.h>

#include "../include/match.h"
#include "../include/movegen.h"
#include "../include/movelist.h"
#include "board.h"

bool rules_is_valid_move(MatchState *state, Move move) {
	bool success = false;
	const Board *board = match_get_board(state);
	Piece src_piece = board_get_piece(board, move.src);
	if (src_piece.player != match_get_player_turn(state) || src_piece.type == EMPTY) {
		success = false;
	} else {
		MoveList *moves = NULL;
		move_list_create(&moves);
		// TODO: error handling
		movegen_generate(board, move.src, moves);

		if (move_list_contains(moves, move)) {
			success = true;
		}
		move_list_destroy(&moves);
	}
	return success;
}

bool rules_is_check(MatchState *state, Player player) {
	assert(state != NULL);
	assert(player != NONE);
	const Board *board = match_get_board(state);
	Position king_pos = board_find_king_pos(board, player);
	MoveList *moves;
	bool created = move_list_create(&moves);
	assert(created != false);
	for (int col = 0; col < 8; col++) {
		for (int row = 0; row < 8; row++) {
			Position pos = (Position) {col, row};
			move_list_clear(moves);
			if (board_is_enemy(board, player, pos)) {
				movegen_generate(board, pos, moves);
				for (size_t k = 0; k < move_list_size(moves); k++) {
					Move *move = NULL;
					move_list_get(moves, k, &move);
					if (move->dst.x == king_pos.x && move->dst.y == king_pos.y) {
						move_list_destroy(&moves);
						return true;
					}
				}
			}
		}
	}
	move_list_destroy(&moves);
	return false;
}
