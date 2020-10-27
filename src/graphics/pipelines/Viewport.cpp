#include "Viewport.h"

void Viewport::init(uint32_t width, uint32_t height) {
	// Viewport
	viewport = {};
	viewport.x = 0.0f;
	viewport.y = 0.0f;
	viewport.width = static_cast<float>(width);
	viewport.height = static_cast<float>(height);
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;

	scissor = {};
	scissor.offset = { 0, 0 };
	scissor.extent = { width, height };

	viewportCreateInfo = {};
	viewportCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	viewportCreateInfo.pNext = nullptr;
	viewportCreateInfo.flags = 0;
	viewportCreateInfo.viewportCount = 1;
	viewportCreateInfo.pViewports = &viewport;
	viewportCreateInfo.scissorCount = 1;
	viewportCreateInfo.pScissors = &scissor;
}

void Viewport::setViewport(CommandBuffer* commandBuffer) {
	vkCmdSetViewport(commandBuffer->commandBuffer, 0, 1, &viewport);
}

void Viewport::setScissor(CommandBuffer* commandBuffer) {
	vkCmdSetScissor(commandBuffer->commandBuffer, 0, 1, &scissor);
}
