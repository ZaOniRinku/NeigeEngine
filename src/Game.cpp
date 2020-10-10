#include "Game.h"

void Game::launch() {
	window->init();
	renderer.window = window;
	renderer.init();

	while (!window->windowGotClosed()) {
		window->pollEvents();

		renderer.update();
	}

	renderer.destroy();
	window->destroy();
}