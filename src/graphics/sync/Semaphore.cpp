#include "Semaphore.h"
#include "../resources/RendererResources.h"

void Semaphore::init() {
	VkSemaphoreCreateInfo semaphoreCreateInfo = {};
	semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
	semaphoreCreateInfo.pNext = nullptr;
	semaphoreCreateInfo.flags = 0;
	NEIGE_VK_CHECK(vkCreateSemaphore(logicalDevice.device, &semaphoreCreateInfo, nullptr, &semaphore));
}

void Semaphore::destroy() {
	vkDestroySemaphore(logicalDevice.device, semaphore, nullptr);
}
