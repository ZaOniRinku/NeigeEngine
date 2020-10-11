#pragma once
#include "vulkan/vulkan.hpp"

struct Buffer {
	VkBuffer buffer = VK_NULL_HANDLE;
	VkDeviceMemory deviceMemory = VK_NULL_HANDLE;
	VkDeviceSize size;

	void destroy();
	void map(VkDeviceSize offset, VkDeviceSize size, void** data);
	void unmap();
};
