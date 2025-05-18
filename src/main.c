#include <stdio.h>
#include <stdlib.h>

typedef enum {
	NONE,
	WHITE_PLAYER,
	BLACK_PLAYER,
} PlayerColor;

typedef enum {
	EMPTY,
	PAWN,
	ROOK,
	KNIGHT,
	BISHOP,
	QUEEN,
	KING,
} PieceType;

typedef struct {
	PlayerColor color;
	PieceType type;
} Piece;

typedef struct {
	Piece board[8][8];
	int turn;
} GameState;

GameState game;

void init_place_pawns(GameState *state) {
	for (int i = 0; i < 8; i++) {
		state->board[1][i].color = BLACK_PLAYER;
		state->board[1][i].type = PAWN;
	}

	for (int i = 0; i < 8; i++) {
		state->board[6][i].color = WHITE_PLAYER;
		state->board[6][i].type = PAWN;
	}
}

void init_place_main_pieces(GameState *state) {
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
	init_place_pawns(state);
	init_place_main_pieces(state);
}

void print_piece(Piece piece) {
	switch (piece.color) {
		case WHITE_PLAYER:
			switch (piece.type) {
				case PAWN:
					printf("P");
					break;
				case ROOK:
					printf("R");
					break;
				case KNIGHT:
					printf("N");
					break;
				case BISHOP:
					printf("B");
					break;
				case QUEEN:
					printf("Q");
					break;
				case KING:
					printf("K");
					break;
				default:
					break;
			}
			break;

		case BLACK_PLAYER:
			switch (piece.type) {
				case PAWN:
					printf("p");
					break;
				case ROOK:
					printf("r");
					break;
				case KNIGHT:
					printf("n");
					break;
				case BISHOP:
					printf("b");
					break;
				case QUEEN:
					printf("q");
					break;
				case KING:
					printf("k");
					break;
				default:
					break;
			}
			break;
		default:
			printf(".");
			break;
	}
};

void print_board(GameState state) {
	for (int i = 0; i < 8; i++) {
		for (int j = 0; j < 8; j++) {
			print_piece(state.board[i][j]);
		}
		printf("\n");
	}
}

int main(void) {
	init_board(&game);
	print_board(game);
	return EXIT_SUCCESS;
}
