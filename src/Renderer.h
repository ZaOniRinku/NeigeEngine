#pragma once
#include <vulkan/vulkan.hpp>
#include "NeigeTools.h"
#include "Window.h"
#include <iostream>
#include <vector>
#include <optional>
#include <set>
#include <string>

// Layers
const std::vector<const char*> layers = {
	"VK_LAYER_KHRONOS_validation"
};

// Extensions
const std::vector<const char*> extensions = {
	VK_KHR_SWAPCHAIN_EXTENSION_NAME
};

// Debug messenger callback
static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
	VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
	void* pUserData) {
	std::cerr << "Validation layer: " << pCallbackData->pMessage << std::endl;

	return VK_FALSE;
}

// Queue families
struct QueueFamiliesIndices {
	std::optional<uint32_t> graphicsFamily;
	std::optional<uint32_t> computeFamily;
	std::optional<uint32_t> presentFamily;

	bool isComplete() {
		return graphicsFamily.has_value() && computeFamily.has_value() && presentFamily.has_value();
	}
};

// Swapchain support
struct SwapchainSupport {
	VkSurfaceCapabilitiesKHR capabilities;
	std::vector<VkSurfaceFormatKHR> formats;
	std::vector<VkPresentModeKHR> presentMode;
};

struct Renderer {
	Window window;
	VkInstance instance;
	VkDebugUtilsMessengerEXT debugMessenger;
	VkSurfaceKHR surface;
	VkPhysicalDevice physicalDevice;
	VkDevice logicalDevice;

	void init();
	void destroy();
	
	// Debug messenger
	void createDebugMessenger();

	// Physical device choice
	bool physicalDeviceSuitable(VkPhysicalDevice physicalDevice);
	QueueFamiliesIndices physicalDeviceFindQueueFamilies(VkPhysicalDevice physicalDevice);
	bool physicalDeviceExtensionSupport(VkPhysicalDevice physicalDevice);
	SwapchainSupport physicalDeviceSwapchainSupport(VkPhysicalDevice physicalDevice);
};