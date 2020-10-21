#include "Image.h"
#include "../../utils/resources/ImageTools.h"
#include "../resources/RendererResources.h"

void Image::init(uint32_t arrayLayers,
	uint32_t imageWidth,
	uint32_t imageHeight,
	uint32_t mipLevels,
	VkSampleCountFlagBits msaaSamples,
	VkFormat format,
	VkImageUsageFlags imageUsage,
	VkFilter filter,
	VkImageAspectFlags imageViewAspectFlags,
	VkSamplerAddressMode imageViewAddressMode,
	VkMemoryPropertyFlags imageMemoryProperties) {
	allocationId = ImageTools::createImage(&image, arrayLayers, imageWidth, imageHeight, mipLevels, msaaSamples, format, imageUsage, imageMemoryProperties);
	VkImageViewType imageViewType = arrayLayers == 6 ? VK_IMAGE_VIEW_TYPE_CUBE : VK_IMAGE_VIEW_TYPE_2D;
	ImageTools::createImageView(&imageView, image, arrayLayers, mipLevels, imageViewType, format, imageViewAspectFlags);
	ImageTools::createImageSampler(&imageSampler, mipLevels, filter, imageViewAddressMode);
	width = imageWidth;
	height = imageHeight;
	mipmapLevels = mipLevels;
}

void Image::destroy() {
	if (imageSampler != VK_NULL_HANDLE) {
		vkDestroySampler(logicalDevice.device, imageSampler, nullptr);
	}
	if (imageView != VK_NULL_HANDLE) {
		vkDestroyImageView(logicalDevice.device, imageView, nullptr);
	}
	if (image != VK_NULL_HANDLE) {
		memoryAllocator.deallocate(allocationId);
		vkDestroyImage(logicalDevice.device, image, nullptr);
	}
}
