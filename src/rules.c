#include "../include/rules.h"

#include <assert.h>

#include "../include/game_state.h"
#include "../include/movegen.h"
#include "../include/movelist.h"
#include "board.h"
#include "log.h"
#include "turn_moves.h"

#define KING_STARTING_X 4
#define ROOK_QS_STARTING_X 0
#define ROOK_KS_STARTING_X 7
#define WHITE_STARTING_Y 7
#define BLACK_STARTING_Y 0
#define EN_PASSANT_WHITE_ROW 3
#define EN_PASSANT_WHITE_TARGET_ROW 2
#define EN_PASSANT_BLACK_ROW 4
#define EN_PASSANT_BLACK_TARGET_ROW 5
#define CASTLING_KS_KING_TARGET_COL 6
#define CASTLING_KS_ROOK_TARGET_COL 5
#define CASTLING_QS_KING_TARGET_COL 2
#define CASTLING_QS_ROOK_TARGET_COL 3

static bool rules_is_tile_targeted_by_enemy(GameState *state, Position pos, Player player);
static bool rules_can_en_passant(GameState *state, Position pos, Move *out_move);

static bool rules_is_tile_targeted_by_enemy(GameState *state, Position target, Player player) {
	assert(state != NULL);
	assert(player != NONE);
	Board *board = gstate_get_board(state);
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

bool rules_is_check(GameState *state, Player player) {
	assert(state != NULL);
	assert(player != NONE);
	const Board *board = gstate_get_board(state);
	Position king_pos = board_find_king_pos(board, player);
	assert(board_is_within_bounds(king_pos));
	return rules_is_tile_targeted_by_enemy(state, king_pos, player);
}

bool rules_is_check_after_move(GameState *state, Player player, Move move) {
	assert(state != NULL);
	Board *board = gstate_get_board(state);
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

bool rules_is_checkmate(GameState *state, Player player) {
	assert(state != NULL);
	assert(player != NONE);

	if (!rules_is_check(state, player)) {
		return false;
	}

	const Board *board = gstate_get_board(state);

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
					if (!rules_is_check_after_move(state, player, *move)) {
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

bool rules_is_promotion(GameState *state, Move move) {
	Piece piece = gstate_get_piece(state, move.src);
	return piece.type == PAWN && ((piece.player == WHITE_PLAYER && move.dst.y == BLACK_STARTING_Y) ||
								  (piece.player == BLACK_PLAYER && move.dst.y == WHITE_STARTING_Y));
}

typedef enum { _CASTLING_KS, _CASTLING_QS } CastlingType;

static int _get_castling_row(Player player) {
	return player == WHITE_PLAYER ? WHITE_STARTING_Y : BLACK_STARTING_Y;
}

static bool _validate_castling_piece_types_and_positions(GameState *state, Player player,
														 CastlingType castling_type) {
	int col = castling_type == _CASTLING_KS ? ROOK_KS_STARTING_X : ROOK_QS_STARTING_X;
	int row = _get_castling_row(player);
	Piece expect_rook = gstate_get_piece(state, (Position) {col, row});
	Piece expect_king = gstate_get_piece(state, (Position) {KING_STARTING_X, row});
	// if castling is available then the king and the rook should be in the original positions
	if (expect_rook.type != ROOK || expect_king.type != KING || expect_rook.player != expect_king.player) {
		return false;
	}
	return true;
}

bool rules_can_castle_kingside(GameState *state, Player player) {
	assert(state != NULL);
	assert(player != NONE);
	if (!gstate_has_castling_rights_kingside(state, player)) {
		return false;
	}
	if (!_validate_castling_piece_types_and_positions(state, player, _CASTLING_KS)) {
		return false;
	}

	// check if the tiles between the king and the rook are empty
	int row = _get_castling_row(player);
	Position king_target_pos = {CASTLING_KS_KING_TARGET_COL, row};
	Position rook_target_pos = {CASTLING_KS_ROOK_TARGET_COL, row};
	if (gstate_get_piece(state, king_target_pos).type != EMPTY ||
		gstate_get_piece(state, rook_target_pos).type != EMPTY) {
		return false;
	}
	// check if the king is in check and if the target tiles are not being threatened by the enemy
	if (rules_is_check(state, player) || rules_is_tile_targeted_by_enemy(state, king_target_pos, player) ||
		rules_is_tile_targeted_by_enemy(state, rook_target_pos, player)) {
		return false;
	}

	return true;
}

bool rules_can_castle_queenside(GameState *state, Player player) {
	assert(state != NULL);
	assert(player != NONE);
	if (!gstate_has_castling_rights_queenside(state, player)) {
		return false;
	}
	if (!_validate_castling_piece_types_and_positions(state, player, _CASTLING_QS)) {
		return false;
	}

	int row = _get_castling_row(player);
	Position empty_tile = {1, row};
	Position king_target_pos = {CASTLING_QS_KING_TARGET_COL, row};
	Position rook_target_pos = {CASTLING_QS_ROOK_TARGET_COL, row};
	if (gstate_get_piece(state, empty_tile).type != EMPTY ||
		gstate_get_piece(state, king_target_pos).type != EMPTY ||
		gstate_get_piece(state, rook_target_pos).type != EMPTY) {
		return false;
	}
	if (rules_is_check(state, player) || rules_is_tile_targeted_by_enemy(state, king_target_pos, player) ||
		rules_is_tile_targeted_by_enemy(state, rook_target_pos, player)) {
		return false;
	}

	return true;
}

bool rules_is_castling(GameState *state, Move move) {
	assert(state != NULL);
	Board *board = gstate_get_board(state);
	if ((move.src.y != WHITE_STARTING_Y || move.dst.y != WHITE_STARTING_Y) &&
		(move.src.y != BLACK_STARTING_Y || move.dst.y != BLACK_STARTING_Y)) {
		return false;
	}
	Piece src_piece = board_get_piece(board, move.src);
	if (src_piece.type != KING) {
		return false;
	}
	Player p = src_piece.player;
	if (move.dst.x == CASTLING_KS_KING_TARGET_COL) {
		return rules_can_castle_kingside(state, p);
	}
	if (move.dst.x == CASTLING_QS_KING_TARGET_COL) {
		return rules_can_castle_queenside(state, p);
	}
	return false;
}

bool rules_is_en_passant(GameState *state, Move move) {
	Move ep_move;
	if (!rules_can_en_passant(state, move.src, &ep_move)) {
		return false;
	}
	return position_eq(ep_move.dst, move.dst);
}

bool rules_is_valid_move(GameState *state, Move move) {
	TurnMoves *m = gstate_get_legal_moves(state);
	return turn_moves_contains(m, move);
}

MoveType rules_get_move_type(GameState *state, Move move) {
	if (!rules_is_valid_move(state, move)) {
		return MOVE_INVALID;
	}

	if (rules_is_castling(state, move)) {
		return MOVE_CASTLING;
	}

	if (rules_is_promotion(state, move)) {
		return MOVE_PROMOTION;
	}

	if (rules_is_en_passant(state, move)) {
		return MOVE_EN_PASSANT;
	}

	return MOVE_REGULAR;
}

static bool rules_can_en_passant(GameState *state, Position pos, Move *out_move) {
	Piece piece = board_get_piece(gstate_get_board(state), pos);
	if (piece.type != PAWN || !gstate_is_en_passant_available(state)) {
		return false;
	}
	int target_row = piece.player == WHITE_PLAYER ? EN_PASSANT_WHITE_TARGET_ROW : EN_PASSANT_BLACK_TARGET_ROW;
	Position ep_target = gstate_get_en_passant_target(state);
	if (pos.y == ep_target.y && abs(pos.x - ep_target.x) == 1) {
		out_move->src = pos;
		out_move->dst = (Position) {.x = ep_target.x, .y = target_row};
		return true;
	}
	return false;
}

static MoveList *rules_generate_piece_moves(GameState *state, Piece piece, Position pos) {
	log_trace("Generating piece moves for piece %d at x:%d y:%d", piece.type, pos.x, pos.y);
	Board *board = gstate_get_board(state);
	MoveList *moves = NULL;
	bool move_list_created = move_list_create(&moves);
	if (!move_list_created) {
		log_error("OOM. Failed to create move list");
		exit(1);
	}
	bool generated = movegen_generate(board, pos, moves);
	if (!generated) {
		log_error("Failed to generate piece moves");
		exit(1);
	}

	MoveList *out_moves = NULL;
	bool out_moves_create = move_list_create(&out_moves);
	if (!out_moves_create) {
		log_error("OOM. Failed to create move list");
		exit(1);
	}
	// filter invalid moves
	for (size_t i = 0; i < move_list_size(moves); i++) {
		Move *move = NULL;
		move_list_get(moves, i, &move);
		if (!rules_is_check_after_move(state, piece.player, *move)) {
			move_list_append(out_moves, *move);
		}
	}
	move_list_destroy(&moves);

	// add special moves to the list if applicable
	if (piece.type == PAWN) {
		Move tmp;
		if (rules_can_en_passant(state, pos, &tmp)) {
			move_list_append(out_moves, tmp);
		}
	}
	if (piece.type == KING) {
		if (rules_can_castle_kingside(state, piece.player)) {
			Move ks_castling =
				(Move) {.src = pos, .dst = (Position) {.x = CASTLING_KS_KING_TARGET_COL, .y = pos.y}};
			move_list_append(out_moves, ks_castling);
		}
		if (rules_can_castle_queenside(state, piece.player)) {
			Move qs_castling =
				(Move) {.src = pos, .dst = (Position) {.x = CASTLING_QS_KING_TARGET_COL, .y = pos.y}};
			move_list_append(out_moves, qs_castling);
		}
	}

	return out_moves;
}

TurnMoves *rules_generate_turn_moves(GameState *state, Player player) {
	log_trace("Generating turn moves for %d", player);
	Board *board = gstate_get_board(state);
	TurnMoves *tm = NULL;
	bool tm_create = turn_moves_create(&tm);
	if (!tm_create) {
		log_error("OOM. Failed to create turn moves");
		exit(1);
	}
	for (int col = 0; col < 8; col++) {
		for (int row = 0; row < 8; row++) {
			Position pos = (Position) {col, row};
			Piece piece = board_get_piece(board, pos);
			if (piece.player == player) {
				MoveList *moves = rules_generate_piece_moves(state, piece, pos);
				if (move_list_size(moves) == 0) {
					move_list_destroy(&moves);
					continue;
				}
				TurnPieceMoves piece_moves = {.pos = pos, .moves = moves};
				bool append = turn_moves_append(tm, piece_moves);
				if (!append) {
					log_error("Failed to append turn moves");
					exit(1);
				}
			}
		}
	}

	return tm;
}

bool rules_is_fifty_moves_draw(GameState *state) {
	assert(state != NULL);
	int halfmove_clock = gstate_get_halfmove_clock(state);

	return halfmove_clock >= 100;
}

bool rules_is_stalemate(GameState *state) {
	assert(state != NULL);
	TurnMoves *moves = gstate_get_legal_moves(state);

	return turn_moves_size(moves) == 0 && !rules_is_check(state, gstate_get_player_turn(state));
}
