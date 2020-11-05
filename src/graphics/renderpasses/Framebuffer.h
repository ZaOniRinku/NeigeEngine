#pragma once
#include "vulkan/vulkan.hpp"
#include "../../utils/NeigeDefines.h"
#include "RenderPass.h"

struct Framebuffer {
	VkFramebuffer framebuffer = VK_NULL_HANDLE;

	void init(RenderPass* renderPass, std::vector<VkImageView>& attachments, uint32_t width, uint32_t height, uint32_t arrayLayers);
	void destroy();
};