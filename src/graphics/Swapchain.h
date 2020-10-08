#pragma once
#include "vulkan/vulkan.hpp"
#include "../utils/NeigeStructs.h"
#include "../window/Window.h"

struct Swapchain {
	VkSwapchainKHR swapchain = VK_NULL_HANDLE;
	SwapchainSupport swapchainSupport;
	VkExtent2D extent;
	VkSurfaceFormatKHR surfaceFormat;
	VkPresentModeKHR presentMode;
	uint32_t imageNumber;
	std::vector<VkImage> images;
	std::vector<VkImageView> imageViews;

	void init(const Window* window);
	void destroy();
};