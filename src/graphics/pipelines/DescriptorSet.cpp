#include "DescriptorSet.h"
#include "../resources/RendererResources.h"
#include "../resources/ShaderResources.h"

void DescriptorSet::init(GraphicsPipeline* associatedGraphicsPipeline, uint32_t set) {
	graphicsPipeline = associatedGraphicsPipeline;

	VkDescriptorSetAllocateInfo descriptorSetAllocateInfo = {};
	descriptorSetAllocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	descriptorSetAllocateInfo.pNext = nullptr;
	descriptorSetAllocateInfo.descriptorPool = graphicsPipeline->descriptorPool;
	descriptorSetAllocateInfo.descriptorSetCount = 1;
	descriptorSetAllocateInfo.pSetLayouts = &graphicsPipeline->descriptorSetLayouts[set];
	NEIGE_VK_CHECK(vkAllocateDescriptorSets(logicalDevice.device, &descriptorSetAllocateInfo, &descriptorSet));
}

void DescriptorSet::update(const std::vector<VkWriteDescriptorSet> writesDescriptorSet) {
	vkUpdateDescriptorSets(logicalDevice.device, static_cast<uint32_t>(writesDescriptorSet.size()), writesDescriptorSet.data(), 0, nullptr);
}

void DescriptorSet::destroy() {
	vkFreeDescriptorSets(logicalDevice.device, graphicsPipeline->descriptorPool, 1, &descriptorSet);
}

void DescriptorSet::bind(CommandBuffer* commandBuffer, uint32_t set) {
	vkCmdBindDescriptorSets(commandBuffer->commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline->pipelineLayout, set, 1, &descriptorSet, 0, nullptr);
}

void DescriptorSet::createEntityDescriptorSet(uint32_t entity, uint32_t frameInFlightIndex) {
	std::vector<VkWriteDescriptorSet> writesDescriptorSet;
	std::vector<VkDescriptorImageInfo> shadowMapsInfos;

	std::vector<std::string> bindingNames = graphicsPipeline->sets[0].names;
	int index = 0;
	for (std::string bindingName : bindingNames) {
		if (bindingName == "object") {
			VkDescriptorBufferInfo objectInfo = {};
			objectInfo.buffer = entityBuffers.at(entity).at(frameInFlightIndex).buffer;
			objectInfo.offset = 0;
			objectInfo.range = sizeof(ObjectUniformBufferObject);

			VkWriteDescriptorSet objectWriteDescriptorSet = {};
			objectWriteDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			objectWriteDescriptorSet.pNext = nullptr;
			objectWriteDescriptorSet.dstSet = descriptorSet;
			objectWriteDescriptorSet.dstBinding = index++;
			objectWriteDescriptorSet.dstArrayElement = 0;
			objectWriteDescriptorSet.descriptorCount = 1;
			objectWriteDescriptorSet.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			objectWriteDescriptorSet.pImageInfo = nullptr;
			objectWriteDescriptorSet.pBufferInfo = &objectInfo;
			objectWriteDescriptorSet.pTexelBufferView = nullptr;

			writesDescriptorSet.push_back(objectWriteDescriptorSet);
		}
		else if (bindingName == "camera") {
			VkDescriptorBufferInfo cameraInfo = {};
			cameraInfo.buffer = cameraBuffers.at(frameInFlightIndex).buffer;
			cameraInfo.offset = 0;
			cameraInfo.range = sizeof(CameraUniformBufferObject);

			VkWriteDescriptorSet cameraWriteDescriptorSet = {};
			cameraWriteDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			cameraWriteDescriptorSet.pNext = nullptr;
			cameraWriteDescriptorSet.dstSet = descriptorSet;
			cameraWriteDescriptorSet.dstBinding = index++;
			cameraWriteDescriptorSet.dstArrayElement = 0;
			cameraWriteDescriptorSet.descriptorCount = 1;
			cameraWriteDescriptorSet.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			cameraWriteDescriptorSet.pImageInfo = nullptr;
			cameraWriteDescriptorSet.pBufferInfo = &cameraInfo;
			cameraWriteDescriptorSet.pTexelBufferView = nullptr;

			writesDescriptorSet.push_back(cameraWriteDescriptorSet);
		}
		else if (bindingName == "shadow") {
			VkDescriptorBufferInfo shadowInfo = {};
			shadowInfo.buffer = shadow.buffers.at(frameInFlightIndex).buffer;
			shadowInfo.offset = 0;
			shadowInfo.range = sizeof(ShadowUniformBufferObject);

			VkWriteDescriptorSet shadowWriteDescriptorSet = {};
			shadowWriteDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			shadowWriteDescriptorSet.pNext = nullptr;
			shadowWriteDescriptorSet.dstSet = descriptorSet;
			shadowWriteDescriptorSet.dstBinding = index++;
			shadowWriteDescriptorSet.dstArrayElement = 0;
			shadowWriteDescriptorSet.descriptorCount = 1;
			shadowWriteDescriptorSet.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			shadowWriteDescriptorSet.pImageInfo = nullptr;
			shadowWriteDescriptorSet.pBufferInfo = &shadowInfo;
			shadowWriteDescriptorSet.pTexelBufferView = nullptr;

			writesDescriptorSet.push_back(shadowWriteDescriptorSet);
		}
		else if (bindingName == "lights") {
			VkDescriptorBufferInfo lightingInfo = {};
			lightingInfo.buffer = lightingBuffers.at(frameInFlightIndex).buffer;
			lightingInfo.offset = 0;
			lightingInfo.range = sizeof(LightingUniformBufferObject);

			VkWriteDescriptorSet lightingWriteDescriptorSet = {};
			lightingWriteDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			lightingWriteDescriptorSet.pNext = nullptr;
			lightingWriteDescriptorSet.dstSet = descriptorSet;
			lightingWriteDescriptorSet.dstBinding = index++;
			lightingWriteDescriptorSet.dstArrayElement = 0;
			lightingWriteDescriptorSet.descriptorCount = 1;
			lightingWriteDescriptorSet.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			lightingWriteDescriptorSet.pImageInfo = nullptr;
			lightingWriteDescriptorSet.pBufferInfo = &lightingInfo;
			lightingWriteDescriptorSet.pTexelBufferView = nullptr;

			writesDescriptorSet.push_back(lightingWriteDescriptorSet);
		}
		else if (bindingName == "irradianceMap") {
			VkDescriptorImageInfo irradianceInfo = {};
			irradianceInfo.sampler = envmap.diffuseIradianceImage.imageSampler;
			irradianceInfo.imageView = envmap.diffuseIradianceImage.imageView;
			irradianceInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

			VkWriteDescriptorSet irradianceWriteDescriptorSet = {};
			irradianceWriteDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			irradianceWriteDescriptorSet.pNext = nullptr;
			irradianceWriteDescriptorSet.dstSet = descriptorSet;
			irradianceWriteDescriptorSet.dstBinding = index++;
			irradianceWriteDescriptorSet.dstArrayElement = 0;
			irradianceWriteDescriptorSet.descriptorCount = 1;
			irradianceWriteDescriptorSet.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
			irradianceWriteDescriptorSet.pImageInfo = &irradianceInfo;
			irradianceWriteDescriptorSet.pBufferInfo = nullptr;
			irradianceWriteDescriptorSet.pTexelBufferView = nullptr;

			writesDescriptorSet.push_back(irradianceWriteDescriptorSet);
		}
		else if (bindingName == "prefilterMap") {
			VkDescriptorImageInfo prefilterInfo = {};
			prefilterInfo.sampler = envmap.prefilterImage.imageSampler;
			prefilterInfo.imageView = envmap.prefilterImage.imageView;
			prefilterInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

			VkWriteDescriptorSet prefilterWriteDescriptorSet = {};
			prefilterWriteDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			prefilterWriteDescriptorSet.pNext = nullptr;
			prefilterWriteDescriptorSet.dstSet = descriptorSet;
			prefilterWriteDescriptorSet.dstBinding = index++;
			prefilterWriteDescriptorSet.dstArrayElement = 0;
			prefilterWriteDescriptorSet.descriptorCount = 1;
			prefilterWriteDescriptorSet.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
			prefilterWriteDescriptorSet.pImageInfo = &prefilterInfo;
			prefilterWriteDescriptorSet.pBufferInfo = nullptr;
			prefilterWriteDescriptorSet.pTexelBufferView = nullptr;

			writesDescriptorSet.push_back(prefilterWriteDescriptorSet);
		}
		else if (bindingName == "brdfLUT") {
			VkDescriptorImageInfo brdfLUTInfo = {};
			brdfLUTInfo.sampler = envmap.brdfConvolutionImage.imageSampler;
			brdfLUTInfo.imageView = envmap.brdfConvolutionImage.imageView;
			brdfLUTInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

			VkWriteDescriptorSet brdfLUTWriteDescriptorSet = {};
			brdfLUTWriteDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			brdfLUTWriteDescriptorSet.pNext = nullptr;
			brdfLUTWriteDescriptorSet.dstSet = descriptorSet;
			brdfLUTWriteDescriptorSet.dstBinding = index++;
			brdfLUTWriteDescriptorSet.dstArrayElement = 0;
			brdfLUTWriteDescriptorSet.descriptorCount = 1;
			brdfLUTWriteDescriptorSet.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
			brdfLUTWriteDescriptorSet.pImageInfo = &brdfLUTInfo;
			brdfLUTWriteDescriptorSet.pBufferInfo = nullptr;
			brdfLUTWriteDescriptorSet.pTexelBufferView = nullptr;

			writesDescriptorSet.push_back(brdfLUTWriteDescriptorSet);
		}
		else if (bindingName == "shadowMaps") {
			shadowMapsInfos.resize(MAX_DIR_LIGHTS + MAX_POINT_LIGHTS + MAX_SPOT_LIGHTS);

			for (int i = 0; i < MAX_DIR_LIGHTS + MAX_POINT_LIGHTS + MAX_SPOT_LIGHTS; i++) {
				shadowMapsInfos[i].sampler = shadow.defaultShadow.imageSampler;
				shadowMapsInfos[i].imageView = (i < shadow.mapCount) ? shadow.images[i].imageView : shadow.defaultShadow.imageView;
				shadowMapsInfos[i].imageLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL;
			}

			VkWriteDescriptorSet shadowMapsWriteDescriptorSet = {};
			shadowMapsWriteDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			shadowMapsWriteDescriptorSet.pNext = nullptr;
			shadowMapsWriteDescriptorSet.dstSet = descriptorSet;
			shadowMapsWriteDescriptorSet.dstBinding = index++;
			shadowMapsWriteDescriptorSet.dstArrayElement = 0;
			shadowMapsWriteDescriptorSet.descriptorCount = static_cast<uint32_t>(shadowMapsInfos.size());
			shadowMapsWriteDescriptorSet.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
			shadowMapsWriteDescriptorSet.pImageInfo = shadowMapsInfos.data();
			shadowMapsWriteDescriptorSet.pBufferInfo = nullptr;
			shadowMapsWriteDescriptorSet.pTexelBufferView = nullptr;

			writesDescriptorSet.push_back(shadowMapsWriteDescriptorSet);
		}
		else if (bindingName == "time") {
			VkDescriptorBufferInfo timeInfo = {};
			timeInfo.buffer = timeBuffers.at(frameInFlightIndex).buffer;
			timeInfo.offset = 0;
			timeInfo.range = sizeof(float);

			VkWriteDescriptorSet timeWriteDescriptorSet = {};
			timeWriteDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			timeWriteDescriptorSet.pNext = nullptr;
			timeWriteDescriptorSet.dstSet = descriptorSet;
			timeWriteDescriptorSet.dstBinding = index++;
			timeWriteDescriptorSet.dstArrayElement = 0;
			timeWriteDescriptorSet.descriptorCount = 1;
			timeWriteDescriptorSet.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			timeWriteDescriptorSet.pImageInfo = nullptr;
			timeWriteDescriptorSet.pBufferInfo = &timeInfo;
			timeWriteDescriptorSet.pTexelBufferView = nullptr;

			writesDescriptorSet.push_back(timeWriteDescriptorSet);
		}
	}

	update(writesDescriptorSet);
}
