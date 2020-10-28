#pragma once
#include "vulkan/vulkan.hpp"
#include "GraphicsPipeline.h"
#include "../commands/CommandBuffer.h"

struct DescriptorSet {
	VkDescriptorSet descriptorSet = VK_NULL_HANDLE;
	GraphicsPipeline* graphicsPipeline;

	void init(GraphicsPipeline* graphicsPipeline);
	void update(const std::vector<VkWriteDescriptorSet> writeDescriptorSets);
	void destroy();
	void bind(CommandBuffer* commandBuffer);
};
