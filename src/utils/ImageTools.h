#pragma once
#include "vulkan/vulkan.hpp"
#include "NeigeDefines.h"
#include "NeigeStructs.h"
#include "RendererResources.h"

struct ImageTools {
	static void createImage(VkImage image,
		VkImageType type,
		uint32_t arrayLayers,
		uint32_t width,
		uint32_t height,
		uint32_t mipLevels,
		VkSampleCountFlagBits samples,
		VkFormat format,
		VkImageTiling tiling,
		VkImageUsageFlags usage,
		VkMemoryPropertyFlags memoryProperties);
	static void createImageView(VkImageView imageView,
		VkImage image,
		uint32_t arrayLayers,
		uint32_t mipLevels,
		VkImageViewType viewType,
		VkFormat format,
		VkImageAspectFlags aspectFlags);
	static void createImageSampler(VkSampler sampler,
		uint32_t mipLevels,
		VkFilter filter,
		VkSamplerAddressMode addressMode);
};