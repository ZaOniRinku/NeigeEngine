#pragma once
#define GLFW_INCLUDE_VULKAN
#include "../../external/glfw/include/GLFW/glfw3.h"
#include "../utils/NeigeDefines.h"
#include "../utils/NeigeStructs.h"
#include "Surface.h"
#include <vector>

struct Window {
	// Callback when window get resized
	static void framebufferResizeCallback(GLFWwindow* window, int width, int height) {
		auto pointer = reinterpret_cast<Window*>(glfwGetWindowUserPointer(window));
		pointer->windowGotResized();
	}

	GLFWwindow* window;
	Surface surface;
	VkExtent2D extent;
	bool gotResized = false;

	void init();
	void destroy();
	void updateExtent();
	std::vector<const char*> instanceExtensions();
	void createSurface();
	void windowGotResized();
	bool windowGotClosed();
	void pollEvents();
	void waitEvents();
};