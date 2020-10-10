#include "CommandBuffer.h"
#include "../utils/RendererResources.h"

void CommandBuffer::init(CommandPool* commandPool) {
	VkCommandBufferAllocateInfo commandBufferAllocateInfo = {};
	commandBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	commandBufferAllocateInfo.pNext = nullptr;
	commandBufferAllocateInfo.commandPool = commandPool->commandPool;
	commandBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	commandBufferAllocateInfo.commandBufferCount = 1;
	NEIGE_VK_CHECK(vkAllocateCommandBuffers(logicalDevice.device, nullptr, &commandBuffer));
}

void CommandBuffer::begin() {
	VkCommandBufferBeginInfo commandBufferBeginInfo = {};
	commandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	commandBufferBeginInfo.pNext = nullptr;
	commandBufferBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
	NEIGE_VK_CHECK(vkBeginCommandBuffer(commandBuffer, &commandBufferBeginInfo));
}

void CommandBuffer::end() {
	NEIGE_VK_CHECK(vkEndCommandBuffer(commandBuffer));
}