#include "utils.h"

#include <ctype.h>

Player utils_get_opponent(Player player) {
	return (player == PLAYER_W) ? PLAYER_B : PLAYER_W;
}

int utils_get_file(Square sqr) {
	return sqr % 8;
}

int utils_get_rank(Square sqr) {
	return sqr / 8;
}

Square utils_fr_to_square(int file, int rank) {
	return file + (rank * 8);
}

bool utils_is_valid_square(Square sqr) {
	return sqr >= 0 && sqr < 64;
}

char utils_piece_to_char(Piece piece) {
	char c;
	switch (piece.type) {
		case PAWN:
			c = 'P';
			break;
		case ROOK:
			c = 'R';
			break;
		case KNIGHT:
			c = 'N';
			break;
		case BISHOP:
			c = 'B';
			break;
		case QUEEN:
			c = 'Q';
			break;
		case KING:
			c = 'K';
			break;
		case EMPTY:
			return '.';
	}
	if (piece.player == PLAYER_B) {
		c = tolower(c);
	}
	return c;
}

Piece utils_char_to_piece(char c) {
	Piece piece;
	if (isupper(c)) {
		piece.player = PLAYER_B;
	} else {
		piece.player = PLAYER_W;
	}
	char cl = tolower(c);
	switch (cl) {
		case 'p':
			piece.type = PAWN;
			break;
		case 'r':
			piece.type = ROOK;
			break;
		case 'n':
			piece.type = KNIGHT;
			break;
		case 'b':
			piece.type = BISHOP;
			break;
		case 'q':
			piece.type = QUEEN;
			break;
		case 'k':
			piece.type = KING;
			break;
		case '.':
			piece.type = EMPTY;
			break;
		default:
			piece.type = EMPTY;
			break;
	}
	return piece;
}
