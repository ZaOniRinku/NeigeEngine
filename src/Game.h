#pragma once
#include "ecs/ECS.h"
#include "ecs/components/Transform.h"
#include "ecs/components/Camera.h"
#include "ecs/components/Light.h"
#include "ecs/components/Renderable.h"
#include "ecs/systems/Renderer.h"
#include "ecs/systems/Lighting.h"
#include "ecs/systems/CameraSystem.h"
#include "ecs/systems/CameraControls.h"
#include "window/Window.h"

struct Game {
	Window* window;
	std::shared_ptr<Renderer> renderer;
	std::shared_ptr<Lighting> lighting;
	std::shared_ptr<CameraSystem> cameraSystem;
	std::shared_ptr<CameraControls> cameraControls;
	double lastFrame = 0.0;

	void init();
	void launch();
};
