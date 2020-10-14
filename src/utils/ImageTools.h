#pragma once
#include "vulkan/vulkan.hpp"
#include "NeigeDefines.h"
#include "NeigeStructs.h"
#include "BufferTools.h"
#include "RendererResources.h"
#include "../graphics/Buffer.h"
#include "../graphics/CommandBuffer.h"
#include "../graphics/CommandPool.h"

struct ImageTools {
	static void createImage(VkImage* image,
		uint32_t arrayLayers,
		uint32_t width,
		uint32_t height,
		uint32_t mipLevels,
		VkSampleCountFlagBits msaaSamples,
		VkFormat format,
		VkImageUsageFlags usage,
		VkMemoryPropertyFlags memoryProperties);
	static void createImageView(VkImageView* imageView,
		VkImage image,
		uint32_t arrayLayers,
		uint32_t mipLevels,
		VkImageViewType viewType,
		VkFormat format,
		VkImageAspectFlags aspectFlags);
	static void createImageSampler(VkSampler* sampler,
		uint32_t mipLevels,
		VkFilter filter,
		VkSamplerAddressMode addressMode);
	static void loadImage(const std::string& filePath,
		VkImage* imageDestination,
		VkFormat format);
	static void transitionLayout(VkImage image,
		VkFormat format,
		VkImageLayout oldLayout,
		VkImageLayout newLayout,
		uint32_t mipLevels,
		uint32_t layers);
	static void generateMipmaps(VkImage image,
		VkFormat format,
		int32_t texelWidth,
		int32_t texelHeight,
		uint32_t mipLevels);
};