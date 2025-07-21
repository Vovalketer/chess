#ifndef TYPES_H
#define TYPES_H
#include <stdbool.h>
#include <stdint.h>

// using the enum values in this file as indexes, do not change

typedef enum {
	// clang-format off
	SQ_NONE = -1,
	SQ_A1, SQ_B1, SQ_C1, SQ_D1, SQ_E1, SQ_F1, SQ_G1, SQ_H1, //  0 ..  7
	SQ_A2, SQ_B2, SQ_C2, SQ_D2, SQ_E2, SQ_F2, SQ_G2, SQ_H2, //  8 .. 15
	SQ_A3, SQ_B3, SQ_C3, SQ_D3, SQ_E3, SQ_F3, SQ_G3, SQ_H3, // 16 .. 23
	SQ_A4, SQ_B4, SQ_C4, SQ_D4, SQ_E4, SQ_F4, SQ_G4, SQ_H4, // 24 .. 31
	SQ_A5, SQ_B5, SQ_C5, SQ_D5, SQ_E5, SQ_F5, SQ_G5, SQ_H5, // 32 .. 39
	SQ_A6, SQ_B6, SQ_C6, SQ_D6, SQ_E6, SQ_F6, SQ_G6, SQ_H6, // 40 .. 47
	SQ_A7, SQ_B7, SQ_C7, SQ_D7, SQ_E7, SQ_F7, SQ_G7, SQ_H7, // 48 .. 55
	SQ_A8, SQ_B8, SQ_C8, SQ_D8, SQ_E8, SQ_F8, SQ_G8, SQ_H8,	// 56 .. 63
	SQ_CNT = 64
	// clang-format on
} Square;

typedef enum { PLAYER_W = 0, PLAYER_B = 1, PLAYER_NONE = -1 } Player;

typedef enum {
	CASTLE_BOTH_ALL = 15,  // 1111
	CASTLE_W_BOTH	= 12,
	CASTLE_W_KS		= 8,
	CASTLE_W_QS		= 4,
	CASTLE_B_BOTH	= 3,
	CASTLE_B_KS		= 2,
	CASTLE_B_QS		= 1,
} CastlingRights;

// used as index
typedef enum {
	EMPTY = -1,
	PAWN,
	ROOK,
	KNIGHT,
	BISHOP,
	QUEEN,
	KING,
} PieceType;

// query results
typedef enum {
	PIECE_NONE,
	W_PAWN,
	W_ROOK,
	W_KNIGHT,
	W_BISHOP,
	W_QUEEN,
	W_KING,
	B_PAWN,
	B_ROOK,
	B_KNIGHT,
	B_BISHOP,
	B_QUEEN,
	B_KING,
} Piece;

typedef enum {
	MV_QUIET,
	MV_PAWN_DOUBLE,
	MV_KS_CASTLE,
	MV_QS_CASTLE,
	MV_CAPTURE,
	MV_EN_PASSANT,
	MV_N_PROM,
	MV_B_PROM,
	MV_R_PROM,
	MV_Q_PROM,
	MV_N_PROM_CAPTURE,
	MV_B_PROM_CAPTURE,
	MV_R_PROM_CAPTURE,
	MV_Q_PROM_CAPTURE,
} MoveType;

typedef struct {
	// could be optimized by using a 8 bit int to encode from/to or encode the whole move in a 16
	// bit int
	Square	  from;
	Square	  to;
	PieceType piece;
	MoveType  mv_type;
} Move;

typedef struct {
	Square	  from;
	Square	  to;
	PieceType moving;
	PieceType captured;
	MoveType  mv_type;
	Square	  ep_target;
	uint8_t	  castling_rights;
	uint8_t	  halfmove_clock;
	uint16_t  fullmove_counter;
	// uint64_t zobrist_key;
} History;

typedef struct Board Board;

#endif
