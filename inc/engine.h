#ifndef ENGINE_H
#define ENGINE_H

#include <stdbool.h>

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
	Player color;
	PieceType type;
} Piece;

typedef struct MatchState MatchState;

MatchState *create_game(void);
void destroy_game(MatchState *state);
Player get_player_turn(const MatchState *state);
int get_turn(const MatchState *state);
Piece get_tile_content(const MatchState *state, int x, int y);
bool move_piece(MatchState *state, int x_orig, int y_orig, int x_dest, int y_dest);

#endif
