#include "Game.h"

void Game::init() {
	window->init();
	renderer->window = window;
	renderer->init();
}

void Game::update() {
	while (!window->windowGotClosed()) {
		window->pollEvents();

		renderer->update();
	}
}

void Game::destroy() {
	renderer->destroy();
	window->destroy();
}
