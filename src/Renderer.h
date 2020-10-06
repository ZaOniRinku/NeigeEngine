#pragma once
#include <vulkan/vulkan.hpp>
#include "NeigeTools.h"
#include "Window.h"
#include <iostream>
#include <vector>

struct Renderer {
	Window window;
	VkInstance instance;

	void init();
	void destroy();
};