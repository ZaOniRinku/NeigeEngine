#include "DescriptorSet.h"
#include "../resources/RendererResources.h"

void DescriptorSet::init(GraphicsPipeline* graphicsPipeline) {
	pipelineLayout = graphicsPipeline->pipelineLayout;
	descriptorPool = graphicsPipeline->descriptorPool;

	VkDescriptorSetAllocateInfo descriptorSetAllocateInfo = {};
	descriptorSetAllocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	descriptorSetAllocateInfo.pNext = nullptr;
	descriptorSetAllocateInfo.descriptorPool = graphicsPipeline->descriptorPool;
	descriptorSetAllocateInfo.descriptorSetCount = 1;
	descriptorSetAllocateInfo.pSetLayouts = &graphicsPipeline->descriptorSetLayout;
	NEIGE_VK_CHECK(vkAllocateDescriptorSets(logicalDevice.device, nullptr, &descriptorSet));
}

void DescriptorSet::destroy() {
	vkFreeDescriptorSets(logicalDevice.device, descriptorPool, 1, &descriptorSet);
}

void DescriptorSet::bind(CommandBuffer* commandBuffer) {
	vkCmdBindDescriptorSets(commandBuffer->commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1, &descriptorSet, 0, nullptr);
}