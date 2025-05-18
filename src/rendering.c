#include "rendering.h"

#include <raylib.h>
#include <stdio.h>
#include <stdlib.h>

#include "game.h"

int CURRENT_WINDOW_WIDTH;
int CURRENT_WINDOW_HEIGHT;

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

static void _draw_board(GameState state, int width, int height) {
	int tile_w_size = width / 8;
	int tile_h_size = height / 8;
	for (int i = 0; i < 8; i++) {
		for (int j = 0; j < 8; j++) {
			DrawRectangle(
				tile_w_size * j, tile_h_size * i, tile_w_size, tile_h_size, (i + j) % 2 == 0 ? WHITE : LIME);
			Piece piece = state.board[i][j];
			Texture2D piece_tex;
			switch (piece.type) {
				case PAWN:
					piece_tex = piece.color == WHITE_PLAYER ? pawn_w : pawn_b;
					break;
				case ROOK:
					piece_tex = piece.color == WHITE_PLAYER ? rook_w : rook_b;
					break;
				case KNIGHT:
					piece_tex = piece.color == WHITE_PLAYER ? knight_w : knight_b;
					break;
				case BISHOP:
					piece_tex = piece.color == WHITE_PLAYER ? bishop_w : bishop_b;
					break;
				case QUEEN:
					piece_tex = piece.color == WHITE_PLAYER ? queen_w : queen_b;
					break;
				case KING:
					piece_tex = piece.color == WHITE_PLAYER ? king_w : king_b;
					break;
				default:
					// dont draw if there's no piece
					continue;
					break;
			}

			int posY = (tile_h_size * i) + ((tile_h_size - piece_tex.height) / 2);
			int posX = (tile_w_size * j) + ((tile_w_size - piece_tex.width) / 2);
			DrawTexture(piece_tex, posX, posY, WHITE);
		}
	}
}

void game_loop(GameState state) {
	while (!WindowShouldClose()) {
		BeginDrawing();
		ClearBackground(RAYWHITE);
		_draw_board(state, CURRENT_WINDOW_WIDTH, CURRENT_WINDOW_HEIGHT);
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
