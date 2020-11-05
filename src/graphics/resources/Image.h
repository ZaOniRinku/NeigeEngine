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

	void destroy();
};