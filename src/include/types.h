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

typedef enum {
	DIR_N = 8,
	DIR_S = -8,
	DIR_W = -1,
	DIR_E = 1,

	DIR_NW = 7,
	DIR_NE = 9,
	DIR_SW = -9,
	DIR_SE = -7,
} Direction;

typedef enum { PLAYER_W = 0, PLAYER_B = 1, PLAYER_NONE = -1 } Player;

typedef enum {
	CASTLING_NO_RIGHTS	= 0,
	CASTLING_WHITE_KS	= 1 << 0,
	CASTLING_WHITE_QS	= 1 << 1,
	CASTLING_BLACK_KS	= 1 << 2,
	CASTLING_BLACK_QS	= 1 << 3,
	CASTLING_WHITE_ALL	= CASTLING_WHITE_KS | CASTLING_WHITE_QS,
	CASTLING_BLACK_ALL	= CASTLING_BLACK_KS | CASTLING_BLACK_QS,
	CASTLING_ALL_RIGHTS = CASTLING_BLACK_ALL | CASTLING_WHITE_ALL
} CastlingRights;

// used as index
typedef enum { EMPTY = -1, PAWN, ROOK, KNIGHT, BISHOP, QUEEN, KING, PIECE_TYPE_CNT } PieceType;

// query results
typedef struct {
	PieceType type;
	Player	  player;
} Piece;

typedef enum {
	MV_QUIET,
	MV_PAWN_DOUBLE,
	MV_KS_CASTLE,
	MV_QS_CASTLE,
	MV_N_PROM,
	MV_B_PROM,
	MV_R_PROM,
	MV_Q_PROM,
	MV_CAPTURE,
	MV_EN_PASSANT,
	MV_N_PROM_CAPTURE,
	MV_B_PROM_CAPTURE,
	MV_R_PROM_CAPTURE,
	MV_Q_PROM_CAPTURE,
	MOVE_TYPE_CNT
} MoveType;

typedef struct {
	// could be optimized by using a 8 bit int to encode from/to or encode the whole move in a 16
	// bit int
	Square	  from;
	Square	  to;
	PieceType piece;
	PieceType captured;
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
	Player	  side;
	// uint64_t zobrist_key;
} History;

typedef struct Board Board;

#endif
