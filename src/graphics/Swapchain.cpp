#include "Swapchain.h"

void Swapchain::init(const PhysicalDevice* physicalDevice, const LogicalDevice* logicalDevice, const Window* window) {
	swapchainSupport = physicalDevice->swapchainSupport(window->surface.surface);
	extent = swapchainSupport.extent(window->extent);
	surfaceFormat = swapchainSupport.surfaceFormat();
	presentMode = swapchainSupport.presentMode();
	uint32_t minImageCount = swapchainSupport.capabilities.minImageCount + 1;
	if (swapchainSupport.capabilities.maxImageCount > 0 && minImageCount > swapchainSupport.capabilities.maxImageCount) {
		minImageCount = swapchainSupport.capabilities.maxImageCount;
	}
	VkSwapchainCreateInfoKHR swapchainCreateInfo = {};
	swapchainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	swapchainCreateInfo.pNext = nullptr;
	swapchainCreateInfo.flags = 0;
	swapchainCreateInfo.surface = window->surface.surface;
	swapchainCreateInfo.minImageCount = minImageCount;
	swapchainCreateInfo.imageFormat = surfaceFormat.format;
	swapchainCreateInfo.imageColorSpace = surfaceFormat.colorSpace;
	swapchainCreateInfo.imageExtent = extent;
	swapchainCreateInfo.imageArrayLayers = 1;
	swapchainCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
	if (physicalDevice->queueFamilyIndices.graphicsFamily.value() != physicalDevice->queueFamilyIndices.presentFamily.value()) {
		swapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
		swapchainCreateInfo.queueFamilyIndexCount = 2;
		std::array<uint32_t, 2> swapchainQueueFamilies = { physicalDevice->queueFamilyIndices.graphicsFamily.value(), physicalDevice->queueFamilyIndices.presentFamily.value() };
		swapchainCreateInfo.pQueueFamilyIndices = swapchainQueueFamilies.data();
	}
	else {
		swapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		swapchainCreateInfo.queueFamilyIndexCount = 0;
		swapchainCreateInfo.pQueueFamilyIndices = nullptr;
	}
	swapchainCreateInfo.preTransform = swapchainSupport.capabilities.currentTransform;
	swapchainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	swapchainCreateInfo.presentMode = presentMode;
	swapchainCreateInfo.clipped = VK_TRUE;
	swapchainCreateInfo.oldSwapchain = VK_NULL_HANDLE;
	NEIGE_VK_CHECK(vkCreateSwapchainKHR(logicalDevice->device, &swapchainCreateInfo, nullptr, &swapchain));
}

void Swapchain::destroy(const LogicalDevice* logicalDevice) {
	vkDestroySwapchainKHR(logicalDevice->device, swapchain, nullptr);
}