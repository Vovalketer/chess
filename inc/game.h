#ifndef GAME_H
#define GAME_H

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

void init_board(GameState *state);
void print_piece(Piece piece);
PlayerColor get_tile_occupant(GameState state, int x, int y);

#endif
