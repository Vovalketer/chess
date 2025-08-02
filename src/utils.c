#include "utils.h"

#include <ctype.h>

#include "types.h"

// string representation of the squares
const char* square_name[SQ_CNT] = {
	"a1", "b1", "c1", "d1", "e1", "f1", "g1", "h1", "a2", "b2", "c2", "d2", "e2", "f2", "g2", "h2",
	"a3", "b3", "c3", "d3", "e3", "f3", "g3", "h3", "a4", "b4", "c4", "d4", "e4", "f4", "g4", "h4",
	"a5", "b5", "c5", "d5", "e5", "f5", "g5", "h5", "a6", "b6", "c6", "d6", "e6", "f6", "g6", "h6",
	"a7", "b7", "c7", "d7", "e7", "f7", "g7", "h7", "a8", "b8", "c8", "d8", "e8", "f8", "g8", "h8"};
const char* piece_type_name[PIECE_TYPE_CNT] = {"pawn", "rook", "knight", "bishop", "queen", "king"};

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
		default:
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

const char* utils_square_to_str(Square sqr) {
	return square_name[sqr];
}

const char* utils_piece_type_to_str(PieceType type) {
	return piece_type_name[type];
}
