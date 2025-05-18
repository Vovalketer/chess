#include <stdio.h>
#include <stdlib.h>


#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

GameState game;

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
}

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
