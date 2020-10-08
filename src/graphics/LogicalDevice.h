#pragma once
#include "vulkan/vulkan.hpp"
#include "../utils/NeigeDefines.h"
#include "../utils/NeigeStructs.h"
#include <set>

struct LogicalDevice {
	VkDevice device;
	Queues queues;

	void init();
	void destroy();
};