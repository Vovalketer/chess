#include "../include/rules.h"

#include <assert.h>

#include "../include/match.h"
#include "../include/movegen.h"
#include "../include/movelist.h"
#include "board.h"

#define KING_STARTING_X 4
#define ROOK_QS_STARTING_X 0
#define ROOK_KS_STARTING_X 7
#define WHITE_STARTING_Y 7
#define BLACK_STARTING_Y 0
#define EN_PASSANT_WHITE_ROW 3
#define EN_PASSANT_BLACK_ROW 4

static bool rules_is_tile_targeted_by_enemy(MatchState *state, Position pos, Player player);

bool rules_is_pseudo_legal_move(MatchState *state, Move move) {
	bool success = false;
	Board *board = match_get_board(state);
	Piece src_piece = board_get_piece(board, move.src);
	if (src_piece.player != match_get_player_turn(state) || src_piece.type == EMPTY) {
		success = false;
	} else {
		success = movegen_contains(board, move);
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
	if (src_piece.player == dst_piece.player) {
		return false;
	}
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

bool rules_can_castle_kingside(MatchState *state, Player player) {
	assert(state != NULL);
	assert(player != NONE);
	if (!match_is_kingside_castling_available(state, player)) {
		return false;
	}
	int row;
	if (player == WHITE_PLAYER) {
		row = 7;
	} else {
		row = 0;
	}
	Piece expect_rook = match_get_piece(state, (Position) {ROOK_KS_STARTING_X, row});
	Piece expect_king = match_get_piece(state, (Position) {KING_STARTING_X, row});
	// if castling is available then the king and the rook should be in the original positions
	assert(expect_rook.player == expect_king.player);
	assert(expect_rook.type == ROOK);
	assert(expect_king.type == KING);

	// check if the tiles between the king and the rook are empty
	Position tile1 = {6, row};
	Position tile2 = {5, row};
	if (match_get_piece(state, tile1).type != EMPTY || match_get_piece(state, tile2).type != EMPTY) {
		return false;
	}
	// check if the king is in check and if the target tiles are not being threatened by the enemy
	if (rules_is_check(state, player) || rules_is_tile_targeted_by_enemy(state, tile1, player) ||
		rules_is_tile_targeted_by_enemy(state, tile2, player)) {
		return false;
	}

	return true;
}

bool rules_can_castle_queenside(MatchState *state, Player player) {
	assert(state != NULL);
	assert(player != NONE);
	if (!match_is_queenside_castling_available(state, player)) {
		return false;
	}
	int row;
	if (player == WHITE_PLAYER) {
		row = WHITE_STARTING_Y;
	} else {
		row = BLACK_STARTING_Y;
	}

	Piece expect_rook = match_get_piece(state, (Position) {ROOK_QS_STARTING_X, row});
	Piece expect_king = match_get_piece(state, (Position) {KING_STARTING_X, row});
	// if castling is available then the king and the rook should be in the original positions
	assert(expect_rook.player == expect_king.player);
	assert(expect_rook.type == ROOK);
	assert(expect_king.type == KING);

	Position tile1 = {1, row};
	Position tile2 = {2, row};
	Position tile3 = {3, row};
	if (match_get_piece(state, tile1).type != EMPTY || match_get_piece(state, tile2).type != EMPTY ||
		match_get_piece(state, tile3).type != EMPTY) {
		return false;
	}
	if (rules_is_tile_targeted_by_enemy(state, tile2, player) ||
		rules_is_tile_targeted_by_enemy(state, tile3, player)) {
		return false;
	}

	return true;
}

bool rules_is_castling(MatchState *state, Move move) {
	assert(state != NULL);
	Board *board = match_get_board(state);
	if (move.src.y != WHITE_STARTING_Y && move.src.y != BLACK_STARTING_Y) {
		return false;
	}
	Piece src_piece = board_get_piece(board, move.src);
	Piece dst_piece = board_get_piece(board, move.dst);
	if (src_piece.type != KING || dst_piece.type != ROOK || src_piece.player != dst_piece.player) {
		return false;
	}
	Player p = src_piece.player;
	if (move.dst.x == ROOK_KS_STARTING_X) {
		return rules_can_castle_kingside(state, p);
	}
	if (move.dst.x == ROOK_QS_STARTING_X) {
		return rules_can_castle_queenside(state, p);
	}
	return false;
}

bool rules_is_en_passant(MatchState *state, Move move) {
	assert(state != NULL);
	Board *board = match_get_board(state);
	Piece pawn = board_get_piece(board, move.src);
	if (pawn.type != PAWN) {
		return false;
	}
	Player player = pawn.player;
	int step;
	int en_passant_row;
	if (player == WHITE_PLAYER) {
		step = -1;
		en_passant_row = EN_PASSANT_WHITE_ROW;
	} else {
		step = 1;
		en_passant_row = EN_PASSANT_BLACK_ROW;
	}
	// check if the pawn is standing in the row where en passant is possible and
	// is attempting to advance in diagonal by 1 square
	if (move.src.y != en_passant_row || abs(move.dst.x - move.src.x) != 1 ||
		move.dst.y != move.src.y + step) {
		return false;
	}

	Piece target = board_get_piece(board, (Position) {move.dst.x, move.dst.y - step});
	if (target.type == PAWN && target.player != player) {
		TurnRecord *record = NULL;
		bool record_get = match_get_last_turn_record(state, &record);
		assert(record_get);
		if (record->src.type != PAWN || record->src.player != target.player) {
			return false;
		}
		if (abs(record->move.dst.y - record->move.src.y) == 2) {
			return true;
		}
	}
	return false;
}

MoveType rules_get_move_type(MatchState *state, Move move) {
	Board *board = match_get_board(state);
	Piece src_piece = board_get_piece(board, move.src);
	Player player = match_get_player_turn(state);

	if (src_piece.player == NONE || player != src_piece.player || rules_is_check_after_move(state, move)) {
		return MOVE_INVALID;
	}

	// castling is our only case where src piece and dest piece are friendly
	if (rules_is_castling(state, move)) {
		return MOVE_CASTLING;
	}
	// fail fast if target is friendly
	if (board_get_piece(board, move.src).player == board_get_piece(board, move.dst).player) {
		return MOVE_INVALID;
	}

	if (rules_is_promotion(state, move.dst)) {
		return MOVE_PROMOTION;
	}
	if (rules_is_en_passant(state, move)) {
		return MOVE_EN_PASSANT;
	}
	if (rules_is_pseudo_legal_move(state, move)) {
		return MOVE_REGULAR;
	} else {
		return MOVE_INVALID;
	}
}
