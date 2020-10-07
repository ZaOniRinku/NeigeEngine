#pragma once
#define GLFW_INCLUDE_VULKAN
#include "../external/glfw/include/GLFW/glfw3.h"
#include "NeigeTools.h"
#include <vector>

struct Window {
	// Callback when window get resized
	static void framebufferResizeCallback(GLFWwindow* window, int width, int height) {
		auto pointer = reinterpret_cast<Window*>(glfwGetWindowUserPointer(window));
		pointer->windowGotResized();
	}

	GLFWwindow* window;
	WindowExtent extent = {};

	void init();
	void destroy();
	WindowExtent getExtent();
	std::vector<const char*> instanceExtensions();
	void createSurface(VkInstance instance, VkSurfaceKHR* surface);
	bool windowGotResized();
	bool windowGotClosed();
	void pollEvents();
};