#include "game.h"

#include <stdlib.h>

struct MatchState {
	Piece board[8][8];
	int turn;
};

static void _init_place_pawns(MatchState *state) {
	for (int i = 0; i < 8; i++) {
		state->board[1][i].color = BLACK_PLAYER;
		state->board[1][i].type = PAWN;
	}

	for (int i = 0; i < 8; i++) {
		state->board[6][i].color = WHITE_PLAYER;
		state->board[6][i].type = PAWN;
	}
}

static void _init_place_main_pieces(MatchState *state) {
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

void init_board(MatchState *state) {
	_init_place_pawns(state);
	_init_place_main_pieces(state);
}

Player _get_tile_occupant(MatchState state, int x, int y) {
	return state.board[y][x].color;
}

bool move_piece(MatchState *state, int x_orig, int y_orig, int x_dest, int y_dest) {
	Piece orig = state->board[y_orig][x_orig];
	state->board[y_dest][x_dest].color = orig.color;
	state->board[y_dest][x_dest].type = orig.type;
	state->board[y_orig][x_orig].color = NONE;
	state->board[y_orig][x_orig].type = EMPTY;
	return true;
}

MatchState *create_game(void) {
	MatchState *state = malloc(sizeof(MatchState));
	init_board(state);
	return state;
}

void destroy_game(MatchState *state) {
	free(state);
}

Player get_player_turn(const MatchState *state) {
	// TODO: check for end of the game, then return NONE
	return (state->turn % 2 == 0) ? WHITE_PLAYER : BLACK_PLAYER;
}

int get_turn(const MatchState *state) {
	return state->turn;
}

Piece get_tile_content(const MatchState *state, int x, int y) {
	return state->board[y][x];
}
