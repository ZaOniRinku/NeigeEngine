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
	NEIGE_VK_CHECK(vkAllocateDescriptorSets(logicalDevice.device, nullptr, &descriptorSet));
}

void DescriptorSet::update() {
	for (size_t i = 0; i < graphicsPipeline->layoutBindings.size(); i++) {
		if (graphicsPipeline->layoutBindings.at(i).descriptorType == VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER) {
			
		}
	}
}

void DescriptorSet::destroy() {
	vkFreeDescriptorSets(logicalDevice.device, graphicsPipeline->descriptorPool, 1, &descriptorSet);
}

void DescriptorSet::bind(CommandBuffer* commandBuffer) {
	vkCmdBindDescriptorSets(commandBuffer->commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline->pipelineLayout, 0, 1, &descriptorSet, 0, nullptr);
}