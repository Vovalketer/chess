#include "makemove.h"

#include "bitboards.h"
#include "bits.h"
#include "board.h"
#include "log.h"
#include "types.h"
#include "utils.h"

static bool is_square_threatened(Board *board, Square sqr, Player player);
static bool is_check(Board *board, Player player);
static void handle_castling_rights(Board *board, Move move, PieceType captured);

static bool is_square_threatened(Board *board, Square sqr, Player player) {
	Player	 opponent	 = utils_get_opponent(player);
	uint64_t occupancies = board->occupancies[player] | board->occupancies[opponent];

	uint64_t pawn_bb =
		bitboards_get_pawn_attacks(sqr, player);  // get the attacks for our own pawns to reflect
												  // the position where the enemy pawns could be
	uint64_t knight_bb = bitboards_get_knight_attacks(sqr);
	uint64_t bishop_bb = bitboards_get_bishop_attacks(sqr, occupancies);
	uint64_t rook_bb   = bitboards_get_rook_attacks(sqr, occupancies);
	uint64_t queen_bb  = bitboards_get_queen_attacks(sqr, occupancies);
	uint64_t king_bb   = bitboards_get_king_attacks(sqr);

	// check if any of these pieces exist in the bitboards from the opponent
	// and if so, the square is threatened
	return pawn_bb & board->pieces[opponent][PAWN] || knight_bb & board->pieces[opponent][KNIGHT] ||
		   bishop_bb & board->pieces[opponent][BISHOP] || rook_bb & board->pieces[opponent][ROOK] ||
		   queen_bb & board->pieces[opponent][QUEEN] || king_bb & board->pieces[opponent][KING];
}

static void handle_castling_rights(Board *board, Move move, PieceType captured) {
	if (move.piece == ROOK) {
		if (board_get_castling_rights(board, board->side) == CASTLING_NO_RIGHTS) {
			return;
		}
		if (board->side == PLAYER_W) {
			if (move.from == SQ_H1 && board_has_castling_rights(board, CASTLING_WHITE_KS)) {
				board_remove_castling_rights(board, CASTLING_WHITE_KS);
			} else if (move.from == SQ_A1 && board_has_castling_rights(board, CASTLING_WHITE_QS)) {
				board_remove_castling_rights(board, CASTLING_WHITE_QS);
			}
		} else {
			if (move.from == SQ_H8 && board_has_castling_rights(board, CASTLING_BLACK_KS)) {
				board_remove_castling_rights(board, CASTLING_BLACK_KS);
			} else if (move.from == SQ_A8 && board_has_castling_rights(board, CASTLING_BLACK_QS)) {
				board_remove_castling_rights(board, CASTLING_BLACK_QS);
			}
		}
	} else if (move.piece == KING) {
		if (board_get_castling_rights(board, board->side) == CASTLING_NO_RIGHTS) {
			return;
		}
		if (board->side == PLAYER_W) {
			board_remove_castling_rights(board, CASTLING_WHITE_ALL);
		} else {
			board_remove_castling_rights(board, CASTLING_BLACK_ALL);
		}
	}

	if (captured == ROOK) {
		Player opponent = utils_get_opponent(board->side);
		if (board_get_castling_rights(board, opponent) == CASTLING_NO_RIGHTS) {
			return;
		}
		if (opponent == PLAYER_W) {
			if (move.to == SQ_H1) {
				board_remove_castling_rights(board, CASTLING_WHITE_KS);
			} else if (move.to == SQ_A1) {
				board_remove_castling_rights(board, CASTLING_WHITE_QS);
			}
		} else {
			if (move.to == SQ_H8) {
				board_remove_castling_rights(board, CASTLING_BLACK_KS);
			} else if (move.to == SQ_A8) {
				board_remove_castling_rights(board, CASTLING_BLACK_QS);
			}
		}
	}
}

static bool is_check(Board *board, Player player) {
	Square king_sqr = bits_get_lsb(board->pieces[player][KING]);
	return is_square_threatened(board, king_sqr, player);
}

bool make_move(Board *board, Move move) {
	History hist = (History) {.from				= move.from,
							  .to				= move.to,
							  .moving			= move.piece,
							  .captured			= EMPTY,
							  .mv_type			= move.mv_type,
							  .ep_target		= board->ep_target,
							  .side				= board->side,
							  .castling_rights	= board->castling_rights,
							  .halfmove_clock	= board->halfmove_clock,
							  .fullmove_counter = board->fullmove_counter};

	switch (move.mv_type) {
		case MV_QUIET:
			board_move_piece(board, move.from, move.to, move.piece);
			break;
		case MV_PAWN_DOUBLE:
			board->ep_target = board->side == PLAYER_W ? move.from + DIR_N : move.from + DIR_S;
			board_move_piece(board, move.from, move.to, move.piece);
			break;
		case MV_KS_CASTLE:
			if (board->side == PLAYER_W) {
				if (is_check(board, PLAYER_W) || board_get_occupant(board, SQ_F1) != PLAYER_NONE ||
					board_get_occupant(board, SQ_G1) != PLAYER_NONE ||
					is_square_threatened(board, SQ_F1, PLAYER_W) ||
					is_square_threatened(board, SQ_G1, PLAYER_W)) {
					return false;
				}
				board_move_piece(board, move.from, move.to, pt_src);
				board_move_piece(board, SQ_H1, SQ_F1, ROOK);
				board_remove_castling_rights(board, CASTLING_WHITE_KS);
			} else {
				if (is_check(board, PLAYER_B) || board_get_occupant(board, SQ_F8) != PLAYER_NONE ||
					board_get_occupant(board, SQ_G8) != PLAYER_NONE ||
					is_square_threatened(board, SQ_F8, PLAYER_B) ||
					is_square_threatened(board, SQ_G8, PLAYER_B)) {
					return false;
				}
				board_move_piece(board, move.from, move.to, pt_src);
				board_move_piece(board, SQ_H8, SQ_F8, ROOK);
				board_remove_castling_rights(board, CASTLING_BLACK_KS);
			}
			break;
		case MV_QS_CASTLE:
			if (board->side == PLAYER_W) {
				if (is_check(board, PLAYER_W) || board_get_occupant(board, SQ_D1) != PLAYER_NONE ||
					board_get_occupant(board, SQ_C1) != PLAYER_NONE ||
					board_get_occupant(board, SQ_B1) != PLAYER_NONE ||
					is_square_threatened(board, SQ_D1, PLAYER_W) ||
					is_square_threatened(board, SQ_C1, PLAYER_W)) {
					return false;
				}
				board_move_piece(board, move.from, move.to, pt_src);
				board_move_piece(board, SQ_A1, SQ_D1, ROOK);
				board_remove_castling_rights(board, CASTLING_WHITE_QS);
			} else {
				if (is_check(board, PLAYER_B) || board_get_occupant(board, SQ_D8) != PLAYER_NONE ||
					board_get_occupant(board, SQ_C8) != PLAYER_NONE ||
					board_get_occupant(board, SQ_B8) != PLAYER_NONE ||
					is_square_threatened(board, SQ_D8, PLAYER_B) ||
					is_square_threatened(board, SQ_C8, PLAYER_B)) {
					return false;
				}
				board_move_piece(board, move.from, move.to, pt_src);
				board_move_piece(board, SQ_A8, SQ_D8, ROOK);
				board_remove_castling_rights(board, CASTLING_BLACK_QS);
			}
			break;
		case MV_CAPTURE:
			hist.captured = board_get_piece_type(board, move.to);
			board_move_piece(board, move.from, move.to, move.piece);
			break;
		case MV_EN_PASSANT:
			hist.captured = PAWN;
			board_remove_piece(board, utils_ep_capture_pos(board->ep_target, board->side));
			board_move_piece(board, move.from, move.to, move.piece);
			break;
		case MV_N_PROM:
			board_remove_piece(board, move.from);
			board_set_piece(board, (Piece) {.player = board->side, .type = KNIGHT}, move.to);
			break;
		case MV_B_PROM:
			board_remove_piece(board, move.from);
			board_set_piece(board, (Piece) {.player = board->side, .type = BISHOP}, move.to);
			break;
		case MV_R_PROM:
			board_remove_piece(board, move.from);
			board_set_piece(board, (Piece) {.player = board->side, .type = ROOK}, move.to);
			break;
		case MV_Q_PROM:
			board_remove_piece(board, move.from);
			board_set_piece(board, (Piece) {.player = board->side, .type = QUEEN}, move.to);
			break;
		case MV_N_PROM_CAPTURE:
			hist.captured = board_get_piece_type(board, move.to);
			board_remove_piece(board, move.from);
			board_set_piece(board, (Piece) {.player = board->side, .type = KNIGHT}, move.to);
			break;
		case MV_B_PROM_CAPTURE:
			hist.captured = board_get_piece_type(board, move.to);
			board_remove_piece(board, move.from);
			board_set_piece(board, (Piece) {.player = board->side, .type = BISHOP}, move.to);
			break;
		case MV_R_PROM_CAPTURE:
			hist.captured = board_get_piece_type(board, move.to);
			board_remove_piece(board, move.from);
			board_set_piece(board, (Piece) {.player = board->side, .type = ROOK}, move.to);
			break;
		case MV_Q_PROM_CAPTURE:
			hist.captured = board_get_piece_type(board, move.to);
			board_remove_piece(board, move.from);
			board_set_piece(board, (Piece) {.player = board->side, .type = QUEEN}, move.to);
			break;
		default:
			log_error("Invalid move type");
			break;
	}

	if (is_check(board, hist.side)) {
		board_apply_history(board, hist);
		return false;
	}

	if (move.piece == PAWN || hist.captured != EMPTY) {
		board->halfmove_clock = 0;
	} else {
		board->halfmove_clock++;
	}
	if (move.mv_type != MV_PAWN_DOUBLE) {
		board->ep_target = SQ_NONE;
	}
	if (hist.side == PLAYER_B) {
		board->fullmove_counter++;
	}
	handle_castling_rights(board, move, hist.captured);
	board->side = utils_get_opponent(hist.side);
	history_append(board->history, hist);
	return true;
}

void unmake_move(Board *board) {
	History hist = {0};
	if (!history_pop_last(board->history, &hist)) {
		log_warning("Could not unmake move, history is empty");
		return;
	}
	board_apply_history(board, hist);
}
