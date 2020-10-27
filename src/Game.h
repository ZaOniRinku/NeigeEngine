#pragma once
#include "ecs/ECS.h"
#include "ecs/components/Transform.h"
#include "ecs/components/Camera.h"
#include "ecs/components/Renderable.h"
#include "ecs/systems/Renderer.h"
#include "window/Window.h"

struct Game {
	Window* window;
	std::shared_ptr<Renderer> renderer;

	void init();
	void launch();
};
