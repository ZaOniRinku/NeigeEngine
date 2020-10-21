#pragma once
#include "vulkan/vulkan.hpp"
#include "../../utils/NeigeDefines.h"
#include "CommandPool.h"

struct CommandBuffer {
	VkCommandBuffer commandBuffer = VK_NULL_HANDLE;

	void init(CommandPool* commandPool);
	void begin();
	void end();
	void endAndSubmit();
};

