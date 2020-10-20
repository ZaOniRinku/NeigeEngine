#pragma once
#include "vulkan/vulkan.hpp"
#include "../utils/NeigeDefines.h"

struct Fence {
	VkFence fence;

	void init();
	void destroy();
	void wait();
	void reset();
};

