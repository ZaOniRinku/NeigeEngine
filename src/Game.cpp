#include "Game.h"

extern ECS ecs;

void Game::init() {
	// Register components
	ecs.registerComponent<Transform>();
	ecs.registerComponent<Camera>();
	ecs.registerComponent<Renderable>();

	// Register systems
	renderer = ecs.registerSystem<Renderer>();
	ComponentMask rendererMask;
	rendererMask.set(ecs.getComponentId<Renderable>());
	rendererMask.set(ecs.getComponentId<Transform>());
	ecs.setSystemComponents<Renderer>(rendererMask);
	renderer->window = window;
}

void Game::launch() {
	window->init();
	renderer->init();

	while (!window->windowGotClosed()) {
		window->pollEvents();

		renderer->update();
	}

	renderer->destroy();
	window->destroy();
}