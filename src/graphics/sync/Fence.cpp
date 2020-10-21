#include "Fence.h"
#include "../resources/RendererResources.h"

void Fence::init() {
	VkFenceCreateInfo fenceCreateInfo = {};
	fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fenceCreateInfo.pNext = nullptr;
	fenceCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;
	NEIGE_VK_CHECK(vkCreateFence(logicalDevice.device, &fenceCreateInfo, nullptr, &fence));
}

void Fence::destroy() {
	vkDestroyFence(logicalDevice.device, fence, nullptr);
}

void Fence::wait() {
	NEIGE_VK_CHECK(vkWaitForFences(logicalDevice.device, 1, &fence, VK_TRUE, std::numeric_limits<uint64_t>::max()));
}

void Fence::reset() {
	NEIGE_VK_CHECK(vkResetFences(logicalDevice.device, 1, &fence));
}