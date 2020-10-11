#pragma once
#include "vulkan/vulkan.hpp"
#include "NeigeDefines.h"
#include "../graphics/CommandBuffer.h"
#include "../graphics/CommandPool.h"

struct BufferTools {
	static void createBuffer(VkBuffer* buffer,
		VkDeviceSize size,
		VkBufferUsageFlags usage,
		VkMemoryPropertyFlags memoryProperties);
	static void createStagingBuffer(VkBuffer& buffer,
		VkDeviceMemory& deviceMemory,
		VkDeviceSize size,
		VkBufferUsageFlags usage,
		VkMemoryPropertyFlags memoryProperties);
	static void copyToImage(VkBuffer buffer,
		VkImage image,
		uint32_t width,
		uint32_t height,
		uint32_t arrayLayers);
};

