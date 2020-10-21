#include "Framebuffer.h"
#include "../resources/RendererResources.h"

void Framebuffer::init(RenderPass* renderPass, std::vector<VkImageView>& attachments, uint32_t width, uint32_t height) {
	VkFramebufferCreateInfo framebufferCreateInfo = {};
	framebufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
	framebufferCreateInfo.pNext = nullptr;
	framebufferCreateInfo.flags = 0;
	framebufferCreateInfo.renderPass = renderPass->renderPass;
	framebufferCreateInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
	framebufferCreateInfo.pAttachments = attachments.data();
	framebufferCreateInfo.width = width;
	framebufferCreateInfo.height = height;
	framebufferCreateInfo.layers = 1;
	NEIGE_VK_CHECK(vkCreateFramebuffer(logicalDevice.device, &framebufferCreateInfo, nullptr, &framebuffer));
}

void Framebuffer::destroy() {
	vkDestroyFramebuffer(logicalDevice.device, framebuffer, nullptr);
}
