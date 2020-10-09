#pragma once
#include "vulkan/vulkan.hpp"
#include "Image.h"

struct Framebuffer {
	VkFramebuffer framebuffer = VK_NULL_HANDLE;
	std::vector<Image> attachments;

	void destroy();
};