#pragma once
#include "vulkan/vulkan.hpp"

struct Surface {
	VkSurfaceKHR surface;

	void destroy();
};