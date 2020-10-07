#pragma once
#include <vulkan/vulkan.hpp>
#include "NeigeTools.h"
#include "MemoryAllocator.h"
#include "PhysicalDevice.h"
#include "Window.h"
#include <iostream>
#include <vector>
#include <string>

// Debug messenger callback
static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
	VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
	void* pUserData) {
	std::cerr << "Validation layer: " << pCallbackData->pMessage << std::endl;

	return VK_FALSE;
}

struct Renderer {
	Window window;
	VkDebugUtilsMessengerEXT debugMessenger;
	MemoryAllocator memoryAllocator;
	Queues queues;
	VkInstance instance;
	VkSurfaceKHR surface;
	PhysicalDevice physicalDevice;
	VkDevice logicalDevice;
	VkSwapchainKHR swapchain;

	void init();
	void destroy();
	
	// Debug messenger
	void createDebugMessenger();
	void destroyDebugMessenger();
};