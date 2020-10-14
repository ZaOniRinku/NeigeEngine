#include "RenderPass.h"
#include "../utils/RendererResources.h"

void RenderPass::init(std::vector<RenderPassAttachment> attachments) {
	for (size_t i = 0; i < attachments.size(); i++) {
		attachmentDescriptions.push_back(attachments[i].description);
		VkAttachmentReference reference = {};
		reference.attachment = attachmentCount++;
		VkClearValue clearValue = {};
		switch (attachments[i].type) {
		case COLOR:
			reference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
			colorAttachmentReferences.push_back(reference);
			clearValue.color = { 0.0f, 0.0f, 0.0f, 1.0f };
			clearValues.push_back(clearValue);
			break;
		case DEPTH:
			reference.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
			depthAttachmentReference = reference;
			clearValue.depthStencil = { 1.0f, 0 };
			clearValues.push_back(clearValue);
			break;
		case SWAPCHAIN:
			reference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
			swapchainAttachmentReference = reference;
			break;
		}
	}
	VkSubpassDescription subpassDescription = {};
	subpassDescription.flags = 0;
	subpassDescription.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpassDescription.colorAttachmentCount = static_cast<uint32_t>(colorAttachmentReferences.size());
	subpassDescription.pColorAttachments = colorAttachmentReferences.data();
	subpassDescription.pResolveAttachments = &swapchainAttachmentReference;
	subpassDescription.pDepthStencilAttachment = &depthAttachmentReference;
	// TODO: manage multi subpass dependencies
	VkSubpassDependency subpassDepedency = {};
	subpassDepedency.srcSubpass = VK_SUBPASS_EXTERNAL;
	subpassDepedency.dstSubpass = 0;
	subpassDepedency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	subpassDepedency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	subpassDepedency.srcAccessMask = 0;
	subpassDepedency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
	VkRenderPassCreateInfo renderPassCreateInfo = {};
	renderPassCreateInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	renderPassCreateInfo.pNext = nullptr;
	renderPassCreateInfo.flags = 0;
	renderPassCreateInfo.attachmentCount = static_cast<uint32_t>(attachmentDescriptions.size());
	renderPassCreateInfo.pAttachments = attachmentDescriptions.data();
	renderPassCreateInfo.subpassCount = 1;
	renderPassCreateInfo.pSubpasses = &subpassDescription;
	renderPassCreateInfo.dependencyCount = 1;
	renderPassCreateInfo.pDependencies = &subpassDepedency;
	NEIGE_VK_CHECK(vkCreateRenderPass(logicalDevice.device, &renderPassCreateInfo, nullptr, &renderPass));
}

void RenderPass::destroy() {
	vkDestroyRenderPass(logicalDevice.device, renderPass, nullptr);
}

void RenderPass::begin(CommandBuffer* commandBuffer, VkFramebuffer framebuffer, VkExtent2D extent) {
	VkRenderPassBeginInfo renderPassBeginInfo = {};
	renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	renderPassBeginInfo.pNext = nullptr;
	renderPassBeginInfo.renderPass = renderPass;
	renderPassBeginInfo.framebuffer = framebuffer;
	renderPassBeginInfo.renderArea.offset = { 0, 0 };
	renderPassBeginInfo.renderArea.extent = extent;
	renderPassBeginInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
	renderPassBeginInfo.pClearValues = clearValues.data();
	vkCmdBeginRenderPass(commandBuffer->commandBuffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
}

void RenderPass::end(CommandBuffer* commandBuffer) {
	vkCmdEndRenderPass(commandBuffer->commandBuffer);
}