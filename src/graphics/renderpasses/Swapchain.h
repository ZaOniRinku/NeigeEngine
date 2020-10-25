#pragma once
#include "vulkan/vulkan.hpp"
#include "../../utils/structs/RendererStructs.h"
#include "../../window/Window.h"
#include "../sync/Semaphore.h"

struct Swapchain {
	VkSwapchainKHR swapchain = VK_NULL_HANDLE;
	SwapchainSupport swapchainSupport;
	VkExtent2D extent;
	VkSurfaceFormatKHR surfaceFormat;
	VkPresentModeKHR presentMode;
	uint32_t imageNumber;
	std::vector<VkImage> images;
	std::vector<VkImageView> imageViews;

	void init(const Window* window, uint32_t* swapchainSize);
	void destroy();
	VkResult acquireNextImage(Semaphore* imageAvailableSemaphore, uint32_t* index);
};