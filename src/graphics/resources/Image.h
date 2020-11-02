#pragma once
#include "vulkan/vulkan.hpp"
#include "../../utils/NeigeDefines.h"
#include "../../utils/structs/RendererStructs.h"

struct Image {
	VkImage image = VK_NULL_HANDLE;
	VkImageView imageView = VK_NULL_HANDLE;
	VkSampler imageSampler = VK_NULL_HANDLE;
	uint32_t width;
	uint32_t height;
	uint32_t mipmapLevels;

	VkDeviceSize allocationId;

	void init(uint32_t arrayLayers,
		uint32_t imageWidth,
		uint32_t imageHeight,
		uint32_t mipLevels,
		VkSampleCountFlagBits msaaSamples,
		VkFormat format,
		VkImageUsageFlags imageUsage,
		VkFilter filter,
		VkImageAspectFlags imageViewAspectFlags,
		VkSamplerAddressMode imageViewAddressMode,
		VkMemoryPropertyFlags imageMemoryProperties,
		VkBorderColor borderColor);
	void destroy();
};