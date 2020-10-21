#pragma once
#include "vulkan/vulkan.hpp"

struct Semaphore {
	VkSemaphore semaphore;

	void init();
	void destroy();
};
