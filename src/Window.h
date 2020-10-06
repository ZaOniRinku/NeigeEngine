#pragma once
#include "../external/glfw/include/GLFW/glfw3.h"
#include "NeigeTools.h"
#include <vector>

struct WindowExtent {
	int width;
	int height;
};

struct Window {
	GLFWwindow* window;
	WindowExtent extent = {};

	void init();
	void destroy();
	std::vector<const char*> instanceExtensions();
};