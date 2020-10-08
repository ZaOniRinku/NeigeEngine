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

void Window::updateExtent() {
	int width;
	int height;
	glfwGetFramebufferSize(window, &width, &height);
	extent = {
		static_cast<uint32_t>(width),
		static_cast<uint32_t>(height)
	};
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

void Window::createSurface(const Instance* instance) {
	NEIGE_VK_CHECK(glfwCreateWindowSurface(instance->instance, window, nullptr, &surface.surface));
}

bool Window::windowGotResized() {
	updateExtent();
	return true;
}

bool Window::windowGotClosed() {
	return glfwWindowShouldClose(window);
}

void Window::pollEvents() {
	glfwPollEvents();
}