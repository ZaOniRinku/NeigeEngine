#pragma once
#include <vulkan/vulkan.hpp>
#include "../utils/NeigeTools.h"
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

	bool isSuitable(Surface surface);
	void findQueueFamilies(VkSurfaceKHR surface);
	bool extensionSupport();
	void getMaxUsableSampleCount();
	SwapchainSupport swapchainSupport(VkSurfaceKHR surface) const;
};