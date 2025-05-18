#ifndef RENDERING_H
#define RENDERING_H

#include "game.h"

void init_rendering(GameState state, int screen_width, int screen_height);
void draw_board(GameState state, int width, int height);
void stop_rendering(void);
#endif
