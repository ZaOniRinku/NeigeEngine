#include "DescriptorSet.h"
#include "../resources/RendererResources.h"

void DescriptorSet::init(GraphicsPipeline* associatedGraphicsPipeline) {
	graphicsPipeline = associatedGraphicsPipeline;

	VkDescriptorSetAllocateInfo descriptorSetAllocateInfo = {};
	descriptorSetAllocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	descriptorSetAllocateInfo.pNext = nullptr;
	descriptorSetAllocateInfo.descriptorPool = graphicsPipeline->descriptorPool;
	descriptorSetAllocateInfo.descriptorSetCount = 1;
	descriptorSetAllocateInfo.pSetLayouts = &graphicsPipeline->descriptorSetLayout;
	NEIGE_VK_CHECK(vkAllocateDescriptorSets(logicalDevice.device, &descriptorSetAllocateInfo, &descriptorSet));
}

void DescriptorSet::update(const std::vector<VkWriteDescriptorSet> writesDescriptorSet) {
	vkUpdateDescriptorSets(logicalDevice.device, static_cast<uint32_t>(writesDescriptorSet.size()), writesDescriptorSet.data(), 0, nullptr);
}

void DescriptorSet::destroy() {
	vkFreeDescriptorSets(logicalDevice.device, graphicsPipeline->descriptorPool, 1, &descriptorSet);
}

void DescriptorSet::bind(CommandBuffer* commandBuffer) {
	vkCmdBindDescriptorSets(commandBuffer->commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline->pipelineLayout, 0, 1, &descriptorSet, 0, nullptr);
}