#pragma once
#include "vulkan/vulkan.hpp"
#include "../NeigeDefines.h"
#include "../../graphics/commands/CommandBuffer.h"
#include "../../graphics/commands/CommandPool.h"

struct BufferTools {
	static void createBuffer(VkBuffer& buffer,
		VkDeviceSize size,
		VkBufferUsageFlags usage,
		VkMemoryPropertyFlags memoryProperties,
		VkDeviceSize* allocationId);
	static void createStagingBuffer(VkBuffer& buffer,
		VkDeviceMemory& deviceMemory,
		VkDeviceSize size);
	static void createUniformBuffer(VkBuffer& buffer,
		VkDeviceMemory& deviceMemory,
		VkDeviceSize size);
	static void copyBuffer(VkBuffer srcBuffer,
		VkBuffer dstBuffer,
		VkDeviceSize size);
	static void copyToImage(VkBuffer srcBuffer,
		VkImage dstImage,
		uint32_t width,
		uint32_t height,
		uint32_t arrayLayers);
};

