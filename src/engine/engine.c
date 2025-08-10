#include "../include/engine.h"

#include <assert.h>
#include <stdlib.h>

#include "board.h"
#include "log.h"
#include "makemove.h"
#include "movegen.h"
#include "search.h"
#include "utils.h"

bool engine_create_standard_match(Board **board) {
	Board *b = board_create();
	if (!b) {
		return false;
	}
	board_from_fen(b, "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
	*board = b;

	return true;
}

bool engine_create_match_from_fen(Board **board, const char *fen) {
	Board *b = board_create();
	if (!b || !board_from_fen(b, fen)) {
		return false;
	}
	board_print(b);
	*board = b;
	return true;
}

MoveMask engine_get_valid_moves(Board *board, Position pos) {
	Square	  sqr		  = utils_fr_to_square(pos.x, pos.y);
	MoveList *moves		  = movegen_generate(board, board->side);
	MoveMask  mm		  = {0};
	int		  valid_moves = 0;
	for (size_t i = 0; i < move_list_size(moves); i++) {
		Move *move = move_list_get(moves, i);
		if (move->from == sqr && make_move(board, *move)) {
			mm.mask[move->to / 8][move->to % 8] = true;
			valid_moves++;
			unmake_move(board);
		}
	}
	log_info("Valid moves for %d at %s = %d",
			 board_get_piece(board, sqr).player,
			 utils_square_to_str(sqr),
			 valid_moves);
	move_list_destroy(&moves);
	return mm;
}

Piece engine_get_piece(Board *board, Position pos) {
	return board_get_piece(board, utils_fr_to_square(pos.x, pos.y));
}

bool engine_move_piece(Board *board, Position from, Position to) {
	Square src = utils_fr_to_square(from.x, from.y);
	Square dst = utils_fr_to_square(to.x, to.y);
	if (!utils_is_valid_square(src) || !utils_is_valid_square(dst)) {
		log_error("Attempted to move to invalid square");
		return false;
	}

	MoveList *moves = movegen_generate(board, board->side);
	bool	  found = false;
	Move	 *mv;
	for (size_t i = 0; i < move_list_size(moves); i++) {
		mv = move_list_get(moves, i);
		if (mv->from == src && mv->to == dst) {
			found = true;
			break;
		}
	}
	move_list_destroy(&moves);

	if (!found) {
		log_error("Attempted to move to invalid square");
		return false;
	}
	log_info("Moving piece from %s to %s", utils_square_to_str(src), utils_square_to_str(dst));

	return make_move(board, *mv);
}

bool engine_autoplay_move(Board *board) {
	Move best_move = search_best_move(board, 5);
	log_info("Best move: %s", utils_move_description(board, best_move).str);
	bool success = make_move(board, best_move);
	if (!success) {
		log_error("Failed to make AI move");
	}
	return success;
}

void engine_undo_move(Board *board) {
	unmake_move(board);
}

void engine_destroy_match(Board **board) {
	board_destroy(board);
}
