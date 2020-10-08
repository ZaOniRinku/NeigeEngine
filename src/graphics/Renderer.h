#pragma once
#include "vulkan/vulkan.hpp"
#include "../utils/NeigeDefines.h"
#include "../utils/NeigeStructs.h"
#include "../utils/RendererResources.h"
#include "../window/Window.h"
#include <iostream>
#include <vector>
#include <string>

struct Renderer {
	Window* window;

	void init();
	void destroy();
};