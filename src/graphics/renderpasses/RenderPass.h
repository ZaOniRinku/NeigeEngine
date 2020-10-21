#pragma once
#include "vulkan/vulkan.hpp"
#include "../../utils/NeigeDefines.h"
#include "../../utils/NeigeStructs.h"
#include "../commands/CommandBuffer.h"
#include "RenderPassAttachment.h"
#include <vector>

struct RenderPass {
	VkRenderPass renderPass = VK_NULL_HANDLE;
	std::vector<VkAttachmentDescription> attachmentDescriptions;
	std::vector<VkAttachmentReference> colorAttachmentReferences;
	VkAttachmentReference depthAttachmentReference;
	VkAttachmentReference swapchainAttachmentReference;
	std::vector<VkClearValue> clearValues;
	uint32_t attachmentCount = 0;

	void init(std::vector<RenderPassAttachment> attachments, std::vector<SubpassDependency> subpassDependencies);
	void destroy();
	void begin(CommandBuffer* commandBuffer, VkFramebuffer framebuffer, VkExtent2D extent);
	void end(CommandBuffer* commandBuffer);
};