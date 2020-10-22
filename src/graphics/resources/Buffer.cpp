#include "Buffer.h"
#include "../resources/RendererResources.h"

void Buffer::destroy() {
	if (deviceMemory != VK_NULL_HANDLE) {
		vkFreeMemory(logicalDevice.device, deviceMemory, nullptr);
	}
	else {
		memoryAllocator.deallocate(allocationId);
	}
	vkDestroyBuffer(logicalDevice.device, buffer, nullptr);
}

void Buffer::map(VkDeviceSize offset, VkDeviceSize size, void** data) {
	vkMapMemory(logicalDevice.device, deviceMemory, offset, size, 0, data);
}

void Buffer::unmap() {
	vkUnmapMemory(logicalDevice.device, deviceMemory);
}
