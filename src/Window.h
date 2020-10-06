#pragma once
#define GLFW_INCLUDE_VULKAN
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
	void createSurface(VkInstance* instance, VkSurfaceKHR* surface);
	bool windowGotClosed();
	void pollEvents();
};