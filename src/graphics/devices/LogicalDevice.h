#pragma once
#include "vulkan/vulkan.hpp"
#include "../../utils/NeigeDefines.h"
#include "../../utils/structs/RendererStructs.h"
#include <set>

struct LogicalDevice {
	VkDevice device = VK_NULL_HANDLE;
	Queues queues;

	void init();
	void destroy();
	void wait();
};