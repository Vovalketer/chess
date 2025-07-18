#ifndef TYPES_H
#define TYPES_H
#include <stdbool.h>

typedef enum {
	NONE,
	WHITE_PLAYER,
	BLACK_PLAYER,
} Player;

typedef enum {
	NO_PROMOTION = 0,
	PROMOTION_QUEEN,
	PROMOTION_BISHOP,
	PROMOTION_KNIGHT,
	PROMOTION_ROOK,
} PromotionType;

typedef struct {
	bool w_ks;
	bool w_qs;
	bool b_ks;
	bool b_qs;
} CastlingRights;

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

typedef struct GameState GameState;
typedef struct Board Board;

#endif
