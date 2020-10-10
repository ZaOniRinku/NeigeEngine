#include "RenderPassAttachment.h"

RenderPassAttachment::RenderPassAttachment(AttachmentType attachmentType,
	VkFormat format,
	VkSampleCountFlagBits msaaSamples,
	VkAttachmentStoreOp storeOp,
	VkAttachmentLoadOp stencilLoadOp,
	VkAttachmentStoreOp stencilStoreOp) {
	type = attachmentType;
	description.format = format;
	description.samples = msaaSamples;
	description.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	description.storeOp = storeOp;
	description.stencilLoadOp = stencilLoadOp;
	description.stencilStoreOp = stencilStoreOp;
	description.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

	switch (type) {
	case COLOR:
		description.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		break;
	case DEPTH:
		description.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
		break;
	case SWAPCHAIN:
		description.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
		break;
	}
}
