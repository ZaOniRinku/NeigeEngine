#include "RenderPassAttachment.h"

RenderPassAttachment::RenderPassAttachment(AttachmentType attachmentType,
	VkFormat format,
	VkSampleCountFlagBits msaaSamples,
	VkAttachmentStoreOp storeOp,
	VkAttachmentLoadOp stencilLoadOp,
	VkAttachmentStoreOp stencilStoreOp) {
	type = attachmentType;
	attachmentDescription.format = format;
	attachmentDescription.samples = msaaSamples;
	attachmentDescription.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	attachmentDescription.storeOp = storeOp;
	attachmentDescription.stencilLoadOp = stencilLoadOp;
	attachmentDescription.stencilStoreOp = stencilStoreOp;
	attachmentDescription.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

	switch (type) {
	case COLOR:
		attachmentDescription.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		break;
	case DEPTH:
		attachmentDescription.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
		break;
	case SWAPCHAIN:
		attachmentDescription.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
		break;
	}
}
