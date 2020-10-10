#pragma once
#include "graphics\Renderer.h"
#include "window\Window.h"

struct Game {
	Window* window;
	Renderer* renderer;

	void init();
	void update();
	void destroy();
};

