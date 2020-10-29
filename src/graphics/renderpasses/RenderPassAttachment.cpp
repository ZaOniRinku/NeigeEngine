#include "RenderPassAttachment.h"

RenderPassAttachment::RenderPassAttachment(AttachmentType attachmentType,
	VkFormat format,
	VkSampleCountFlagBits msaaSamples,
	VkAttachmentLoadOp loadOp,
	VkAttachmentStoreOp storeOp,
	VkAttachmentLoadOp stencilLoadOp,
	VkAttachmentStoreOp stencilStoreOp) {
	type = attachmentType;
	description.format = format;
	description.samples = msaaSamples;
	description.loadOp = loadOp;
	description.storeOp = storeOp;
	description.stencilLoadOp = stencilLoadOp;
	description.stencilStoreOp = stencilStoreOp;
	description.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

	switch (type) {
	case AttachmentType::COLOR:
		description.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		break;
	case AttachmentType::DEPTH:
		description.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
		break;
	case AttachmentType::SWAPCHAIN:
		description.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
		break;
	}
}
