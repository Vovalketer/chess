#include "hash.h"

#include <stdint.h>
#include <stdlib.h>
#include <time.h>

#include "board.h"
#include "log.h"
#include "types.h"
#include "utils.h"

static uint64_t board_key[SQ_CNT][PIECE_TYPE_CNT][PLAYER_CNT];
static uint64_t black_to_move_key;
static uint64_t castling_key[4];
static uint64_t ep_key[8];	// store a key for each column

// rand doesnt have a 64 bit version
static uint64_t rand64(void) {
	return (uint64_t) rand() << 32 | rand();
}

void hash_init(void) {
	srand(time(NULL));
	for (Player p = PLAYER_W; p < PLAYER_CNT; p++) {
		for (Square sq = SQ_A1; sq < SQ_CNT; sq++) {
			for (PieceType pt = PAWN; pt < PIECE_TYPE_CNT; pt++) {
				board_key[sq][pt][p] = rand64();
			}
		}
	}

	for (int i = 0; i < 4; i++) {
		castling_key[i] = rand64();
	}

	for (int i = 0; i < 8; i++) {
		ep_key[i] = rand64();
	}

	black_to_move_key = rand64();
}

void hash_reset(void) {
	hash_init();
}

uint64_t hash_board(Board* board) {
	uint64_t key = 0ULL;

	for (Square sq = SQ_A1; sq < SQ_CNT; sq++) {
		Piece piece = board_get_piece(board, sq);
		if (piece.type != EMPTY)
			key ^= board_key[sq][piece.type][piece.player];
	}
	if (board->ep_target != SQ_NONE)
		key ^= ep_key[utils_get_file(board->ep_target)];

	if (board_has_castling_rights(board, CASTLING_WHITE_KS))
		key ^= castling_key[0];
	if (board_has_castling_rights(board, CASTLING_WHITE_QS))
		key ^= castling_key[1];
	if (board_has_castling_rights(board, CASTLING_BLACK_KS))
		key ^= castling_key[2];
	if (board_has_castling_rights(board, CASTLING_BLACK_QS))
		key ^= castling_key[3];

	if (board_get_player_turn(board) == PLAYER_B)
		key ^= black_to_move_key;

	return key;
}

void hash_update(Board* board, Move move, uint8_t old_castling_rights, Square old_ep) {
	if (old_castling_rights != board->castling_rights) {
		// we have no way to tell if a player lost castling rights because of a move or loss of a
		// rook so we just reset the key
		board->hash = hash_board(board);
		return;
	}
	board->hash ^= board_key[move.from][move.piece.type][move.piece.player];
	switch (move.mv_type) {
		case MV_QUIET:
			board->hash ^= board_key[move.to][move.piece.type][move.piece.player];
			break;
		case MV_PAWN_DOUBLE:
			board->hash ^= board_key[move.to][move.piece.type][move.piece.player];
			break;
		case MV_KS_CASTLE:
			break;
		case MV_QS_CASTLE:
			break;
		case MV_N_PROM:
			board->hash ^= board_key[move.to][KNIGHT][move.piece.player];
			break;
		case MV_B_PROM:
			board->hash ^= board_key[move.to][BISHOP][move.piece.player];
			break;
		case MV_R_PROM:
			board->hash ^= board_key[move.to][ROOK][move.piece.player];
			break;
		case MV_Q_PROM:
			board->hash ^= board_key[move.to][QUEEN][move.piece.player];
			break;
		case MV_CAPTURE:
			board->hash ^=
				board_key[move.to][move.captured_type][utils_get_opponent(move.piece.player)];
			board->hash ^= board_key[move.to][move.piece.type][move.piece.player];
			break;
		case MV_EN_PASSANT:
			board->hash ^= board_key[utils_ep_capture_pos(move.to, move.piece.player)]
									[move.captured_type][utils_get_opponent(move.piece.player)];
			board->hash ^= board_key[move.to][move.piece.type][move.piece.player];
			break;
		case MV_N_PROM_CAPTURE:
			board->hash ^=
				board_key[move.to][move.captured_type][utils_get_opponent(move.piece.player)];
			board->hash ^= board_key[move.to][KNIGHT][move.piece.player];
			break;
		case MV_B_PROM_CAPTURE:
			board->hash ^=
				board_key[move.to][move.captured_type][utils_get_opponent(move.piece.player)];
			board->hash ^= board_key[move.to][BISHOP][move.piece.player];
			break;
		case MV_R_PROM_CAPTURE:
			board->hash ^=
				board_key[move.to][move.captured_type][utils_get_opponent(move.piece.player)];
			board->hash ^= board_key[move.to][ROOK][move.piece.player];
			break;
		case MV_Q_PROM_CAPTURE:
			board->hash ^=
				board_key[move.to][move.captured_type][utils_get_opponent(move.piece.player)];
			board->hash ^= board_key[move.to][QUEEN][move.piece.player];
			break;
		default:
			break;
	}

	if (old_ep == SQ_NONE && board->ep_target != SQ_NONE) {
		board->hash ^= ep_key[utils_get_file(board->ep_target)];
	} else if (old_ep != SQ_NONE && board->ep_target == SQ_NONE) {
		board->hash ^= ep_key[utils_get_file(old_ep)];
	}

	// flip every turn
	board->hash ^= black_to_move_key;
}
