#include "../include/rules.h"

#include <assert.h>

#include "../include/match.h"
#include "../include/movegen.h"
#include "../include/movelist.h"
#include "board.h"

static bool rules_is_tile_targeted_by_enemy(MatchState *state, Position pos, Player player);

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

static bool rules_is_tile_targeted_by_enemy(MatchState *state, Position target, Player player) {
	assert(state != NULL);
	assert(player != NONE);
	Board *board = match_get_board(state);
	MoveList *moves = NULL;
	bool created = move_list_create(&moves);
	assert(created != false);
	for (int col = 0; col < 8; col++) {
		for (int row = 0; row < 8; row++) {
			Position pos = (Position) {col, row};
			if (board_is_enemy(board, player, pos)) {
				move_list_clear(moves);
				movegen_generate(board, pos, moves);
				for (size_t k = 0; k < move_list_size(moves); k++) {
					Move *move = NULL;
					move_list_get(moves, k, &move);
					if (move->dst.x == target.x && move->dst.y == target.y) {
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

bool rules_is_check(MatchState *state, Player player) {
	assert(state != NULL);
	assert(player != NONE);
	const Board *board = match_get_board(state);
	Position king_pos = board_find_king_pos(board, player);
	assert(board_is_within_bounds(king_pos));
	return rules_is_tile_targeted_by_enemy(state, king_pos, player);
}

bool rules_is_check_after_move(MatchState *state, Move move) {
	assert(state != NULL);
	Player player = match_get_player_turn(state);
	Board *board = match_get_board(state);
	Piece src_piece = board_get_piece(board, move.src);
	Piece dst_piece = board_get_piece(board, move.dst);
	bool is_move = board_move_piece(board, move.src, move.dst);
	assert(is_move == true);
	bool is_check = rules_is_check(state, player);
	// undo move
	bool set_src = board_set_piece(board, src_piece, move.src);
	bool set_dst = board_set_piece(board, dst_piece, move.dst);
	assert(set_src == true);
	assert(set_dst == true);
	return is_check;
}

bool rules_is_checkmate(MatchState *state, Player player) {
	assert(state != NULL);
	assert(player != NONE);

	if (!rules_is_check(state, player)) {
		return false;
	}

	const Board *board = match_get_board(state);

	MoveList *moves = NULL;
	bool created = move_list_create(&moves);
	assert(created != false);
	for (int col = 0; col < 8; col++) {
		for (int row = 0; row < 8; row++) {
			Position pos = (Position) {col, row};
			if (board_is_friendly(board, player, pos)) {
				move_list_clear(moves);
				movegen_generate(board, pos, moves);
				for (size_t k = 0; k < move_list_size(moves); k++) {
					Move *move = NULL;
					move_list_get(moves, k, &move);
					if (!rules_is_check_after_move(state, *move)) {
						move_list_destroy(&moves);
						return false;
					}
				}
			}
		}
	}
	move_list_destroy(&moves);
	return true;
}

bool rules_is_promotion(MatchState *state, Position pos) {
	Piece piece = match_get_piece(state, pos);
	return piece.type == PAWN &&
		   ((piece.player == WHITE_PLAYER && pos.y == 0) || (piece.player == BLACK_PLAYER && pos.y == 7));
}
MoveType rules_get_move_type(MatchState *state, Move move) {
	Board *board = match_get_board(state);
	Piece src_piece = board_get_piece(board, move.src);
	Piece dst_piece = board_get_piece(board, move.dst);
	Player player = match_get_player_turn(state);

	if (src_piece.player == NONE || player != src_piece.player || src_piece.player == dst_piece.player) {
		return MOVE_INVALID;
	}

	if (rules_is_promotion(state, move.dst)) {
		return MOVE_PROMOTION;
	}
	// if (rules_is_castling_move(state, move)) {
	// 	return MOVE_CASTLING;
	// }
	// if (rules_is_en_passant(state, move)) {
	// 	return MOVE_EN_PASSANT;
	// }
	if (rules_is_valid_move(state, move)) {
		return MOVE_REGULAR;
	} else {
		return MOVE_INVALID;
	}
}
