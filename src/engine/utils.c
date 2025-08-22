#include "utils.h"

#include <ctype.h>
#include <stdio.h>

#include "board.h"
#include "types.h"

// string representation of the squares
const char* square_name[SQ_CNT] = {
	"a1", "b1", "c1", "d1", "e1", "f1", "g1", "h1", "a2", "b2", "c2", "d2", "e2", "f2", "g2", "h2",
	"a3", "b3", "c3", "d3", "e3", "f3", "g3", "h3", "a4", "b4", "c4", "d4", "e4", "f4", "g4", "h4",
	"a5", "b5", "c5", "d5", "e5", "f5", "g5", "h5", "a6", "b6", "c6", "d6", "e6", "f6", "g6", "h6",
	"a7", "b7", "c7", "d7", "e7", "f7", "g7", "h7", "a8", "b8", "c8", "d8", "e8", "f8", "g8", "h8"};
const char* piece_type_name[PIECE_TYPE_CNT] = {"pawn", "rook", "knight", "bishop", "queen", "king"};
const char* move_type_name[MOVE_TYPE_CNT]	= {"quiet",
											   "pawn double",
											   "ks castle",
											   "qs castle",
											   "n prom",
											   "b prom",
											   "r prom",
											   "q prom",
											   "capture",
											   "en passant",
											   "n prom capture",
											   "b prom capture",
											   "r prom capture",
											   "q prom capture"};
const char* piece_type_white_unicode[PIECE_TYPE_CNT] = {"♙", "♖", "♘", "♗", "♕", "♔"};
const char* piece_type_black_unicode[PIECE_TYPE_CNT] = {"♟", "♜", "♞", "♝", "♛", "♚"};

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

Square utils_ep_capture_pos(Square ep_target, Player active) {
	Direction dir = active == PLAYER_W ? DIR_N : DIR_S;
	return ep_target - dir;
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

const char* utils_piece_to_unicode_str(Piece piece) {
	if (piece.type == EMPTY) {
		return ".";
	}
	if (piece.player == PLAYER_W) {
		return piece_type_white_unicode[piece.type];
	} else {
		return piece_type_black_unicode[piece.type];
	}
}

Piece utils_char_to_piece(char c) {
	Piece piece;
	if (isupper(c)) {
		piece.player = PLAYER_W;
	} else {
		piece.player = PLAYER_B;
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
	if (sqr == SQ_NONE) {
		return "no sq";
	}
	return square_name[sqr];
}

const char* utils_piece_type_to_str(PieceType type) {
	if (type == EMPTY) {
		return "-";
	}
	return piece_type_name[type];
}

const char* utils_move_type_to_str(MoveType type) {
	return move_type_name[type];
}

FixedStr utils_move_description(Board* board, Move move) {
	FixedStr str;
	Piece	 piece_from = board_get_piece(board, move.from);
	snprintf(str.str,
			 sizeof(str.str),
			 "Move: %s -> %s | Type: %s | Player: %d | Piece: %s | Captured: %s",
			 utils_square_to_str(move.from),
			 utils_square_to_str(move.to),
			 utils_move_type_to_str(move.mv_type),
			 piece_from.player,
			 utils_piece_type_to_str(piece_from.type),
			 utils_piece_type_to_str(move.captured_type));
	return str;
}

FixedStr utils_square_contents(Board* board, Square sqr) {
	FixedStr str;
	Piece	 piece = board_get_piece(board, sqr);
	snprintf(str.str,
			 sizeof(str.str),
			 "Square: %s | Player: %d | Piece: %s",
			 utils_square_to_str(sqr),
			 piece.player,
			 utils_piece_type_to_str(piece.type));
	return str;
}
