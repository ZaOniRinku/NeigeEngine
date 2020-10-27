#pragma once
#include "vulkan/vulkan.hpp"
#include "GraphicsPipeline.h"
#include "../commands/CommandBuffer.h"

struct DescriptorSet {
	VkDescriptorSet descriptorSet = VK_NULL_HANDLE;
	GraphicsPipeline* graphicsPipeline;
	std::vector<VkWriteDescriptorSet> writesDescriptorSet;

	void init(GraphicsPipeline* graphicsPipeline);
	void update();
	void destroy();
	void bind(CommandBuffer* commandBuffer);
};
