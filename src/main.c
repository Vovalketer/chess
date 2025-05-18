#include <stdlib.h>

#include "rendering.h"

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

GameState game;

int main(void) {
	init_board(&game);
	init_rendering(game, WINDOW_WIDTH, WINDOW_HEIGHT);
	stop_rendering();
	return EXIT_SUCCESS;
}
