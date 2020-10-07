#include "Window.h"

void Window::init() {
	glfwInit();
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	window = glfwCreateWindow(extent.width, extent.height, "", nullptr, nullptr);
	glfwSetWindowUserPointer(window, this);
	glfwSetFramebufferSizeCallback(window, framebufferResizeCallback);
}

void Window::destroy() {
	glfwDestroyWindow(window);
	glfwTerminate();
}

WindowExtent Window::getExtent() {
	WindowExtent extent;
	glfwGetFramebufferSize(window, &extent.width, &extent.height);
	return extent;
}

std::vector<const char*> Window::instanceExtensions() {
	uint32_t extensionCount;
	const char** extensions = glfwGetRequiredInstanceExtensions(&extensionCount);
	std::vector<const char*> instanceExtensions (extensions, extensions + extensionCount);
	if (NEIGE_DEBUG) {
		instanceExtensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
	}

	return instanceExtensions;
}

void Window::createSurface(VkInstance instance, VkSurfaceKHR* surface) {
	NEIGE_VK_CHECK(glfwCreateWindowSurface(instance, window, nullptr, surface));
}

bool Window::windowGotResized() {
	glfwGetFramebufferSize(window, &extent.width, &extent.height);
	return true;
}

bool Window::windowGotClosed() {
	return glfwWindowShouldClose(window);
}

void Window::pollEvents() {
	glfwPollEvents();
}