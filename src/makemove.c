#include "makemove.h"

#include "board.h"
#include "types.h"

// TODO: validate moves, check and castling squares
bool make_move(Board *board, Move move) {
	PieceType pt_src  = move.piece;
	History	  history = (History) {.from			 = move.from,
								   .to				 = move.to,
								   .moving			 = pt_src,
								   .captured		 = EMPTY,
								   .mv_type			 = move.mv_type,
								   .ep_target		 = board->ep_target,
								   .castling_rights	 = board->castling_rights,
								   .halfmove_clock	 = board->halfmove_clock,
								   .fullmove_counter = board->fullmove_counter};

	switch (move.mv_type) {
		case MV_QUIET:
			board_move_piece(board, move.from, move.to, pt_src);
			break;
		case MV_PAWN_DOUBLE:
			board->ep_target	  = move.to;
			board->halfmove_clock = 0;
			board_move_piece(board, move.from, move.to, pt_src);
			break;
		case MV_KS_CASTLE:
			board_move_piece(board, move.from, move.to, pt_src);
			if (board->side == PLAYER_W) {
				board_move_piece(board, SQ_H1, SQ_F1, ROOK);
				board_remove_castling_rights(board, CASTLE_W_KS);
			} else {
				board_move_piece(board, SQ_H8, SQ_F8, ROOK);
				board_remove_castling_rights(board, CASTLE_B_KS);
			}
			break;
		case MV_QS_CASTLE:
			board_move_piece(board, move.from, move.to, pt_src);
			if (board->side == PLAYER_W) {
				board_move_piece(board, SQ_A1, SQ_D1, ROOK);
				board_remove_castling_rights(board, CASTLE_W_QS);
			} else {
				board_move_piece(board, SQ_A8, SQ_D8, ROOK);
				board_remove_castling_rights(board, CASTLE_B_QS);
			}
			break;
		case MV_CAPTURE:
			board->halfmove_clock = 0;
			history.captured	  = board_get_piece_type(board, move.to);
			board_move_piece(board, move.from, move.to, pt_src);
			break;
		case MV_EN_PASSANT:
			board->halfmove_clock = 0;
			history.captured	  = PAWN;
			board_move_piece(board, move.from, move.to, pt_src);
			break;
		case MV_N_PROM:
			board_remove_piece(board, move.from);
			board_set_piece(board, board->side, KNIGHT, move.to);
			break;
		case MV_B_PROM:
			board_remove_piece(board, move.from);
			board_set_piece(board, board->side, BISHOP, move.to);
			break;
		case MV_R_PROM:
			board_remove_piece(board, move.from);
			board_set_piece(board, board->side, ROOK, move.to);
			break;
		case MV_Q_PROM:
			board_remove_piece(board, move.from);
			board_set_piece(board, board->side, QUEEN, move.to);
			break;
		case MV_N_PROM_CAPTURE:
			history.captured = board_get_piece_type(board, move.to);
			board_remove_piece(board, move.from);
			board_set_piece(board, board->side, KNIGHT, move.to);
			break;
		case MV_B_PROM_CAPTURE:
			history.captured = board_get_piece_type(board, move.to);
			board_remove_piece(board, move.from);
			board_set_piece(board, board->side, BISHOP, move.to);
			break;
		case MV_R_PROM_CAPTURE:
			history.captured = board_get_piece_type(board, move.to);
			board_remove_piece(board, move.from);
			board_set_piece(board, board->side, ROOK, move.to);
			break;
		case MV_Q_PROM_CAPTURE:
			history.captured = board_get_piece_type(board, move.to);
			board_remove_piece(board, move.from);
			board_set_piece(board, board->side, QUEEN, move.to);
			break;
	}

	if (move.piece == PAWN || move.mv_type == MV_CAPTURE) {
		board->halfmove_clock = 0;
	} else {
		board->halfmove_clock++;
	}
	if (move.mv_type != MV_PAWN_DOUBLE) {
		board->ep_target = SQ_NONE;
	}

	history_append(board->history, history);
	return true;
}
