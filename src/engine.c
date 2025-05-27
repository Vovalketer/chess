#include "../include/engine.h"

#include <stdio.h>
#include <stdlib.h>

#include "../include/board.h"
#include "../include/movegen.h"

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

MoveMask engine_get_valid_moves(const BoardState *state, int x, int y) {
	MoveMask mm = {0};
	if (!board_is_within_bounds(x, y)) {
		return mm;
	}
	MoveList *moves;
	move_list_create(&moves);
	movegen_generate(state, x, y, moves);

	for (size_t i = 0; i < move_list_size(moves); i++) {
		Move m = move_list_get(moves, i);
		if (m.x_src == x && m.y_src == y) {
			mm.mask[m.y_dest][m.x_dest] = true;
			mm.count++;
		}
	}
	move_list_destroy(&moves);
	return mm;
}

Piece engine_get_piece(const BoardState *state, int x, int y) {
	return board_get_piece(state, x, y);
}

bool engine_move_piece(BoardState *state, int x_src, int y_src, int x_dest, int y_dest) {
	return board_move_piece(state, x_src, y_src, x_dest, y_dest);
}

void engine_destroy_match(BoardState **state) {
	board_destroy(state);
}

static void _init_place_pawns(BoardState *state) {
	for (int i = 0; i < 8; i++) {
		bool success = board_set_piece(state, (Piece) {BLACK_PLAYER, PAWN}, i, 1);
		if (!success) {
			exit(EXIT_FAILURE);
		}
	}

	for (int i = 0; i < 8; i++) {
		bool success = board_set_piece(state, (Piece) {WHITE_PLAYER, PAWN}, i, 6);
		if (!success) {
			exit(EXIT_FAILURE);
		}
	}
}

static void _init_place_main_pieces(BoardState *state) {
	board_set_piece(state, (Piece) {WHITE_PLAYER, ROOK}, 0, 7);
	board_set_piece(state, (Piece) {WHITE_PLAYER, KNIGHT}, 1, 7);
	board_set_piece(state, (Piece) {WHITE_PLAYER, BISHOP}, 2, 7);
	board_set_piece(state, (Piece) {WHITE_PLAYER, QUEEN}, 3, 7);
	board_set_piece(state, (Piece) {WHITE_PLAYER, KING}, 4, 7);
	board_set_piece(state, (Piece) {WHITE_PLAYER, BISHOP}, 5, 7);
	board_set_piece(state, (Piece) {WHITE_PLAYER, KNIGHT}, 6, 7);
	board_set_piece(state, (Piece) {WHITE_PLAYER, ROOK}, 7, 7);

	board_set_piece(state, (Piece) {BLACK_PLAYER, ROOK}, 0, 0);
	board_set_piece(state, (Piece) {BLACK_PLAYER, KNIGHT}, 1, 0);
	board_set_piece(state, (Piece) {BLACK_PLAYER, BISHOP}, 2, 0);
	board_set_piece(state, (Piece) {BLACK_PLAYER, QUEEN}, 3, 0);
	board_set_piece(state, (Piece) {BLACK_PLAYER, KING}, 4, 0);
	board_set_piece(state, (Piece) {BLACK_PLAYER, BISHOP}, 5, 0);
	board_set_piece(state, (Piece) {BLACK_PLAYER, KNIGHT}, 6, 0);
	board_set_piece(state, (Piece) {BLACK_PLAYER, ROOK}, 7, 0);
}

static void _init_match(BoardState *state) {
	_init_place_pawns(state);
	_init_place_main_pieces(state);
}
