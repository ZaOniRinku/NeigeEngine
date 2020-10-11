#include "Buffer.h"
#include "../utils/RendererResources.h"

void Buffer::destroy() {
	vkDestroyBuffer(logicalDevice.device, buffer, nullptr);
	vkFreeMemory(logicalDevice.device, deviceMemory, nullptr);
}

void Buffer::map(VkDeviceSize offset, VkDeviceSize size, void** data) {
	vkMapMemory(logicalDevice.device, deviceMemory, offset, size, 0, data);
}

void Buffer::unmap() {
	vkUnmapMemory(logicalDevice.device, deviceMemory);
}
