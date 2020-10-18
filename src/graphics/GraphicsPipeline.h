#pragma once
#include "vulkan/vulkan.hpp"
#include "../utils/NeigeDefines.h"
#include "../utils/NeigeStructs.h"
#include "CommandBuffer.h"
#include "RenderPass.h"
#include "Shader.h"
#include <vector>

struct GraphicsPipeline {
	VkPipeline pipeline;
	VkPipelineLayout pipelineLayout;
	Shader* vertexShader = nullptr;
	Shader* fragmentShader = nullptr;
	Shader* tesselationControlShader = nullptr;
	Shader* tesselationEvaluationShader = nullptr;
	Shader* geometryShader = nullptr;
	std::vector<VkDescriptorSetLayout> descriptorSetLayouts;
	std::vector<VkPushConstantRange> pushConstantRanges;

	void init(bool colorBlend, RenderPass* renderPass, uint32_t viewportWidth, uint32_t viewportHeight);
	void destroy();
	void bind(CommandBuffer* commandBuffer);
};

