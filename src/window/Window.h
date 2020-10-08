#pragma once
#define GLFW_INCLUDE_VULKAN
#include "../../external/glfw/include/GLFW/glfw3.h"
#include "../utils/NeigeTools.h"
#include "../utils/NeigeStructs.h"
#include "Surface.h"
#include "../graphics/Instance.h"
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

	void init();
	void destroy();
	void updateExtent();
	std::vector<const char*> instanceExtensions();
	void createSurface(const Instance* instance);
	bool windowGotResized();
	bool windowGotClosed();
	void pollEvents();
};