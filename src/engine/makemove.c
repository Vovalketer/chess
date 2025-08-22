#include "makemove.h"

#include "board.h"
#include "log.h"
#include "types.h"
#include "utils.h"

static void handle_castling_rights(Board *board, Move move, PieceType captured);

static void handle_castling_rights(Board *board, Move move, PieceType captured) {
	if (move.piece.type == ROOK) {
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
	} else if (move.piece.type == KING) {
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

bool make_move(Board *board, Move move) {
	History hist = (History) {.from				= move.from,
							  .to				= move.to,
							  .moving			= move.piece.type,
							  .captured			= move.captured_type,
							  .mv_type			= move.mv_type,
							  .ep_target		= board->ep_target,
							  .side				= board->side,
							  .castling_rights	= board->castling_rights,
							  .halfmove_clock	= board->halfmove_clock,
							  .fullmove_counter = board->fullmove_counter};

	switch (move.mv_type) {
		case MV_QUIET:
			board_move_piece(board, move.from, move.to);
			break;
		case MV_PAWN_DOUBLE:
			board->ep_target = board->side == PLAYER_W ? move.from + DIR_N : move.from + DIR_S;
			board_move_piece(board, move.from, move.to);
			break;
		case MV_KS_CASTLE: {
			Player p = board->side;
			Square f = p == PLAYER_W ? ROOK_CASTLING_W_KS_DST : ROOK_CASTLING_B_KS_DST;
			Square g = p == PLAYER_W ? KING_CASTLING_W_KS_DST : KING_CASTLING_B_KS_DST;
			Square h = p == PLAYER_W ? ROOK_CASTLING_W_KS_SRC : ROOK_CASTLING_B_KS_SRC;
			if (board_is_check(board, p) || board_get_occupant(board, f) != PLAYER_NONE ||
				board_get_occupant(board, g) != PLAYER_NONE ||
				board_is_square_threatened(board, f, p) ||
				board_is_square_threatened(board, g, p)) {
				return false;
			}
			board_move_piece(board, move.from, move.to);
			board_move_piece(board, h, f);
			board_remove_castling_rights(board,
										 p == PLAYER_W ? CASTLING_WHITE_KS : CASTLING_BLACK_KS);
			break;
		}
		case MV_QS_CASTLE: {
			Player p = board->side;
			Square a = p == PLAYER_W ? ROOK_CASTLING_W_QS_SRC : ROOK_CASTLING_B_QS_SRC;
			Square b = p == PLAYER_W ? SQ_B1 : SQ_B8;
			Square c = p == PLAYER_W ? KING_CASTLING_W_QS_DST : KING_CASTLING_B_QS_DST;
			Square d = p == PLAYER_W ? ROOK_CASTLING_W_QS_DST : ROOK_CASTLING_B_QS_DST;
			if (board_is_check(board, p) || board_get_occupant(board, d) != PLAYER_NONE ||
				board_get_occupant(board, c) != PLAYER_NONE ||
				board_get_occupant(board, b) != PLAYER_NONE ||
				board_is_square_threatened(board, d, p) ||
				board_is_square_threatened(board, c, p)) {
				return false;
			}
			board_move_piece(board, move.from, move.to);
			board_move_piece(board, a, d);
			board_remove_castling_rights(board,
										 p == PLAYER_W ? CASTLING_WHITE_QS : CASTLING_BLACK_QS);
			break;
		}
		case MV_CAPTURE:
			board_move_piece(board, move.from, move.to);
			break;
		case MV_EN_PASSANT:
			board_remove_piece(board, utils_ep_capture_pos(board->ep_target, board->side));
			board_move_piece(board, move.from, move.to);
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
			board_remove_piece(board, move.from);
			board_set_piece(board, (Piece) {.player = board->side, .type = KNIGHT}, move.to);
			break;
		case MV_B_PROM_CAPTURE:
			board_remove_piece(board, move.from);
			board_set_piece(board, (Piece) {.player = board->side, .type = BISHOP}, move.to);
			break;
		case MV_R_PROM_CAPTURE:
			board_remove_piece(board, move.from);
			board_set_piece(board, (Piece) {.player = board->side, .type = ROOK}, move.to);
			break;
		case MV_Q_PROM_CAPTURE:
			board_remove_piece(board, move.from);
			board_set_piece(board, (Piece) {.player = board->side, .type = QUEEN}, move.to);
			break;
		default:
			log_error("Invalid move type");
			break;
	}

	if (board_is_check(board, hist.side)) {
		board_apply_history(board, hist);
		return false;
	}

	if (move.piece.type == PAWN || hist.captured != EMPTY) {
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
