#include "Game.h"

extern ECS ecs;

void Game::init() {
	// Register components
	ecs.registerComponent<Transform>();
	ecs.registerComponent<Camera>();
	ecs.registerComponent<Renderable>();
	ecs.registerComponent<Light>();

	// Register systems
	renderer = ecs.registerSystem<Renderer>();
	ComponentMask rendererMask;
	rendererMask.set(ecs.getComponentId<Renderable>());
	rendererMask.set(ecs.getComponentId<Transform>());
	ecs.setSystemComponents<Renderer>(rendererMask);
	renderer->window = window;

	lighting = ecs.registerSystem<Lighting>();
	ComponentMask lightingMask;
	lightingMask.set(ecs.getComponentId<Light>());
	ecs.setSystemComponents<Lighting>(lightingMask);

	cameraControls = ecs.registerSystem<CameraControls>();
	ComponentMask cameraControlsMask;
	cameraControlsMask.set(ecs.getComponentId<Camera>());
	ecs.setSystemComponents<CameraControls>(cameraControlsMask);
}

void Game::launch() {
	window->init();
	lighting->init();
	renderer->init();

	while (!window->windowGotClosed()) {
		window->pollEvents();

		double currentTime = glfwGetTime();
		double deltaTime = currentTime - lastFrame;

		cameraControls->update(deltaTime);

		lighting->update();

		renderer->update();

		lastFrame = currentTime;
	}

	renderer->destroy();
	window->destroy();
}