#include <stdlib.h>

#include "game.h"
#include "rendering.h"

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

MatchState* game;

int main(void) {
	game = create_game();
	init_rendering(WINDOW_WIDTH, WINDOW_HEIGHT);
	game_loop(game);
	stop_rendering();
	destroy_game(game);

	return EXIT_SUCCESS;
}
