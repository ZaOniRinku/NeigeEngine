#include "Renderer.h"
#include "../utils/RendererResources.h"
#include "Shader.h"

void Renderer::init() {
	// Instance
	instance.init(VK_MAKE_VERSION(0, 0, 1), window->instanceExtensions());
	
	// Surface
	window->createSurface();

	// Pick physical device
	PhysicalDevicePicker::pick(window);

	// Logical device
	logicalDevice.init();

	// Swapchain
	swapchain.init(window);
}

void Renderer::update() {
	
}

void Renderer::destroy() {
	vkDeviceWaitIdle(logicalDevice.device);
	memoryAllocator.destroy();
	swapchain.destroy();
	window->surface.destroy();
	logicalDevice.destroy();
	instance.destroy();
}
