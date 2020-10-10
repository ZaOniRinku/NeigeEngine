#include "Image.h"
#include "../utils/ImageTools.h"
#include "../utils/RendererResources.h"

void Image::init(VkImageType imageType,
	VkImageViewType imageViewType,
	uint32_t arrayLayers,
	uint32_t imageWidth,
	uint32_t imageHeight,
	uint32_t mipLevels,
	VkSampleCountFlagBits msaaSamples,
	VkFormat format,
	VkImageTiling imageTiling,
	VkImageUsageFlags imageUsage,
	VkFilter filter,
	VkImageAspectFlags imageViewAspectFlags,
	VkSamplerAddressMode imageViewAddressMode,
	VkMemoryPropertyFlags imageMemoryProperties) {
	ImageTools::createImage(&image, imageType, arrayLayers, imageWidth, imageHeight, mipLevels, msaaSamples, format, imageTiling, imageUsage, imageMemoryProperties);
	ImageTools::createImageView(&imageView, image, arrayLayers, mipLevels, imageViewType, format, imageViewAspectFlags);
	ImageTools::createImageSampler(&imageSampler, mipLevels, filter, imageViewAddressMode);
}

void Image::destroy() {
	vkDestroySampler(logicalDevice.device, imageSampler, nullptr);
	vkDestroyImageView(logicalDevice.device, imageView, nullptr);
	vkDestroyImage(logicalDevice.device, image, nullptr);
}
