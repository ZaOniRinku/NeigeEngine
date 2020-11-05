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
	std::vector<VkDescriptorSetLayout> descriptorSetLayouts;
	std::string vertexShaderPath;
	std::string fragmentShaderPath;
	std::string tesselationControlShaderPath;
	std::string tesselationEvaluationShaderPath;
	std::string geometryShaderPath;
	RenderPass* renderPass;
	Viewport* viewport;
	Topology topology;
	bool colorBlend = true;
	bool multiSample = true;
	bool depthFail = false;
	std::vector<Set> sets;
	std::vector<VkPushConstantRange> pushConstantRanges;

	void init();
	void destroy();
	void bind(CommandBuffer* commandBuffer);
	void pushConstant(CommandBuffer* commandBuffer, VkShaderStageFlags stages, uint32_t offset, uint32_t size, const void* data);
	void destroyPipeline();
	VkPrimitiveTopology topologyToVkTopology();
};
