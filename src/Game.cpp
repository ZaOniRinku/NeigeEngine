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

	ComponentMask cameraMask;
	cameraMask.set(ecs.getComponentId<Camera>());

	cameraSystem = ecs.registerSystem<CameraSystem>();
	ecs.setSystemComponents<CameraSystem>(cameraMask);

	cameraControls = ecs.registerSystem<CameraControls>();
	ecs.setSystemComponents<CameraControls>(cameraMask);
}

void Game::launch() {
	window->init();
	cameraSystem->init();
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