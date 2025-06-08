#include "../include/engine.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "../include/match.h"
#include "../include/movegen.h"
#include "../include/movelist.h"
#include "../include/rules.h"

static void _init_match(MatchState *state);
static void _init_place_pawns(MatchState *state);
static void _init_place_main_pieces(MatchState *state);

bool engine_create_empty_board(MatchState **state) {
	MatchState *b;
	bool result = match_create(&b);
	if (!result) {
		return false;
	}
	*state = b;
	return true;
}

bool engine_create_match(MatchState **state) {
	MatchState *b;
	bool result = engine_create_empty_board(&b);
	if (!result) {
		return false;
	}
	_init_match(b);
	*state = b;
	return true;
}

MoveMask engine_get_valid_moves(const MatchState *state, Position pos) {
	MoveMask mm = {0};
	if (!match_is_within_bounds(pos)) {
		return mm;
	}
	MoveList *moves = NULL;
	move_list_create(&moves);
	movegen_generate(state, pos, moves);

	for (size_t i = 0; i < move_list_size(moves); i++) {
		Move *m = NULL;
		move_list_get(moves, i, &m);
		if (position_eq(m->src, pos)) {
			mm.mask[m->dst.y][m->dst.x] = true;
			mm.count++;
		}
	}
	move_list_destroy(&moves);
	return mm;
}

Piece engine_get_piece(const MatchState *state, Position pos) {
	return match_get_piece(state, pos);
}

bool engine_move_piece(MatchState *state, Position src, Position dst) {
	Move move = (Move) {src, dst};
	Piece piece = match_get_piece(state, src);
	if (piece.player == NONE) {
		return false;
	}
	if (!rules_is_valid_move(state, move)) {
		return false;
	}

	// clone board and calculate if the king iwll be in check after moving the piece
	MatchState *clone;
	bool cloned = match_clone(&clone, state);
	match_move_piece(clone, src, dst);
	assert(cloned != false);
	bool is_check = rules_is_check(clone, piece.player);
	match_destroy(&clone);
	if (is_check) {
		return false;
	}

	match_move_piece(state, src, dst);
	match_next_turn(state);
	return true;
}

bool engine_undo_move(MatchState *state) {
	// TODO: implement, possibly by adding a move list to the board state itself
	return false;
}

void engine_destroy_match(MatchState **state) {
	match_destroy(state);
}

static void _init_place_pawns(MatchState *state) {
	for (int i = 0; i < 8; i++) {
		bool success = match_set_piece(state, (Piece) {BLACK_PLAYER, PAWN}, (Position) {i, 1});
		if (!success) {
			exit(EXIT_FAILURE);
		}
	}

	for (int i = 0; i < 8; i++) {
		bool success = match_set_piece(state, (Piece) {WHITE_PLAYER, PAWN}, (Position) {i, 6});
		if (!success) {
			exit(EXIT_FAILURE);
		}
	}
}

static void _init_place_main_pieces(MatchState *state) {
	match_set_piece(state, (Piece) {WHITE_PLAYER, ROOK}, (Position) {0, 7});
	match_set_piece(state, (Piece) {WHITE_PLAYER, KNIGHT}, (Position) {1, 7});
	match_set_piece(state, (Piece) {WHITE_PLAYER, BISHOP}, (Position) {2, 7});
	match_set_piece(state, (Piece) {WHITE_PLAYER, QUEEN}, (Position) {3, 7});
	match_set_piece(state, (Piece) {WHITE_PLAYER, KING}, (Position) {4, 7});
	match_set_piece(state, (Piece) {WHITE_PLAYER, BISHOP}, (Position) {5, 7});
	match_set_piece(state, (Piece) {WHITE_PLAYER, KNIGHT}, (Position) {6, 7});
	match_set_piece(state, (Piece) {WHITE_PLAYER, ROOK}, (Position) {7, 7});

	match_set_piece(state, (Piece) {BLACK_PLAYER, ROOK}, (Position) {0, 0});
	match_set_piece(state, (Piece) {BLACK_PLAYER, KNIGHT}, (Position) {1, 0});
	match_set_piece(state, (Piece) {BLACK_PLAYER, BISHOP}, (Position) {2, 0});
	match_set_piece(state, (Piece) {BLACK_PLAYER, QUEEN}, (Position) {3, 0});
	match_set_piece(state, (Piece) {BLACK_PLAYER, KING}, (Position) {4, 0});
	match_set_piece(state, (Piece) {BLACK_PLAYER, BISHOP}, (Position) {5, 0});
	match_set_piece(state, (Piece) {BLACK_PLAYER, KNIGHT}, (Position) {6, 0});
	match_set_piece(state, (Piece) {BLACK_PLAYER, ROOK}, (Position) {7, 0});
}

static void _init_match(MatchState *state) {
	_init_place_pawns(state);
	_init_place_main_pieces(state);
}
