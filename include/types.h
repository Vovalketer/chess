#ifndef TYPES_H
#define TYPES_H

typedef enum {
	NONE,
	WHITE_PLAYER,
	BLACK_PLAYER,
} Player;

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
	Player player;
	PieceType type;
} Piece;

typedef struct BoardState BoardState;

#endif
