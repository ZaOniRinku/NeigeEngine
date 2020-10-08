#pragma once
#include "vulkan/vulkan.hpp"
#include "../utils/ImageTools.h"
#include "../utils/NeigeDefines.h"
#include "../utils/NeigeStructs.h"

struct Image {
	VkImage image;
	VkImageView imageView;
	VkSampler imageSampler;
};