#include "../include/engine.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "../include/board.h"
#include "../include/movegen.h"
#include "../include/movelist.h"
#include "../include/rules.h"

static void _init_match(BoardState *state);
static void _init_place_pawns(BoardState *state);
static void _init_place_main_pieces(BoardState *state);

bool engine_create_empty_board(BoardState **state) {
	BoardState *b;
	bool result = board_create(&b);
	if (!result) {
		return false;
	}
	*state = b;
	return true;
}

bool engine_create_match(BoardState **state) {
	BoardState *b;
	bool result = engine_create_empty_board(&b);
	if (!result) {
		return false;
	}
	_init_match(b);
	*state = b;
	return true;
}

MoveMask engine_get_valid_moves(const BoardState *state, Position pos) {
	MoveMask mm = {0};
	if (!board_is_within_bounds(pos)) {
		return mm;
	}
	MoveList *moves;
	move_list_create(&moves);
	movegen_generate(state, pos, moves);

	for (size_t i = 0; i < move_list_size(moves); i++) {
		Move m = move_list_get(moves, i);
		if (m.src.x == pos.x && m.src.y == pos.y) {
			mm.mask[m.dst.y][m.dst.x] = true;
			mm.count++;
		}
	}
	move_list_destroy(&moves);
	return mm;
}

Piece engine_get_piece(const BoardState *state, Position pos) {
	return board_get_piece(state, pos);
}

bool engine_move_piece(BoardState *state, Position src, Position dst) {
	Move move = (Move) {src, dst};
	Piece piece = board_get_piece(state, src);
	if (piece.player == NONE) {
		return false;
	}
	if (!rules_is_valid_move(state, move)) {
		return false;
	}

	// clone board and calculate if the king iwll be in check after moving the piece
	BoardState *clone;
	bool cloned = board_clone(&clone, state);
	board_move_piece(clone, src, dst);
	assert(cloned != false);
	bool is_check = rules_is_check(clone, piece.player);
	board_destroy(&clone);
	if (is_check) {
		return false;
	}

	board_move_piece(state, src, dst);
	board_next_turn(state);
	return true;
}

bool engine_undo_move(BoardState *state) {
	// TODO: implement, possibly by adding a move list to the board state itself
	return false;
}

void engine_destroy_match(BoardState **state) {
	board_destroy(state);
}

static void _init_place_pawns(BoardState *state) {
	for (int i = 0; i < 8; i++) {
		bool success = board_set_piece(state, (Piece) {BLACK_PLAYER, PAWN}, (Position) {i, 1});
		if (!success) {
			exit(EXIT_FAILURE);
		}
	}

	for (int i = 0; i < 8; i++) {
		bool success = board_set_piece(state, (Piece) {WHITE_PLAYER, PAWN}, (Position) {i, 6});
		if (!success) {
			exit(EXIT_FAILURE);
		}
	}
}

static void _init_place_main_pieces(BoardState *state) {
	board_set_piece(state, (Piece) {WHITE_PLAYER, ROOK}, (Position) {0, 7});
	board_set_piece(state, (Piece) {WHITE_PLAYER, KNIGHT}, (Position) {1, 7});
	board_set_piece(state, (Piece) {WHITE_PLAYER, BISHOP}, (Position) {2, 7});
	board_set_piece(state, (Piece) {WHITE_PLAYER, QUEEN}, (Position) {3, 7});
	board_set_piece(state, (Piece) {WHITE_PLAYER, KING}, (Position) {4, 7});
	board_set_piece(state, (Piece) {WHITE_PLAYER, BISHOP}, (Position) {5, 7});
	board_set_piece(state, (Piece) {WHITE_PLAYER, KNIGHT}, (Position) {6, 7});
	board_set_piece(state, (Piece) {WHITE_PLAYER, ROOK}, (Position) {7, 7});

	board_set_piece(state, (Piece) {BLACK_PLAYER, ROOK}, (Position) {0, 0});
	board_set_piece(state, (Piece) {BLACK_PLAYER, KNIGHT}, (Position) {1, 0});
	board_set_piece(state, (Piece) {BLACK_PLAYER, BISHOP}, (Position) {2, 0});
	board_set_piece(state, (Piece) {BLACK_PLAYER, QUEEN}, (Position) {3, 0});
	board_set_piece(state, (Piece) {BLACK_PLAYER, KING}, (Position) {4, 0});
	board_set_piece(state, (Piece) {BLACK_PLAYER, BISHOP}, (Position) {5, 0});
	board_set_piece(state, (Piece) {BLACK_PLAYER, KNIGHT}, (Position) {6, 0});
	board_set_piece(state, (Piece) {BLACK_PLAYER, ROOK}, (Position) {7, 0});
}

static void _init_match(BoardState *state) {
	_init_place_pawns(state);
	_init_place_main_pieces(state);
}
