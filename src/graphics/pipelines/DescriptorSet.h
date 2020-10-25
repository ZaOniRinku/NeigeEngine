#pragma once
#include "vulkan/vulkan.hpp"
#include "GraphicsPipeline.h"
#include "../commands/CommandBuffer.h"

struct DescriptorSet {
	VkDescriptorSet descriptorSet = VK_NULL_HANDLE;
	VkPipelineLayout pipelineLayout;
	VkDescriptorPool descriptorPool;

	void init(GraphicsPipeline* graphicsPipeline);
	void destroy();
	void bind(CommandBuffer* commandBuffer);
};
