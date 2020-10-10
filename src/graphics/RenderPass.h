#pragma once
#include "vulkan/vulkan.hpp"
#include "../utils/NeigeDefines.h"
#include "CommandBuffer.h"
#include "RenderPassAttachment.h"
#include <vector>

struct RenderPass {
	VkRenderPass renderPass = VK_NULL_HANDLE;
	std::vector<VkAttachmentDescription> attachmentDescriptions;
	std::vector<VkAttachmentReference> colorAttachmentReferences;
	std::vector<VkAttachmentReference> otherAttachmentReferences;
	std::vector<VkClearValue> clearValues;
	uint32_t attachmentCount = 0;
	int depthIndex = -1;
	int swapchainIndex = -1;

	void init(std::vector<RenderPassAttachment> attachments);
	void destroy();
	void begin(CommandBuffer* commandBuffer, VkFramebuffer framebuffer, VkExtent2D extent);
	void end(CommandBuffer* commandBuffer);
};