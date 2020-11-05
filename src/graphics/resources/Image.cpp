#include "Image.h"
#include "../../utils/resources/ImageTools.h"
#include "../resources/RendererResources.h"

void Image::destroy() {
	if (imageSampler != VK_NULL_HANDLE) {
		vkDestroySampler(logicalDevice.device, imageSampler, nullptr);
		imageSampler = VK_NULL_HANDLE;
	}
	if (imageView != VK_NULL_HANDLE) {
		vkDestroyImageView(logicalDevice.device, imageView, nullptr);
		imageView = VK_NULL_HANDLE;
	}
	if (image != VK_NULL_HANDLE) {
		memoryAllocator.deallocate(allocationId);
		vkDestroyImage(logicalDevice.device, image, nullptr);
		image = VK_NULL_HANDLE;
	}
}
