#include "CommandBuffer.h"
#include "../resources/RendererResources.h"

void CommandBuffer::init(CommandPool* commandPool) {
	VkCommandBufferAllocateInfo commandBufferAllocateInfo = {};
	commandBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	commandBufferAllocateInfo.pNext = nullptr;
	commandBufferAllocateInfo.commandPool = commandPool->commandPool;
	commandBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	commandBufferAllocateInfo.commandBufferCount = 1;
	NEIGE_VK_CHECK(vkAllocateCommandBuffers(logicalDevice.device, &commandBufferAllocateInfo, &commandBuffer));
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

void CommandBuffer::endAndSubmit() {
	NEIGE_VK_CHECK(vkEndCommandBuffer(commandBuffer));
	VkSubmitInfo submitInfo = {};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.pNext = nullptr;
	submitInfo.waitSemaphoreCount = 0;
	submitInfo.pSignalSemaphores = nullptr;
	submitInfo.pWaitDstStageMask = nullptr;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &commandBuffer;
	submitInfo.signalSemaphoreCount = 0;
	submitInfo.pSignalSemaphores = nullptr;
	NEIGE_VK_CHECK(vkQueueSubmit(logicalDevice.queues.graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE));
	NEIGE_VK_CHECK(vkQueueWaitIdle(logicalDevice.queues.graphicsQueue));
}