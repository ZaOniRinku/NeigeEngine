#pragma once
#include "vulkan/vulkan.hpp"
#include "../utils/NeigeDefines.h"
#include "RenderPass.h"

struct Framebuffer {
	VkFramebuffer framebuffer = VK_NULL_HANDLE;
	std::vector<VkImageView> attachments;

	void init(RenderPass* renderPass, uint32_t width, uint32_t height);
	void destroy();
};