#pragma once
#include "vulkan/vulkan.hpp"

enum AttachmentType {
	COLOR,
	DEPTH,
	SWAPCHAIN
};

struct RenderPassAttachment {
	AttachmentType type;
	VkAttachmentDescription description = {};

	RenderPassAttachment(AttachmentType attachmentType,
		VkFormat format,
		VkSampleCountFlagBits msaaSamples,
		VkAttachmentStoreOp storeOp,
		VkAttachmentLoadOp stencilLoadOp,
		VkAttachmentStoreOp stencilStoreOp);
};