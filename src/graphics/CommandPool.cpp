#include "CommandPool.h"
#include "../utils/RendererResources.h"

void CommandPool::init() {
	VkCommandPoolCreateInfo commandPoolCreateInfo = {};
	commandPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	commandPoolCreateInfo.pNext = nullptr;
	commandPoolCreateInfo.flags = 0;
	commandPoolCreateInfo.queueFamilyIndex = physicalDevice.queueFamilyIndices.graphicsFamily.value();
	NEIGE_VK_CHECK(vkCreateCommandPool(logicalDevice.device, &commandPoolCreateInfo, nullptr, &commandPool));
}

void CommandPool::destroy() {
	vkDestroyCommandPool(logicalDevice.device, commandPool, nullptr);
}

void CommandPool::reset() {
	NEIGE_VK_CHECK(vkResetCommandPool(logicalDevice.device, commandPool, 0));
}