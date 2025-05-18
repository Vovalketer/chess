#include "game.h"

static void _init_place_pawns(GameState *state) {
	for (int i = 0; i < 8; i++) {
		state->board[1][i].color = BLACK_PLAYER;
		state->board[1][i].type = PAWN;
	}

	for (int i = 0; i < 8; i++) {
		state->board[6][i].color = WHITE_PLAYER;
		state->board[6][i].type = PAWN;
	}
}

static void _init_place_main_pieces(GameState *state) {
	state->board[7][0].color = WHITE_PLAYER;
	state->board[7][0].type = ROOK;

	state->board[7][1].color = WHITE_PLAYER;
	state->board[7][1].type = KNIGHT;

	state->board[7][2].color = WHITE_PLAYER;
	state->board[7][2].type = BISHOP;

	state->board[7][3].color = WHITE_PLAYER;
	state->board[7][3].type = QUEEN;

	state->board[7][4].color = WHITE_PLAYER;
	state->board[7][4].type = KING;

	state->board[7][5].color = WHITE_PLAYER;
	state->board[7][5].type = BISHOP;

	state->board[7][6].color = WHITE_PLAYER;
	state->board[7][6].type = KNIGHT;

	state->board[7][7].color = WHITE_PLAYER;
	state->board[7][7].type = ROOK;

	state->board[0][0].color = BLACK_PLAYER;
	state->board[0][0].type = ROOK;

	state->board[0][1].color = BLACK_PLAYER;
	state->board[0][1].type = KNIGHT;

	state->board[0][2].color = BLACK_PLAYER;
	state->board[0][2].type = BISHOP;

	state->board[0][3].color = BLACK_PLAYER;
	state->board[0][3].type = QUEEN;

	state->board[0][4].color = BLACK_PLAYER;
	state->board[0][4].type = KING;

	state->board[0][5].color = BLACK_PLAYER;
	state->board[0][5].type = BISHOP;

	state->board[0][6].color = BLACK_PLAYER;
	state->board[0][6].type = KNIGHT;

	state->board[0][7].color = BLACK_PLAYER;
	state->board[0][7].type = ROOK;
}

void init_board(GameState *state) {
	_init_place_pawns(state);
	_init_place_main_pieces(state);
}

PlayerColor get_tile_occupant(GameState state, int x, int y) {
	return state.board[y][x].color;
}
