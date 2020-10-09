#pragma once
#include "vulkan/vulkan.hpp"
#include "../utils/NeigeDefines.h"
#include "../utils/NeigeStructs.h"

struct Image {
	VkImage image = VK_NULL_HANDLE;
	VkImageView imageView = VK_NULL_HANDLE;
	VkSampler imageSampler = VK_NULL_HANDLE;
	uint32_t mipLevels;

	void init(VkImageType imageType,
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
		VkMemoryPropertyFlags imageMemoryProperties);
	void destroy();
};