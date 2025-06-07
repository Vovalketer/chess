#include "../include/movegen.h"

#include "../include/board.h"
#include "../include/movelist.h"
#include "criterion/criterion.h"
#include "criterion/new/assert.h"

#define KNIGHT_OFFSET_ROWS 8
#define DIAG_OFFSET_ROWS 4
#define CROSS_OFFSET_ROWS 8
#define MATRIX_COLS 2
const int knight_moves[KNIGHT_OFFSET_ROWS][MATRIX_COLS] = {
	{-2, -1}, {-2, 1}, {-1, -2}, {-1, 2}, {1, -2}, {1, 2}, {2, -1}, {2, 1}};
const int cross_moves[CROSS_OFFSET_ROWS][MATRIX_COLS] = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}};
const int diag_moves[DIAG_OFFSET_ROWS][MATRIX_COLS] = {{-1, -1}, {-1, 1}, {1, -1}, {1, 1}};
BoardState *board = NULL;

void setup(void) {
	bool created = board_create(&board);
	cr_assert(created, "board_create failed");
}

void teardown(void) {
	board_destroy(&board);
	// movegen_destroy(&moves);
}

Test(movegen, generate_returns_false_when_out_of_bounds, .init = setup, .fini = teardown) {
	Position pos = (Position) {-1, -1};
	MoveList *moves = NULL;
	move_list_create(&moves);
	bool success = movegen_generate(board, pos, moves);
	cr_assert_eq(success, false, "movegen should return false when out of bounds");
	move_list_destroy(&moves);
}

Test(movegen, white_pawns_have_two_moves_at_starting_row, .init = setup, .fini = teardown) {
	int white_pawn_starting_row = 6;
	for (int i = 0; i < 8; i++) {
		Piece pawn = (Piece) {.player = WHITE_PLAYER, .type = PAWN};
		board_set_piece(board, pawn, (Position) {i, white_pawn_starting_row});
	}
	for (int i = 0; i < 8; i++) {
		MoveList *moves = NULL;
		move_list_create(&moves);
		movegen_generate(board, (Position) {i, white_pawn_starting_row}, moves);
		int moves_size = move_list_size(moves);
		cr_assert(eq(uint, moves_size, 2),
				  "white pawns should have 2 moves at their starting row, but got %d",
				  moves_size);
		move_list_destroy(&moves);
	}
}

Test(movegen, black_pawns_have_two_moves_at_starting_row, .init = setup, .fini = teardown) {
	int black_pawn_starting_row = 1;
	for (int i = 0; i < 8; i++) {
		Piece pawn = (Piece) {.player = BLACK_PLAYER, .type = PAWN};
		board_set_piece(board, pawn, (Position) {i, black_pawn_starting_row});
	}
	for (int i = 0; i < 8; i++) {
		MoveList *moves = NULL;
		move_list_create(&moves);
		movegen_generate(board, (Position) {i, black_pawn_starting_row}, moves);
		int moves_size = move_list_size(moves);
		cr_assert(eq(uint, moves_size, 2),
				  "black pawns should have 2 moves at their starting row, but got %d",
				  moves_size);
		move_list_destroy(&moves);
	}
}

Test(movegen, white_pawns_can_only_move_forward_at_non_starting_rows, .init = setup, .fini = teardown) {
	int white_pawn_starting_row = 6;
	int white_cant_advance_further_row = 0;
	// starting from 1 as 0 would lead to check if they can move to y-1
	for (int row = 0; row < 8; row++) {
		if (row == white_pawn_starting_row || row == white_cant_advance_further_row) {
			continue;
		}
		for (int col = 0; col < 8; col++) {
			Piece pawn = (Piece) {.player = WHITE_PLAYER, .type = PAWN};
			Position pos = (Position) {col, row};
			bool set_piece = board_set_piece(board, pawn, pos);
			cr_assert(set_piece, "failed to set piece at x:%d y:%d", col, row);

			MoveList *moves = NULL;
			move_list_create(&moves);
			movegen_generate(board, pos, moves);
			size_t moves_size = move_list_size(moves);
			cr_assert(eq(uint, moves_size, 1),
					  "white pawns should have 1 move at non starting row, but got %zu when generating moves "
					  "from x:%d y:%d",
					  moves_size,
					  col,
					  row);

			Move *move = NULL;
			move_list_get(moves, 0, &move);
			cr_assert(move->dst.x == col && move->dst.y == row - 1,
					  "white pawn should be able to move forward, but got x:%d y:%d -> x:%d y:%d",
					  move->src.x,
					  move->src.y,
					  move->dst.x,
					  move->dst.y);

			// clean up
			board_remove_piece(board, pos);
			move_list_destroy(&moves);
		}
	}
}

Test(movegen, black_pawns_can_only_move_forward_at_non_starting_rows, .init = setup, .fini = teardown) {
	int black_pawn_starting_row = 1;
	int black_cant_advance_further_row = 7;
	for (int row = 0; row < 8; row++) {
		if (row == black_pawn_starting_row || row == black_cant_advance_further_row) {
			continue;
		}
		for (int col = 0; col < 8; col++) {
			Piece pawn = (Piece) {.player = BLACK_PLAYER, .type = PAWN};
			Position pos = (Position) {col, row};
			bool set_piece = board_set_piece(board, pawn, pos);
			cr_assert(set_piece, "failed to set piece at x:%d y:%d", col, row);

			MoveList *moves = NULL;
			move_list_create(&moves);
			movegen_generate(board, pos, moves);
			size_t moves_size = move_list_size(moves);
			cr_assert(eq(uint, moves_size, 1),
					  "black pawns should have 1 move at non starting row, but got %zu when generating moves "
					  "from x:%d y:%d",
					  moves_size,
					  col,
					  row);

			Move *move = NULL;
			move_list_get(moves, 0, &move);
			cr_assert(move->dst.x == col && move->dst.y == row + 1,
					  "black pawn should be able to move forward, but got x:%d y:%d -> x:%d y:%d",
					  move->src.x,
					  move->src.y,
					  move->dst.x,
					  move->dst.y);

			// clean up
			board_remove_piece(board, pos);
			move_list_destroy(&moves);
		}
	}
}

Test(movegen, white_pawns_can_capture_enemies_at_ne_and_nw, .init = setup, .fini = teardown) {
	const int w_pawn_x = 4;
	const int w_pawn_y = 6;
	const Position w_pawn_pos = (Position) {w_pawn_x, w_pawn_y};
	Piece w_pawn = (Piece) {.player = WHITE_PLAYER, .type = PAWN};
	board_set_piece(board, w_pawn, w_pawn_pos);

	Piece b_pawn = (Piece) {.player = BLACK_PLAYER, .type = PAWN};
	Position b_pawn_ne_pos = (Position) {w_pawn_x - 1, w_pawn_y - 1};
	Position b_pawn_nw_pos = (Position) {w_pawn_x + 1, w_pawn_y - 1};
	Position b_pawn_n_pos = (Position) {w_pawn_x, w_pawn_y - 1};

	board_set_piece(board, b_pawn, b_pawn_ne_pos);
	board_set_piece(board, b_pawn, b_pawn_nw_pos);
	board_set_piece(board, b_pawn, b_pawn_n_pos);

	MoveList *moves = NULL;
	move_list_create(&moves);
	movegen_generate(board, w_pawn_pos, moves);

	size_t moves_size = move_list_size(moves);
	cr_assert(eq(uint, moves_size, 2), "white pawn should have 2 moves, but got %d", moves_size);

	move_list_contains(moves, (Move) {w_pawn_pos, b_pawn_ne_pos});
	move_list_contains(moves, (Move) {w_pawn_pos, b_pawn_nw_pos});

	move_list_destroy(&moves);
}

Test(movegen, black_pawns_can_capture_enemies_at_se_and_sw, .init = setup, .fini = teardown) {
	const int b_pawn_x = 4;
	const int b_pawn_y = 1;
	const Position b_pawn_pos = (Position) {b_pawn_x, b_pawn_y};
	Piece b_pawn = (Piece) {.player = BLACK_PLAYER, .type = PAWN};
	board_set_piece(board, b_pawn, b_pawn_pos);

	Piece w_pawn = (Piece) {.player = WHITE_PLAYER, .type = PAWN};
	Position w_pawn_se_pos = (Position) {b_pawn_x - 1, b_pawn_y + 1};
	Position w_pawn_sw_pos = (Position) {b_pawn_x + 1, b_pawn_y + 1};
	Position w_pawn_s_pos = (Position) {b_pawn_x, b_pawn_y + 1};

	board_set_piece(board, w_pawn, w_pawn_se_pos);
	board_set_piece(board, w_pawn, w_pawn_sw_pos);
	board_set_piece(board, w_pawn, w_pawn_s_pos);

	MoveList *moves = NULL;
	move_list_create(&moves);
	movegen_generate(board, b_pawn_pos, moves);

	size_t moves_size = move_list_size(moves);
	cr_assert(eq(uint, moves_size, 2), "black pawn should have 2 moves, but got %d", moves_size);

	move_list_contains(moves, (Move) {b_pawn_pos, w_pawn_se_pos});
	move_list_contains(moves, (Move) {b_pawn_pos, w_pawn_sw_pos});

	move_list_destroy(&moves);
}

Test(movegen, white_pawns_cannot_move_to_same_row, .init = setup, .fini = teardown) {
	const int w_pawn_x = 4;
	const int w_pawn_y = 6;
	const Position w_pawn_pos = (Position) {w_pawn_x, w_pawn_y};
	Piece w_pawn = (Piece) {.player = WHITE_PLAYER, .type = PAWN};
	board_set_piece(board, w_pawn, w_pawn_pos);

	MoveList *moves = NULL;
	move_list_create(&moves);
	movegen_generate(board, w_pawn_pos, moves);

	bool left = move_list_contains(moves, (Move) {w_pawn_pos, (Position) {w_pawn_x - 1, w_pawn_y}});
	cr_assert_eq(left, false, "white pawn should not be able to move to the west");
	bool right = move_list_contains(moves, (Move) {w_pawn_pos, (Position) {w_pawn_x + 1, w_pawn_y}});
	cr_assert_eq(right, false, "white pawn should not be able to move to the east");

	move_list_destroy(&moves);
}

Test(movegen, black_pawns_cannot_move_to_same_row, .init = setup, .fini = teardown) {
	const int b_pawn_x = 4;
	const int b_pawn_y = 1;
	const Position b_pawn_pos = (Position) {b_pawn_x, b_pawn_y};
	Piece b_pawn = (Piece) {.player = BLACK_PLAYER, .type = PAWN};
	board_set_piece(board, b_pawn, b_pawn_pos);

	MoveList *moves = NULL;
	move_list_create(&moves);
	movegen_generate(board, b_pawn_pos, moves);

	bool left = move_list_contains(moves, (Move) {b_pawn_pos, (Position) {b_pawn_x - 1, b_pawn_y}});
	cr_assert_eq(left, false, "black pawn should not be able to move to the west");
	bool right = move_list_contains(moves, (Move) {b_pawn_pos, (Position) {b_pawn_x + 1, b_pawn_y}});
	cr_assert_eq(right, false, "black pawn should not be able to move to the east");

	move_list_destroy(&moves);
}

Test(movegen, white_pawns_cannot_move_behind, .init = setup, .fini = teardown) {
	const int w_pawn_x = 4;
	const int w_pawn_y = 6;
	const Position w_pawn_pos = (Position) {w_pawn_x, w_pawn_y};
	Piece w_pawn = (Piece) {.player = WHITE_PLAYER, .type = PAWN};
	board_set_piece(board, w_pawn, w_pawn_pos);

	MoveList *moves = NULL;
	move_list_create(&moves);
	movegen_generate(board, w_pawn_pos, moves);

	bool south = move_list_contains(moves, (Move) {w_pawn_pos, (Position) {w_pawn_x, w_pawn_y + 1}});
	cr_assert_eq(south, false, "white pawn should not be able to move to the south");

	bool southwest = move_list_contains(moves, (Move) {w_pawn_pos, (Position) {w_pawn_x - 1, w_pawn_y + 1}});
	cr_assert_eq(southwest, false, "white pawn should not be able to move to the southwest");

	bool southeast = move_list_contains(moves, (Move) {w_pawn_pos, (Position) {w_pawn_x + 1, w_pawn_y + 1}});
	cr_assert_eq(southeast, false, "white pawn should not be able to move to the southeast");

	move_list_destroy(&moves);
}

Test(movegen, black_pawns_cannot_move_behind, .init = setup, .fini = teardown) {
	const int b_pawn_x = 4;
	const int b_pawn_y = 1;
	const Position b_pawn_pos = (Position) {b_pawn_x, b_pawn_y};
	Piece b_pawn = (Piece) {.player = BLACK_PLAYER, .type = PAWN};
	board_set_piece(board, b_pawn, b_pawn_pos);

	MoveList *moves = NULL;
	move_list_create(&moves);
	movegen_generate(board, b_pawn_pos, moves);

	bool south = move_list_contains(moves, (Move) {b_pawn_pos, (Position) {b_pawn_x, b_pawn_y - 1}});
	cr_assert_eq(south, false, "black pawn should not be able to move to the south");

	bool southwest = move_list_contains(moves, (Move) {b_pawn_pos, (Position) {b_pawn_x - 1, b_pawn_y - 1}});
	cr_assert_eq(southwest, false, "black pawn should not be able to move to the southwest");

	bool southeast = move_list_contains(moves, (Move) {b_pawn_pos, (Position) {b_pawn_x + 1, b_pawn_y - 1}});
	cr_assert_eq(southeast, false, "black pawn should not be able to move to the southeast");

	move_list_destroy(&moves);
}

Test(movegen, rook_can_move_in_cross, .init = setup, .fini = teardown) {
	Piece rook = (Piece) {.player = WHITE_PLAYER, .type = ROOK};
	for (int starting_row = 0; starting_row < 8; starting_row++) {
		for (int starting_col = 0; starting_col < 8; starting_col++) {
			Position starting_pos = (Position) {starting_col, starting_row};
			board_set_piece(board, rook, starting_pos);

			MoveList *moves = NULL;
			move_list_create(&moves);
			movegen_generate(board, starting_pos, moves);

			size_t moves_size = move_list_size(moves);
			cr_assert_eq(moves_size,
						 14,
						 "rook should have 14 moves,but got %zu at pos x:%d y:%d",
						 moves_size,
						 starting_col,
						 starting_row);
			for (int row = 0; row < 8; row++) {
				for (int col = 0; col < 8; col++) {
					Position pos = {col, row};
					Move move = (Move) {starting_pos, pos};
					if (col == starting_col && row == starting_row) {
						// skip the starting position
						continue;
					}
					if (starting_col == col) {
						cr_assert_eq(move_list_contains(moves, move),
									 true,
									 "rook should be able to move horizontally");
					} else if (starting_row == row) {
						cr_assert_eq(
							move_list_contains(moves, move), true, "rook should be able to move vertically");
					} else {
						cr_assert_eq(move_list_contains(moves, move),
									 false,
									 "rook should not be able to move in non cross patterns");
					}
				}
			}
			board_remove_piece(board, starting_pos);
			move_list_destroy(&moves);
		}
	}
}

Test(movegen, rook_cant_go_over_allies, .init = setup, .fini = teardown) {
	int rook_x = 4;
	int rook_y = 3;
	Piece rook = (Piece) {.player = WHITE_PLAYER, .type = ROOK};
	Position rook_pos = (Position) {rook_x, rook_y};
	board_set_piece(board, rook, rook_pos);

	Piece pawn = (Piece) {.player = WHITE_PLAYER, .type = PAWN};
	Position pawn_pos = (Position) {rook_x, rook_y + 1};
	board_set_piece(board, pawn, pawn_pos);
	pawn_pos = (Position) {rook_x, rook_y - 1};
	board_set_piece(board, pawn, pawn_pos);
	pawn_pos = (Position) {rook_x + 1, rook_y};
	board_set_piece(board, pawn, pawn_pos);
	pawn_pos = (Position) {rook_x - 1, rook_y};
	board_set_piece(board, pawn, pawn_pos);

	MoveList *moves = NULL;
	move_list_create(&moves);
	movegen_generate(board, rook_pos, moves);

	size_t moves_size = move_list_size(moves);
	cr_assert_eq(moves_size, 0, "rook should have no moves, but got %zu", moves_size);

	move_list_destroy(&moves);
}

Test(movegen, rook_cant_go_over_enemies, .init = setup, .fini = teardown) {
	int rook_x = 4;
	int rook_y = 3;
	Piece rook = (Piece) {.player = WHITE_PLAYER, .type = ROOK};
	Position rook_pos = (Position) {rook_x, rook_y};
	board_set_piece(board, rook, rook_pos);

	Piece enemy_pawn = (Piece) {.player = BLACK_PLAYER, .type = PAWN};
	// should be able to capture these
	Position enemy_pawn_pos = (Position) {rook_x, rook_y + 1};
	board_set_piece(board, enemy_pawn, enemy_pawn_pos);
	enemy_pawn_pos = (Position) {rook_x, rook_y - 1};
	board_set_piece(board, enemy_pawn, enemy_pawn_pos);
	enemy_pawn_pos = (Position) {rook_x + 1, rook_y};
	board_set_piece(board, enemy_pawn, enemy_pawn_pos);
	enemy_pawn_pos = (Position) {rook_x - 1, rook_y};
	board_set_piece(board, enemy_pawn, enemy_pawn_pos);

	// should not be able to capture these
	Position enemy_pawn_pos2 = (Position) {rook_x, rook_y + 2};
	board_set_piece(board, enemy_pawn, enemy_pawn_pos2);
	enemy_pawn_pos2 = (Position) {rook_x, rook_y - 2};
	board_set_piece(board, enemy_pawn, enemy_pawn_pos2);
	enemy_pawn_pos2 = (Position) {rook_x + 2, rook_y};
	board_set_piece(board, enemy_pawn, enemy_pawn_pos2);
	enemy_pawn_pos2 = (Position) {rook_x - 2, rook_y};
	board_set_piece(board, enemy_pawn, enemy_pawn_pos2);

	MoveList *moves = NULL;
	move_list_create(&moves);
	movegen_generate(board, rook_pos, moves);

	size_t moves_size = move_list_size(moves);
	cr_assert_eq(moves_size, 4, "rook should have no moves, but got %zu", moves_size);

	move_list_destroy(&moves);
}

Test(movegen, knight_can_move_in_L, .init = setup, .fini = teardown) {
	int knight_x = 4;
	int knight_y = 3;
	Piece knight = (Piece) {.player = WHITE_PLAYER, .type = KNIGHT};
	Position knight_pos = (Position) {knight_x, knight_y};
	board_set_piece(board, knight, knight_pos);

	MoveList *moves = NULL;
	move_list_create(&moves);
	movegen_generate(board, knight_pos, moves);

	size_t moves_size = move_list_size(moves);
	cr_assert_eq(moves_size, 8, "knight should have 8 moves, but got %zu", moves_size);

	for (int i = 0; i < 8; i++) {
		int dx = knight_moves[i][0];
		int dy = knight_moves[i][1];
		Position target_pos = (Position) {knight_x + dx, knight_y + dy};
		Move move = (Move) {knight_pos, target_pos};
		cr_assert_eq(move_list_contains(moves, move), true, "knight should be able to move in L");
	}

	move_list_destroy(&moves);
}

Test(movegen, generate_moves_for_knight_on_board_edge, .init = setup, .fini = teardown) {
	Position knight_ne = (Position) {0, 0};
	Position knight_nw = (Position) {7, 0};
	Position knight_se = (Position) {0, 7};
	Position knight_sw = (Position) {7, 7};
	board_set_piece(board, (Piece) {.player = WHITE_PLAYER, .type = KNIGHT}, knight_ne);
	board_set_piece(
		board, (Piece) {.player = WHITE_PLAYER, .type = KNIGHT}, (Position) {knight_nw.x, knight_nw.y});
	board_set_piece(
		board, (Piece) {.player = WHITE_PLAYER, .type = KNIGHT}, (Position) {knight_se.x, knight_se.y});
	board_set_piece(
		board, (Piece) {.player = WHITE_PLAYER, .type = KNIGHT}, (Position) {knight_sw.x, knight_sw.y});

	Position knight_pos_arr[4] = {knight_ne, knight_nw, knight_se, knight_sw};
	for (int i = 0; i < 4; i++) {
		MoveList *moves = NULL;
		move_list_create(&moves);

		movegen_generate(board, knight_pos_arr[i], moves);
		size_t moves_size = move_list_size(moves);
		cr_assert_eq(moves_size, 2, "knight should have 2 moves, but got %zu", moves_size);

		int dx = knight_moves[i][0];
		int dy = knight_moves[i][1];
		Position knight_pos = knight_pos_arr[i];
		Position target_pos = (Position) {knight_pos.x + dx, knight_pos.y + dy};

		Move move = (Move) {knight_pos, target_pos};
		bool contains = move_list_contains(moves, move);

		if (board_is_within_bounds(target_pos)) {
			cr_assert_eq(contains,
						 true,
						 "knight at the corner x:%d y:%d should be able to move to x:%d y:%d",
						 knight_pos.x,
						 knight_pos.y,
						 target_pos.x,
						 target_pos.y);

		} else {
			cr_assert_eq(contains,
						 false,
						 "knight at the corner x:%d y:%d should not be able to move to x:%d y:%d",
						 knight_pos.x,
						 knight_pos.y,
						 target_pos.x,
						 target_pos.y);
		}
		move_list_destroy(&moves);
	}
}

Test(movegen, bishop_can_move_in_diagonals, .init = setup, .fini = teardown) {
	int bishop_x = 3;
	int bishop_y = 3;
	Piece bishop = (Piece) {.player = WHITE_PLAYER, .type = BISHOP};
	Position bishop_pos = (Position) {bishop_x, bishop_y};
	board_set_piece(board, bishop, bishop_pos);

	MoveList *moves = NULL;
	move_list_create(&moves);
	movegen_generate(board, bishop_pos, moves);

	size_t moves_size = move_list_size(moves);
	cr_assert_eq(moves_size, 13, "bishop should have 13 moves, but got %zu", moves_size);

	for (int row = 0; row < 8; row++) {
		for (int col = 0; col < 8; col++) {
			Move move = (Move) {bishop_pos, (Position) {col, row}};
			if (col == bishop_x && row == bishop_y) {
				// skip the starting position
				continue;
			}
			bool contains = move_list_contains(moves, move);
			if (col == row || col + row == 6) {
				cr_assert_eq(contains,
							 true,
							 "bishop at x:%d y:%d should be able to move to position x:%d y:%d",
							 bishop_x,
							 bishop_y,
							 col,
							 row);
			} else {
				cr_assert_eq(contains,
							 false,
							 "bishop at x:%d y:%d should not be able to move to position x:%d y:%d",
							 bishop_x,
							 bishop_y,
							 col,
							 row);
			}
		}
	}

	move_list_destroy(&moves);
}

Test(movegen, bishop_cant_skip_allies, .init = setup, .fini = teardown) {
	int bishop_x = 3;
	int bishop_y = 3;
	Piece bishop = (Piece) {.player = WHITE_PLAYER, .type = BISHOP};
	Position bishop_pos = (Position) {bishop_x, bishop_y};
	board_set_piece(board, bishop, bishop_pos);

	Piece pawn = (Piece) {.player = WHITE_PLAYER, .type = PAWN};
	Position pawn_pos = (Position) {bishop_x + 1, bishop_y + 1};
	board_set_piece(board, pawn, pawn_pos);
	pawn_pos = (Position) {bishop_x - 1, bishop_y - 1};
	board_set_piece(board, pawn, pawn_pos);
	pawn_pos = (Position) {bishop_x - 1, bishop_y + 1};
	board_set_piece(board, pawn, pawn_pos);
	pawn_pos = (Position) {bishop_x + 1, bishop_y - 1};
	board_set_piece(board, pawn, pawn_pos);

	MoveList *moves = NULL;
	move_list_create(&moves);
	movegen_generate(board, bishop_pos, moves);

	size_t moves_size = move_list_size(moves);
	cr_assert_eq(moves_size, 0, "bishop should have no moves, but got %zu", moves_size);

	move_list_destroy(&moves);
}

Test(movegen, bishop_cant_skip_enemies, .init = setup, .fini = teardown) {
	int bishop_x = 3;
	int bishop_y = 3;
	Piece bishop = (Piece) {.player = WHITE_PLAYER, .type = BISHOP};
	Position bishop_pos = (Position) {bishop_x, bishop_y};
	board_set_piece(board, bishop, bishop_pos);

	Piece pawn = (Piece) {.player = BLACK_PLAYER, .type = PAWN};
	// should be able to capture these
	Position pawn_pos = (Position) {bishop_x + 1, bishop_y + 1};
	board_set_piece(board, pawn, pawn_pos);
	pawn_pos = (Position) {bishop_x - 1, bishop_y - 1};
	board_set_piece(board, pawn, pawn_pos);
	pawn_pos = (Position) {bishop_x - 1, bishop_y + 1};
	board_set_piece(board, pawn, pawn_pos);
	pawn_pos = (Position) {bishop_x + 1, bishop_y - 1};
	board_set_piece(board, pawn, pawn_pos);

	// should not be able to capture these
	Position pawn_pos2 = (Position) {bishop_x + 2, bishop_y + 2};
	board_set_piece(board, pawn, pawn_pos2);
	pawn_pos2 = (Position) {bishop_x - 2, bishop_y - 2};
	board_set_piece(board, pawn, pawn_pos2);
	pawn_pos2 = (Position) {bishop_x - 2, bishop_y + 2};
	board_set_piece(board, pawn, pawn_pos2);
	pawn_pos2 = (Position) {bishop_x + 2, bishop_y - 2};
	board_set_piece(board, pawn, pawn_pos2);

	MoveList *moves = NULL;
	move_list_create(&moves);
	movegen_generate(board, bishop_pos, moves);

	size_t moves_size = move_list_size(moves);
	cr_assert_eq(moves_size, 4, "bishop should have 4 moves, but got %zu", moves_size);

	move_list_destroy(&moves);
}

Test(movegen, queen_can_move_in_cross_and_diag, .init = setup, .fini = teardown) {
	int queen_x = 3;
	int queen_y = 3;
	Piece queen = (Piece) {.player = WHITE_PLAYER, .type = QUEEN};
	Position queen_pos = (Position) {queen_x, queen_y};
	board_set_piece(board, queen, queen_pos);

	MoveList *moves = NULL;
	move_list_create(&moves);
	movegen_generate(board, queen_pos, moves);

	size_t moves_size = move_list_size(moves);
	cr_assert_eq(
		moves_size, 27, "queen should have 27 moves at x:%d y:%d, but got %zu", queen_x, queen_y, moves_size);

	move_list_destroy(&moves);
}

Test(movegen, queen_cant_skip_allies, .init = setup, .fini = teardown) {
	int queen_x = 3;
	int queen_y = 3;
	Piece queen = (Piece) {.player = WHITE_PLAYER, .type = QUEEN};
	Position queen_pos = (Position) {queen_x, queen_y};
	board_set_piece(board, queen, queen_pos);

	Piece pawn = (Piece) {.player = WHITE_PLAYER, .type = PAWN};
	Position pawn_pos = (Position) {queen_x + 1, queen_y + 1};
	board_set_piece(board, pawn, pawn_pos);
	pawn_pos = (Position) {queen_x - 1, queen_y - 1};
	board_set_piece(board, pawn, pawn_pos);
	pawn_pos = (Position) {queen_x - 1, queen_y + 1};
	board_set_piece(board, pawn, pawn_pos);
	pawn_pos = (Position) {queen_x + 1, queen_y - 1};
	board_set_piece(board, pawn, pawn_pos);

	Position pawn_pos2 = (Position) {queen_x, queen_y + 1};
	board_set_piece(board, pawn, pawn_pos2);
	pawn_pos2 = (Position) {queen_x, queen_y - 1};
	board_set_piece(board, pawn, pawn_pos2);
	pawn_pos2 = (Position) {queen_x + 1, queen_y};
	board_set_piece(board, pawn, pawn_pos2);
	pawn_pos2 = (Position) {queen_x - 1, queen_y};
	board_set_piece(board, pawn, pawn_pos2);

	MoveList *moves = NULL;
	move_list_create(&moves);
	movegen_generate(board, queen_pos, moves);

	size_t moves_size = move_list_size(moves);
	cr_assert_eq(moves_size, 0, "queen should have no moves, but got %zu", moves_size);

	move_list_destroy(&moves);
}

Test(movegen, queen_cant_skip_enemies, .init = setup, .fini = teardown) {
	int queen_x = 3;
	int queen_y = 3;
	Piece queen = (Piece) {.player = WHITE_PLAYER, .type = QUEEN};
	Position queen_pos = (Position) {queen_x, queen_y};
	board_set_piece(board, queen, queen_pos);

	Piece pawn = (Piece) {.player = BLACK_PLAYER, .type = PAWN};
	// should be able to capture these
	Position pawn_pos = (Position) {queen_x + 1, queen_y + 1};
	board_set_piece(board, pawn, pawn_pos);
	pawn_pos = (Position) {queen_x - 1, queen_y - 1};
	board_set_piece(board, pawn, pawn_pos);
	pawn_pos = (Position) {queen_x - 1, queen_y + 1};
	board_set_piece(board, pawn, pawn_pos);
	pawn_pos = (Position) {queen_x + 1, queen_y - 1};
	board_set_piece(board, pawn, pawn_pos);
	pawn_pos = (Position) {queen_x, queen_y + 1};
	board_set_piece(board, pawn, pawn_pos);
	pawn_pos = (Position) {queen_x, queen_y - 1};
	board_set_piece(board, pawn, pawn_pos);
	pawn_pos = (Position) {queen_x + 1, queen_y};
	board_set_piece(board, pawn, pawn_pos);
	pawn_pos = (Position) {queen_x - 1, queen_y};
	board_set_piece(board, pawn, pawn_pos);

	// should not be able to capture these
	Position pawn_pos2 = (Position) {queen_x + 2, queen_y + 2};
	board_set_piece(board, pawn, pawn_pos2);
	pawn_pos2 = (Position) {queen_x - 2, queen_y - 2};
	board_set_piece(board, pawn, pawn_pos2);
	pawn_pos2 = (Position) {queen_x - 2, queen_y + 2};
	board_set_piece(board, pawn, pawn_pos2);
	pawn_pos2 = (Position) {queen_x + 2, queen_y - 2};
	board_set_piece(board, pawn, pawn_pos2);
	pawn_pos2 = (Position) {queen_x + 2, queen_y};
	board_set_piece(board, pawn, pawn_pos2);
	pawn_pos2 = (Position) {queen_x - 2, queen_y};
	board_set_piece(board, pawn, pawn_pos2);
	pawn_pos2 = (Position) {queen_x, queen_y + 2};
	board_set_piece(board, pawn, pawn_pos2);
	pawn_pos2 = (Position) {queen_x, queen_y + 2};
	board_set_piece(board, pawn, pawn_pos2);

	MoveList *moves = NULL;
	move_list_create(&moves);
	movegen_generate(board, queen_pos, moves);

	size_t moves_size = move_list_size(moves);
	cr_assert_eq(moves_size, 8, "queen should have 8 moves, but got %zu", moves_size);

	move_list_destroy(&moves);
}

Test(movegen, king_can_move_in_cross_and_diag, .init = setup, .fini = teardown) {
	int king_x = 3;
	int king_y = 3;
	Piece king = (Piece) {.player = WHITE_PLAYER, .type = KING};
	Position king_pos = (Position) {king_x, king_y};
	board_set_piece(board, king, king_pos);

	MoveList *moves = NULL;
	move_list_create(&moves);
	movegen_generate(board, king_pos, moves);

	size_t moves_size = move_list_size(moves);
	cr_assert_eq(
		moves_size, 8, "king should have 8 moves at x:%d y:%d, but got %zu", king_x, king_y, moves_size);

	move_list_destroy(&moves);
}
