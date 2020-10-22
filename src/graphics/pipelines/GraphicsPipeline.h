#pragma once
#include "vulkan/vulkan.hpp"
#include "../../utils/NeigeDefines.h"
#include "../../utils/NeigeStructs.h"
#include "../commands/CommandBuffer.h"
#include "../renderpasses/RenderPass.h"
#include "Shader.h"
#include <vector>

struct GraphicsPipeline {
	VkPipeline pipeline = VK_NULL_HANDLE;
	VkPipelineLayout pipelineLayout = VK_NULL_HANDLE;
	VkDescriptorPool descriptorPool = VK_NULL_HANDLE;
	std::string vertexShaderPath;
	std::string fragmentShaderPath;
	std::string tesselationControlShaderPath;
	std::string tesselationEvaluationShaderPath;
	std::string geometryShaderPath;
	std::vector<VkDescriptorSetLayout> descriptorSetLayouts;
	std::vector<VkPushConstantRange> pushConstantRanges;

	void init(bool colorBlend, RenderPass* renderPass, uint32_t viewportWidth, uint32_t viewportHeight);
	void destroy();
	void bind(CommandBuffer* commandBuffer);
};
