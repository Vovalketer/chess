#ifndef RENDERING_H
#define RENDERING_H

#include "game.h"

void init_rendering(int screen_width, int screen_height);
void game_loop(MatchState *state);
void stop_rendering(void);
#endif
