#pragma once
#define GLFW_INCLUDE_VULKAN
#include "../../external/glfw/include/GLFW/glfw3.h"
#include "../utils/NeigeDefines.h"
#include "../utils/structs/RendererStructs.h"
#include "../inputs/KeyboardInputs.h"
#include "Surface.h"
#include <vector>

struct Window {
	// Callback when window get resized
	static void framebufferResizeCallback(GLFWwindow* window, int width, int height) {
		auto pointer = reinterpret_cast<Window*>(glfwGetWindowUserPointer(window));
		pointer->windowGotResized();
	}

	// Callback for keys
	static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
		auto pointer = reinterpret_cast<Window*>(glfwGetWindowUserPointer(window));
		pointer->keyAction(key, action);
	}

	GLFWwindow* window;
	Surface surface;
	VkExtent2D extent;
	bool gotResized = false;
	KeyboardInputs inputs;

	void init();
	void destroy();
	void updateExtent();
	std::vector<const char*> instanceExtensions();
	void createSurface();
	void windowGotResized();
	void keyAction(int key, int action);
	bool windowGotClosed();
	void pollEvents();
	void waitEvents();
};