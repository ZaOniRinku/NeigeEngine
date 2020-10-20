#pragma once
#include "vulkan/vulkan.hpp"
#include "../utils/NeigeDefines.h"
#include "../utils/NeigeStructs.h"
#include "../window/Surface.h"
#include <set>
#include <string>

struct PhysicalDevice {
	VkPhysicalDevice device = VK_NULL_HANDLE;
	VkPhysicalDeviceProperties properties;
	VkPhysicalDeviceFeatures features;
	VkPhysicalDeviceMemoryProperties memoryProperties;
	QueueFamilyIndices queueFamilyIndices;
	VkSampleCountFlagBits maxUsableSampleCount;
	VkFormat colorFormat = VK_FORMAT_UNDEFINED;
	VkFormat depthFormat = VK_FORMAT_UNDEFINED;

	bool isSuitable(const Surface* surface);
	void findQueueFamilies(VkSurfaceKHR surface);
	bool extensionSupport();
	void getMaxUsableSampleCount();
	SwapchainSupport swapchainSupport(VkSurfaceKHR surface);
	void findColorFormat();
	void findDepthFormat();
};