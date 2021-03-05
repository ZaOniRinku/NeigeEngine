#include "RenderPassAttachment.h"

RenderPassAttachment::RenderPassAttachment(AttachmentType attachmentType,
	VkFormat format,
	VkSampleCountFlagBits msaaSamples,
	VkAttachmentLoadOp loadOp,
	VkAttachmentStoreOp storeOp,
	VkAttachmentLoadOp stencilLoadOp,
	VkAttachmentStoreOp stencilStoreOp,
	VkImageLayout initialLayout,
	VkImageLayout finalLayout) {
	type = attachmentType;
	description.format = format;
	description.samples = msaaSamples;
	description.loadOp = loadOp;
	description.storeOp = storeOp;
	description.stencilLoadOp = stencilLoadOp;
	description.stencilStoreOp = stencilStoreOp;
	description.initialLayout = initialLayout;
	description.finalLayout = finalLayout;
}
