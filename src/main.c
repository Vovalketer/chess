#include <stdio.h>
#include <stdlib.h>

#include "../include/engine.h"
#include "../include/ui.h"

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

MatchState* game;

int main(void) {
	bool created = engine_create_match(&game);
	if (!created) {
		printf("Error creating game\n");
		exit(EXIT_FAILURE);
	}
	init_rendering(WINDOW_WIDTH, WINDOW_HEIGHT);
	game_loop(game);
	stop_rendering();
	engine_destroy_match(&game);

	return EXIT_SUCCESS;
}
