#pragma once
#include "vulkan/vulkan.hpp"

struct Surface {
	VkSurfaceKHR surface = VK_NULL_HANDLE;

	void destroy();
};