#pragma once
#include "vulkan/vulkan.hpp"
#include "../../utils/NeigeDefines.h"
#include "../../utils/structs/RendererStructs.h"
#include "../../utils/structs/ShaderStructs.h"
#include "../commands/CommandBuffer.h"
#include "../renderpasses/RenderPass.h"
#include "Shader.h"
#include "Viewport.h"
#include <vector>

struct GraphicsPipeline {
	VkPipeline pipeline = VK_NULL_HANDLE;
	VkPipelineLayout pipelineLayout = VK_NULL_HANDLE;
	VkDescriptorPool descriptorPool = VK_NULL_HANDLE;
	VkDescriptorSetLayout descriptorSetLayout = VK_NULL_HANDLE;
	std::string vertexShaderPath;
	std::string fragmentShaderPath;
	std::string tesselationControlShaderPath;
	std::string tesselationEvaluationShaderPath;
	std::string geometryShaderPath;
	std::vector<VkDescriptorSetLayoutBinding> layoutBindings;
	std::vector<SHADER_TYPE> layoutBindingsShaderTypes;
	std::vector<VkPushConstantRange> pushConstantRanges;
	Viewport* viewport;

	void init(bool colorBlend, RenderPass* renderPass, Viewport* viewportToUse);
	void destroy();
	void bind(CommandBuffer* commandBuffer);
};
