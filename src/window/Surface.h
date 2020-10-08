#pragma once
#include "vulkan/vulkan.hpp"
#include "../graphics/Instance.h"

struct Surface {
	VkSurfaceKHR surface;

	void destroy(Instance* instance);
};