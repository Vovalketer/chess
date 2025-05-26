#include <stdio.h>
#include <stdlib.h>

#include "../include/engine.h"
#include "../include/ui.h"

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

BoardState* game;

int main(void) {
	bool created = create_standard_match(&game);
	if (!created) {
		printf("Error creating game\n");
		exit(EXIT_FAILURE);
	}
	init_rendering(WINDOW_WIDTH, WINDOW_HEIGHT);
	game_loop(game);
	stop_rendering();
	destroy_game(&game);

	return EXIT_SUCCESS;
}
