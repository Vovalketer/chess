#include "../include/ui.h"

#include <raylib.h>
#include <stdio.h>
#include <stdlib.h>

#include "../include/engine.h"

int CURRENT_WINDOW_WIDTH;
int CURRENT_WINDOW_HEIGHT;

int DRAWN_TILE_W_SIZE;
int DRAWN_TILE_H_SIZE;

Texture2D pawn_w;
Texture2D pawn_b;
Texture2D rook_w;
Texture2D rook_b;
Texture2D knight_w;
Texture2D knight_b;
Texture2D bishop_w;
Texture2D bishop_b;
Texture2D queen_w;
Texture2D queen_b;
Texture2D king_w;
Texture2D king_b;

static Texture2D _load_texture_or_exit(const char* tex_path) {
	Texture2D tex = LoadTexture(tex_path);
	if (tex.id == 0) {
		printf("Error loading texture: %s\n", tex_path);
		exit(EXIT_FAILURE);
	}
	return tex;
}

static void _load_textures(void) {
	pawn_w = _load_texture_or_exit("resources/Chess_plt60.png");
	pawn_b = _load_texture_or_exit("resources/Chess_pdt60.png");
	rook_w = _load_texture_or_exit("resources/Chess_rlt60.png");
	rook_b = _load_texture_or_exit("resources/Chess_rdt60.png");
	knight_w = _load_texture_or_exit("resources/Chess_nlt60.png");
	knight_b = _load_texture_or_exit("resources/Chess_ndt60.png");
	bishop_w = _load_texture_or_exit("resources/Chess_blt60.png");
	bishop_b = _load_texture_or_exit("resources/Chess_bdt60.png");
	queen_w = _load_texture_or_exit("resources/Chess_qlt60.png");
	queen_b = _load_texture_or_exit("resources/Chess_qdt60.png");
	king_w = _load_texture_or_exit("resources/Chess_klt60.png");
	king_b = _load_texture_or_exit("resources/Chess_kdt60.png");
}

void init_rendering(int window_width, int window_height) {
	CURRENT_WINDOW_WIDTH = window_width;
	CURRENT_WINDOW_HEIGHT = window_height;
	InitWindow(CURRENT_WINDOW_WIDTH, CURRENT_WINDOW_HEIGHT, "Chess");
	SetTargetFPS(30);

	_load_textures();
}

static void _draw_board(GameState* state, int width, int height) {
	DRAWN_TILE_W_SIZE = width / 8;
	DRAWN_TILE_H_SIZE = height / 8;
	for (int row = 0; row < 8; row++) {
		for (int col = 0; col < 8; col++) {
			DrawRectangle(DRAWN_TILE_W_SIZE * col,
						  DRAWN_TILE_H_SIZE * row,
						  DRAWN_TILE_W_SIZE,
						  DRAWN_TILE_H_SIZE,
						  (row + col) % 2 == 0 ? WHITE : LIME);
			Position pos = (Position) {col, row};
			Piece piece = engine_get_piece(state, pos);
			Texture2D piece_tex;
			switch (piece.type) {
				case PAWN:
					piece_tex = piece.player == WHITE_PLAYER ? pawn_w : pawn_b;
					break;
				case ROOK:
					piece_tex = piece.player == WHITE_PLAYER ? rook_w : rook_b;
					break;
				case KNIGHT:
					piece_tex = piece.player == WHITE_PLAYER ? knight_w : knight_b;
					break;
				case BISHOP:
					piece_tex = piece.player == WHITE_PLAYER ? bishop_w : bishop_b;
					break;
				case QUEEN:
					piece_tex = piece.player == WHITE_PLAYER ? queen_w : queen_b;
					break;
				case KING:
					piece_tex = piece.player == WHITE_PLAYER ? king_w : king_b;
					break;
				default:
					// dont draw if there's no piece
					continue;
					break;
			}

			int posY = (DRAWN_TILE_H_SIZE * row) + ((DRAWN_TILE_H_SIZE - piece_tex.height) / 2);
			int posX = (DRAWN_TILE_W_SIZE * col) + ((DRAWN_TILE_W_SIZE - piece_tex.width) / 2);
			DrawTexture(piece_tex, posX, posY, WHITE);
		}
	}
}

static void _draw_valid_moves(MoveMask mm) {
	const int circle_radius = 14;
	for (int row = 0; row < 8; row++) {
		for (int col = 0; col < 8; col++) {
			if (mm.mask[row][col]) {
				DrawCircle((DRAWN_TILE_W_SIZE * col) + ((DRAWN_TILE_W_SIZE + (circle_radius / 2)) / 2),
						   (DRAWN_TILE_H_SIZE * row) + ((DRAWN_TILE_H_SIZE + (circle_radius / 2)) / 2),
						   circle_radius,
						   RED);
			}
		}
	}
}

void game_loop(GameState* state) {
	int tile_w_size = CURRENT_WINDOW_WIDTH / 8;
	int tile_h_size = CURRENT_WINDOW_HEIGHT / 8;
	Vector2 mouse_position = {0, 0};
	bool selected = false;
	Position selected_pos = (Position) {-1, -1};
	MoveMask mm = {0};
	while (!WindowShouldClose()) {
		BeginDrawing();
		ClearBackground(RAYWHITE);
		_draw_board(state, CURRENT_WINDOW_WIDTH, CURRENT_WINDOW_HEIGHT);
		if (selected) {
			_draw_valid_moves(mm);
		}

		if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
			mouse_position = GetMousePosition();
			int x = mouse_position.x / tile_w_size;
			int y = mouse_position.y / tile_h_size;
			if (x >= 0 && x < 8 && y >= 0 && y < 8) {
				Position pos = (Position) {x, y};
				Piece piece = engine_get_piece(state, pos);
				if (selected) {
					if (selected_pos.x == x && selected_pos.y == y) {
						selected = false;
						selected_pos = (Position) {-1, -1};
					} else {
						engine_move_piece(state, selected_pos, pos);
						selected = false;
						selected_pos = (Position) {-1, -1};
					}
				} else if (!selected && piece.player != NONE) {
					selected_pos = pos;
					mm = engine_get_valid_moves(state, selected_pos);
					selected = true;
				}
			}
		}

		if (IsKeyPressed(KEY_U)) {
			engine_undo_move(state);
		}

		EndDrawing();
	}
}

void stop_rendering(void) {
	UnloadTexture(pawn_w);
	UnloadTexture(pawn_b);
	UnloadTexture(rook_w);
	UnloadTexture(rook_b);
	UnloadTexture(knight_w);
	UnloadTexture(knight_b);
	UnloadTexture(bishop_w);
	UnloadTexture(bishop_b);
	UnloadTexture(queen_w);
	UnloadTexture(queen_b);
	UnloadTexture(king_w);
	UnloadTexture(king_b);
	CloseWindow();
}
