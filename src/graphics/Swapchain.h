#pragma once
#include "vulkan/vulkan.hpp"
#include "../utils/NeigeStructs.h"
#include "../window/Window.h"
#include "LogicalDevice.h"
#include "PhysicalDevice.h"

struct Swapchain {
	VkSwapchainKHR swapchain = VK_NULL_HANDLE;
	SwapchainSupport swapchainSupport;
	VkExtent2D extent;
	VkSurfaceFormatKHR surfaceFormat;
	VkPresentModeKHR presentMode;
	std::vector<VkImage> images;
	std::vector<VkImageView> imageViews;

	void init(const PhysicalDevice* physicalDevice, const LogicalDevice* logicalDevice, const Window* window);
	void destroy(const LogicalDevice* logicalDevice);
};