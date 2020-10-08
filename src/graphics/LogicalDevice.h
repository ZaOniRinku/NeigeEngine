#pragma once
#include "vulkan/vulkan.hpp"
#include "../utils/NeigeStructs.h"
#include "../utils/NeigeTools.h"
#include "PhysicalDevice.h"
#include <set>

struct LogicalDevice {
	VkDevice device;
	Queues queues;

	void init(const PhysicalDevice* physicalDevice);
	void destroy();
};
