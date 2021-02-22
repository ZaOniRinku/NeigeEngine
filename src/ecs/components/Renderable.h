#pragma once
#include "../../graphics/resources/Buffer.h"
#include "../../graphics/pipelines/DescriptorSet.h"
#include "../../graphics/pipelines/GraphicsPipeline.h"
#include "../../graphics/resources/ShaderResources.h"
#include "../../utils/structs/ShaderStructs.h"
#include <string>
#include <vector>
#include <iostream>

struct Renderable {
	// Model
	std::string modelPath = "";

	// Shaders
	std::string vertexShaderPath = "";
	std::string fragmentShaderPath = "";
	std::string tesselationControlShaderPath = "";
	std::string tesselationEvaluationShaderPath = "";
	std::string geometryShaderPath = "";

	// Pipeline topology
	Topology topology = Topology::TRIANGLE_LIST;

	// GraphicsPipeline
	GraphicsPipeline* graphicsPipeline;
	std::string lookupString = "";

	void createLookupString() {
		lookupString = vertexShaderPath + fragmentShaderPath + tesselationControlShaderPath + tesselationEvaluationShaderPath + geometryShaderPath + std::to_string(static_cast<int>(topology));
	}

	// Descriptor sets and buffers
	std::vector<Buffer> buffers;
	std::vector<DescriptorSet> descriptorSets;
	std::vector<DescriptorSet> shadowDescriptorSets;

	void createEntityDescriptorSet(uint32_t frameInFlightIndex) {
		descriptorSets[frameInFlightIndex].init(graphicsPipeline, 0);

		std::vector<VkWriteDescriptorSet> writesDescriptorSet;
		std::vector<VkDescriptorImageInfo> shadowMapsInfos;

		for (size_t i = 0; i < graphicsPipeline->sets[0].bindings.size(); i++) {
			std::string bindingName = graphicsPipeline->sets[0].bindings[i].name;
			if (bindingName == "object") {
				VkDescriptorBufferInfo objectInfo = {};
				objectInfo.buffer = buffers.at(frameInFlightIndex).buffer;
				objectInfo.offset = 0;
				objectInfo.range = sizeof(ObjectUniformBufferObject);

				VkWriteDescriptorSet objectWriteDescriptorSet = {};
				objectWriteDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
				objectWriteDescriptorSet.pNext = nullptr;
				objectWriteDescriptorSet.dstSet = descriptorSets.at(frameInFlightIndex).descriptorSet;
				objectWriteDescriptorSet.dstBinding = graphicsPipeline->sets[0].bindings[i].binding.binding;
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
				cameraWriteDescriptorSet.dstSet = descriptorSets.at(frameInFlightIndex).descriptorSet;
				cameraWriteDescriptorSet.dstBinding = graphicsPipeline->sets[0].bindings[i].binding.binding;
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
				shadowWriteDescriptorSet.dstSet = descriptorSets.at(frameInFlightIndex).descriptorSet;
				shadowWriteDescriptorSet.dstBinding = graphicsPipeline->sets[0].bindings[i].binding.binding;
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
				lightingWriteDescriptorSet.dstSet = descriptorSets.at(frameInFlightIndex).descriptorSet;
				lightingWriteDescriptorSet.dstBinding = graphicsPipeline->sets[0].bindings[i].binding.binding;
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
				irradianceWriteDescriptorSet.dstSet = descriptorSets.at(frameInFlightIndex).descriptorSet;
				irradianceWriteDescriptorSet.dstBinding = graphicsPipeline->sets[0].bindings[i].binding.binding;
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
				prefilterWriteDescriptorSet.dstSet = descriptorSets.at(frameInFlightIndex).descriptorSet;
				prefilterWriteDescriptorSet.dstBinding = graphicsPipeline->sets[0].bindings[i].binding.binding;
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
				brdfLUTWriteDescriptorSet.dstSet = descriptorSets.at(frameInFlightIndex).descriptorSet;
				brdfLUTWriteDescriptorSet.dstBinding = graphicsPipeline->sets[0].bindings[i].binding.binding;
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

				for (int j = 0; j < MAX_DIR_LIGHTS + MAX_POINT_LIGHTS + MAX_SPOT_LIGHTS; j++) {
					shadowMapsInfos[j].sampler = shadow.defaultShadow.imageSampler;
					shadowMapsInfos[j].imageView = (j < shadow.mapCount) ? shadow.images[j].imageView : shadow.defaultShadow.imageView;
					shadowMapsInfos[j].imageLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL;
				}

				VkWriteDescriptorSet shadowMapsWriteDescriptorSet = {};
				shadowMapsWriteDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
				shadowMapsWriteDescriptorSet.pNext = nullptr;
				shadowMapsWriteDescriptorSet.dstSet = descriptorSets.at(frameInFlightIndex).descriptorSet;
				shadowMapsWriteDescriptorSet.dstBinding = graphicsPipeline->sets[0].bindings[i].binding.binding;
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
				timeWriteDescriptorSet.dstSet = descriptorSets.at(frameInFlightIndex).descriptorSet;
				timeWriteDescriptorSet.dstBinding = graphicsPipeline->sets[0].bindings[i].binding.binding;
				timeWriteDescriptorSet.dstArrayElement = 0;
				timeWriteDescriptorSet.descriptorCount = 1;
				timeWriteDescriptorSet.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
				timeWriteDescriptorSet.pImageInfo = nullptr;
				timeWriteDescriptorSet.pBufferInfo = &timeInfo;
				timeWriteDescriptorSet.pTexelBufferView = nullptr;

				writesDescriptorSet.push_back(timeWriteDescriptorSet);
			}
		}

		descriptorSets.at(frameInFlightIndex).update(writesDescriptorSet);
	}

	void createShadowEntityDescriptorSet(uint32_t frameInFlightIndex) {
		shadowDescriptorSets[frameInFlightIndex].init(&shadow.graphicsPipeline, 0);

		VkDescriptorBufferInfo objectInfo = {};
		objectInfo.buffer = buffers.at(frameInFlightIndex).buffer;
		objectInfo.offset = 0;
		objectInfo.range = sizeof(ObjectUniformBufferObject);

		VkDescriptorBufferInfo shadowInfo = {};
		shadowInfo.buffer = shadow.buffers.at(frameInFlightIndex).buffer;
		shadowInfo.offset = 0;
		shadowInfo.range = sizeof(ShadowUniformBufferObject);

		std::vector<VkWriteDescriptorSet> writesDescriptorSet;

		VkWriteDescriptorSet objectWriteDescriptorSet = {};
		objectWriteDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		objectWriteDescriptorSet.pNext = nullptr;
		objectWriteDescriptorSet.dstSet = shadowDescriptorSets[frameInFlightIndex].descriptorSet;
		objectWriteDescriptorSet.dstBinding = 0;
		objectWriteDescriptorSet.dstArrayElement = 0;
		objectWriteDescriptorSet.descriptorCount = 1;
		objectWriteDescriptorSet.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		objectWriteDescriptorSet.pImageInfo = nullptr;
		objectWriteDescriptorSet.pBufferInfo = &objectInfo;
		objectWriteDescriptorSet.pTexelBufferView = nullptr;
		writesDescriptorSet.push_back(objectWriteDescriptorSet);

		VkWriteDescriptorSet shadowWriteDescriptorSet = {};
		shadowWriteDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		shadowWriteDescriptorSet.pNext = nullptr;
		shadowWriteDescriptorSet.dstSet = shadowDescriptorSets[frameInFlightIndex].descriptorSet;
		shadowWriteDescriptorSet.dstBinding = 1;
		shadowWriteDescriptorSet.dstArrayElement = 0;
		shadowWriteDescriptorSet.descriptorCount = 1;
		shadowWriteDescriptorSet.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		shadowWriteDescriptorSet.pImageInfo = nullptr;
		shadowWriteDescriptorSet.pBufferInfo = &shadowInfo;
		shadowWriteDescriptorSet.pTexelBufferView = nullptr;
		writesDescriptorSet.push_back(shadowWriteDescriptorSet);

		shadowDescriptorSets[frameInFlightIndex].update(writesDescriptorSet);
	}
};