#ifndef UI_H
#define UI_H

#include "types.h"

void init_rendering(int screen_width, int screen_height);
void game_loop(MatchState *state);
void stop_rendering(void);
#endif
