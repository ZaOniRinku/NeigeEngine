#include "GraphicsPipeline.h"
#include "../utils/RendererResources.h"

void GraphicsPipeline::init(bool colorBlend, RenderPass* renderPass, uint32_t viewportWidth, uint32_t viewportHeight) {
	std::vector<VkPipelineShaderStageCreateInfo> pipelineStages;

	if (vertexShader) {
		NEIGE_ASSERT(vertexShader->type == VERTEX, "Vertex shader in pipeline is not a vertex shader.");

		VkPipelineShaderStageCreateInfo vertexShaderCreateInfo = {};
		vertexShaderCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		vertexShaderCreateInfo.pNext = nullptr;
		vertexShaderCreateInfo.flags = 0;
		vertexShaderCreateInfo.stage = vertexShader->shaderTypeToVkShaderFlagBits();
		vertexShaderCreateInfo.module = vertexShader->module;
		vertexShaderCreateInfo.pName = "main";
		vertexShaderCreateInfo.pSpecializationInfo = nullptr;
		pipelineStages.push_back(vertexShaderCreateInfo);

		VkPipelineVertexInputStateCreateInfo vertexInputCreateInfo = {};
		vertexInputCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
		vertexInputCreateInfo.pNext = nullptr;
		vertexInputCreateInfo.flags = 0;
		vertexInputCreateInfo.vertexBindingDescriptionCount = 0;
		vertexInputCreateInfo.pVertexBindingDescriptions = nullptr;
		vertexInputCreateInfo.vertexAttributeDescriptionCount = 0;
		vertexInputCreateInfo.pVertexAttributeDescriptions = nullptr;

		descriptorSetLayouts.insert(descriptorSetLayouts.end(), vertexShader->descriptorSetLayouts.begin(), vertexShader->descriptorSetLayouts.end());
		pushConstantRanges.insert(pushConstantRanges.end(), vertexShader->pushConstantRanges.begin(), vertexShader->pushConstantRanges.end());
	}

	if (fragmentShader) {
		NEIGE_ASSERT(fragmentShader->type == FRAGMENT, "Fragment shader in pipeline is not a fragment shader.");

		VkPipelineShaderStageCreateInfo fragmentShaderCreateInfo = {};
		fragmentShaderCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		fragmentShaderCreateInfo.pNext = nullptr;
		fragmentShaderCreateInfo.flags = 0;
		fragmentShaderCreateInfo.stage = fragmentShader->shaderTypeToVkShaderFlagBits();
		fragmentShaderCreateInfo.module = fragmentShader->module;
		fragmentShaderCreateInfo.pName = "main";
		fragmentShaderCreateInfo.pSpecializationInfo = nullptr;
		pipelineStages.push_back(fragmentShaderCreateInfo);

		descriptorSetLayouts.insert(descriptorSetLayouts.end(), fragmentShader->descriptorSetLayouts.begin(), fragmentShader->descriptorSetLayouts.end());
		pushConstantRanges.insert(pushConstantRanges.end(), fragmentShader->pushConstantRanges.begin(), fragmentShader->pushConstantRanges.end());
	}

	if (tesselationControlShader) {
		NEIGE_ASSERT(tesselationControlShader->type == TESSELATION_CONTROL, "Tesselation control shader in pipeline is not a tesselation control shader.");

		VkPipelineShaderStageCreateInfo tesselationControlShaderCreateInfo = {};
		tesselationControlShaderCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		tesselationControlShaderCreateInfo.pNext = nullptr;
		tesselationControlShaderCreateInfo.flags = 0;
		tesselationControlShaderCreateInfo.stage = tesselationControlShader->shaderTypeToVkShaderFlagBits();
		tesselationControlShaderCreateInfo.module = tesselationControlShader->module;
		tesselationControlShaderCreateInfo.pName = "main";
		tesselationControlShaderCreateInfo.pSpecializationInfo = nullptr;
		pipelineStages.push_back(tesselationControlShaderCreateInfo);

		descriptorSetLayouts.insert(descriptorSetLayouts.end(), tesselationControlShader->descriptorSetLayouts.begin(), tesselationControlShader->descriptorSetLayouts.end());
		pushConstantRanges.insert(pushConstantRanges.end(), tesselationControlShader->pushConstantRanges.begin(), tesselationControlShader->pushConstantRanges.end());
	}

	if (tesselationEvaluationShader) {
		NEIGE_ASSERT(tesselationEvaluationShader->type == TESSELATION_EVALUATION, "Tesselation evaluation shader in pipeline is not a tesselation evaluation shader.");

		VkPipelineShaderStageCreateInfo tesselationEvaluationShaderCreateInfo = {};
		tesselationEvaluationShaderCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		tesselationEvaluationShaderCreateInfo.pNext = nullptr;
		tesselationEvaluationShaderCreateInfo.flags = 0;
		tesselationEvaluationShaderCreateInfo.stage = tesselationEvaluationShader->shaderTypeToVkShaderFlagBits();
		tesselationEvaluationShaderCreateInfo.module = tesselationEvaluationShader->module;
		tesselationEvaluationShaderCreateInfo.pName = "main";
		tesselationEvaluationShaderCreateInfo.pSpecializationInfo = nullptr;
		pipelineStages.push_back(tesselationEvaluationShaderCreateInfo);

		descriptorSetLayouts.insert(descriptorSetLayouts.end(), tesselationEvaluationShader->descriptorSetLayouts.begin(), tesselationEvaluationShader->descriptorSetLayouts.end());
		pushConstantRanges.insert(pushConstantRanges.end(), tesselationEvaluationShader->pushConstantRanges.begin(), tesselationEvaluationShader->pushConstantRanges.end());
	}

	if (geometryShader) {
		NEIGE_ASSERT(geometryShader->type == GEOMETRY, "Geometry shader in pipeline is not a geometry shader.");

		VkPipelineShaderStageCreateInfo geometryShaderCreateInfo = {};
		geometryShaderCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		geometryShaderCreateInfo.pNext = nullptr;
		geometryShaderCreateInfo.flags = 0;
		geometryShaderCreateInfo.stage = geometryShader->shaderTypeToVkShaderFlagBits();
		geometryShaderCreateInfo.module = geometryShader->module;
		geometryShaderCreateInfo.pName = "main";
		geometryShaderCreateInfo.pSpecializationInfo = nullptr;
		pipelineStages.push_back(geometryShaderCreateInfo);

		descriptorSetLayouts.insert(descriptorSetLayouts.end(), geometryShader->descriptorSetLayouts.begin(), geometryShader->descriptorSetLayouts.end());
		pushConstantRanges.insert(pushConstantRanges.end(), geometryShader->pushConstantRanges.begin(), geometryShader->pushConstantRanges.end());
	}

	// Pipeline layout
	VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo = {};
	pipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutCreateInfo.pNext = nullptr;
	pipelineLayoutCreateInfo.flags = 0;
	pipelineLayoutCreateInfo.setLayoutCount = static_cast<uint32_t>(descriptorSetLayouts.size());
	pipelineLayoutCreateInfo.pSetLayouts = descriptorSetLayouts.data();
	pipelineLayoutCreateInfo.pushConstantRangeCount = static_cast<uint32_t>(pushConstantRanges.size());
	pipelineLayoutCreateInfo.pPushConstantRanges = pushConstantRanges.data();
	NEIGE_VK_CHECK(vkCreatePipelineLayout(logicalDevice.device, &pipelineLayoutCreateInfo, nullptr, &pipelineLayout));

	// Vertex input
	VkVertexInputBindingDescription inputBindingDescription = Vertex::getInputBindingDescription();
	std::vector<VkVertexInputAttributeDescription> inputAttributionDescriptions = Vertex::getInputAttributeDescriptions();

	VkPipelineVertexInputStateCreateInfo vertexInputCreateInfo = {};
	vertexInputCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	vertexInputCreateInfo.pNext = nullptr;
	vertexInputCreateInfo.flags = 0;
	vertexInputCreateInfo.vertexBindingDescriptionCount = 1;
	vertexInputCreateInfo.pVertexBindingDescriptions = &inputBindingDescription;
	vertexInputCreateInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(inputAttributionDescriptions.size());
	vertexInputCreateInfo.pVertexAttributeDescriptions = inputAttributionDescriptions.data();

	// Assembly
	VkPipelineInputAssemblyStateCreateInfo inputAssemblyCreateInfo = {};
	inputAssemblyCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	inputAssemblyCreateInfo.pNext = nullptr;
	inputAssemblyCreateInfo.flags = 0;
	inputAssemblyCreateInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	inputAssemblyCreateInfo.primitiveRestartEnable = VK_FALSE;

	// Viewport
	VkViewport viewport = {};
	viewport.x = 0.0f;
	viewport.y = 0.0f;
	viewport.width = static_cast<float>(viewportWidth);
	viewport.height = static_cast<float>(viewportHeight);
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;

	VkRect2D scissor = {};
	scissor.offset = { 0, 0 };
	scissor.extent = { viewportWidth, viewportHeight };

	VkPipelineViewportStateCreateInfo viewportCreateInfo = {};
	viewportCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	viewportCreateInfo.pNext = nullptr;
	viewportCreateInfo.flags = 0;
	viewportCreateInfo.viewportCount = 1;
	viewportCreateInfo.pViewports = &viewport;
	viewportCreateInfo.scissorCount = 1;
	viewportCreateInfo.pScissors = &scissor;

	// Rasterization
	VkPipelineRasterizationStateCreateInfo rasterizationCreateInfo = {};
	rasterizationCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	rasterizationCreateInfo.pNext = nullptr;
	rasterizationCreateInfo.flags = 0;
	rasterizationCreateInfo.depthClampEnable = VK_FALSE;
	rasterizationCreateInfo.rasterizerDiscardEnable = VK_FALSE;
	rasterizationCreateInfo.polygonMode = VK_POLYGON_MODE_FILL;
	rasterizationCreateInfo.cullMode = VK_CULL_MODE_BACK_BIT;
	rasterizationCreateInfo.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
	rasterizationCreateInfo.depthBiasClamp = VK_FALSE;
	rasterizationCreateInfo.depthBiasConstantFactor = 0.0f;
	rasterizationCreateInfo.depthBiasClamp = 0.0f;
	rasterizationCreateInfo.depthBiasSlopeFactor = 0.0f;
	rasterizationCreateInfo.lineWidth = 0.0f;

	// Multisample
	VkPipelineMultisampleStateCreateInfo multisampleCreateInfo = {};
	multisampleCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	multisampleCreateInfo.pNext = nullptr;
	multisampleCreateInfo.flags = 0;
	multisampleCreateInfo.rasterizationSamples = physicalDevice.maxUsableSampleCount;
	multisampleCreateInfo.sampleShadingEnable = VK_TRUE;
	multisampleCreateInfo.minSampleShading = 0.1f;
	multisampleCreateInfo.pSampleMask = nullptr;
	multisampleCreateInfo.alphaToCoverageEnable = VK_FALSE;
	multisampleCreateInfo.alphaToOneEnable = VK_FALSE;

	// Depth stencil
	VkPipelineDepthStencilStateCreateInfo depthStencilCreateInfo = {};
	depthStencilCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
	depthStencilCreateInfo.pNext = nullptr;
	depthStencilCreateInfo.flags = 0;
	depthStencilCreateInfo.depthTestEnable = VK_TRUE;
	depthStencilCreateInfo.depthWriteEnable = VK_TRUE;
	depthStencilCreateInfo.depthCompareOp = VK_COMPARE_OP_LESS;
	depthStencilCreateInfo.depthBoundsTestEnable = VK_FALSE;
	depthStencilCreateInfo.stencilTestEnable = VK_FALSE;
	depthStencilCreateInfo.front = {};
	depthStencilCreateInfo.back = {};
	depthStencilCreateInfo.minDepthBounds = 0.0f;
	depthStencilCreateInfo.maxDepthBounds = 1.0f;

	// Color blend
	VkPipelineColorBlendStateCreateInfo colorBlendCreateInfo = {};
	if (colorBlend) {
		VkPipelineColorBlendAttachmentState colorBlendAttachment = {};
		colorBlendAttachment.blendEnable = VK_TRUE;
		colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
		colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
		colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
		colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
		colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
		colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;
		colorBlendAttachment.colorWriteMask = { VK_COLOR_COMPONENT_R_BIT |
			VK_COLOR_COMPONENT_G_BIT |
			VK_COLOR_COMPONENT_B_BIT |
			VK_COLOR_COMPONENT_A_BIT };

		colorBlendCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
		colorBlendCreateInfo.pNext = nullptr;
		colorBlendCreateInfo.flags = 0;
		colorBlendCreateInfo.logicOpEnable = VK_FALSE;
		colorBlendCreateInfo.logicOp = VK_LOGIC_OP_COPY;
		colorBlendCreateInfo.attachmentCount = 1;
		colorBlendCreateInfo.pAttachments = &colorBlendAttachment;
		colorBlendCreateInfo.blendConstants[0] = 0.0f;
		colorBlendCreateInfo.blendConstants[1] = 0.0f;
		colorBlendCreateInfo.blendConstants[2] = 0.0f;
		colorBlendCreateInfo.blendConstants[3] = 0.0f;
	}

	// Pipeline
	VkGraphicsPipelineCreateInfo graphicsPipelineCreateInfo = {};
	graphicsPipelineCreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	graphicsPipelineCreateInfo.pNext = nullptr;
	graphicsPipelineCreateInfo.flags = 0;
	graphicsPipelineCreateInfo.stageCount = static_cast<uint32_t>(pipelineStages.size());
	graphicsPipelineCreateInfo.pStages = pipelineStages.data();
	graphicsPipelineCreateInfo.pVertexInputState = &vertexInputCreateInfo;
	graphicsPipelineCreateInfo.pInputAssemblyState = &inputAssemblyCreateInfo;
	graphicsPipelineCreateInfo.pTessellationState = nullptr;
	graphicsPipelineCreateInfo.pViewportState = &viewportCreateInfo;
	graphicsPipelineCreateInfo.pRasterizationState = &rasterizationCreateInfo;
	graphicsPipelineCreateInfo.pMultisampleState = &multisampleCreateInfo;
	graphicsPipelineCreateInfo.pDepthStencilState = &depthStencilCreateInfo;
	graphicsPipelineCreateInfo.pColorBlendState = colorBlend ? &colorBlendCreateInfo : nullptr;
	graphicsPipelineCreateInfo.pDynamicState = nullptr;
	graphicsPipelineCreateInfo.layout = pipelineLayout;
	graphicsPipelineCreateInfo.renderPass = renderPass->renderPass;
	graphicsPipelineCreateInfo.subpass = 0;
	graphicsPipelineCreateInfo.basePipelineHandle = VK_NULL_HANDLE;
	graphicsPipelineCreateInfo.basePipelineIndex = -1;
	NEIGE_VK_CHECK(vkCreateGraphicsPipelines(logicalDevice.device, VK_NULL_HANDLE, 1, &graphicsPipelineCreateInfo, nullptr, &pipeline));
}

void GraphicsPipeline::destroy() {
	vkDestroyPipelineLayout(logicalDevice.device, pipelineLayout, nullptr);
	vkDestroyPipeline(logicalDevice.device, pipeline, nullptr);
}

void GraphicsPipeline::bind(CommandBuffer* commandBuffer) {
	vkCmdBindPipeline(commandBuffer->commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);
}