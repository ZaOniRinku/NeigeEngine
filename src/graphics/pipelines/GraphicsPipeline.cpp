#include "GraphicsPipeline.h"
#include "../resources/RendererResources.h"

void GraphicsPipeline::init(bool colorBlend, RenderPass* renderPass, Viewport* viewportToUse) {
	viewport = viewportToUse;

	layoutBindings.clear();
	layoutBindings.shrink_to_fit();

	pushConstantRanges.clear();
	pushConstantRanges.shrink_to_fit();

	std::vector<VkPipelineShaderStageCreateInfo> pipelineStages;
	std::set<VkDescriptorType> uniqueDescriptorTypes;

	if (vertexShaderPath != "") {
		Shader shader;
		std::unordered_map<std::string, Shader>::const_iterator mapSearch = shaders.find(vertexShaderPath);
		if (mapSearch == shaders.end()) {
			shader.init(vertexShaderPath);
			shaders.emplace(vertexShaderPath, shader);
		}
		else {
			shader = shaders[vertexShaderPath];
		}
		NEIGE_ASSERT(shader.type == ShaderType::VERTEX, "Vertex shader in pipeline is not a vertex shader.");

		VkPipelineShaderStageCreateInfo vertexShaderCreateInfo = {};
		vertexShaderCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		vertexShaderCreateInfo.pNext = nullptr;
		vertexShaderCreateInfo.flags = 0;
		vertexShaderCreateInfo.stage = shader.shaderTypeToVkShaderFlagBits();
		vertexShaderCreateInfo.module = shader.module;
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

		layoutBindings.insert(layoutBindings.end(), shader.layoutBindings.begin(), shader.layoutBindings.end());
		pushConstantRanges.insert(pushConstantRanges.end(), shader.pushConstantRanges.begin(), shader.pushConstantRanges.end());
		uniqueDescriptorTypes.insert(shader.uniqueDescriptorTypes.begin(), shader.uniqueDescriptorTypes.end());
	}

	if (fragmentShaderPath != "") {
		Shader shader;
		std::unordered_map<std::string, Shader>::const_iterator mapSearch = shaders.find(fragmentShaderPath);
		if (mapSearch == shaders.end()) {
			shader.init(fragmentShaderPath);
			shaders.emplace(fragmentShaderPath, shader);
		}
		else {
			shader = shaders[fragmentShaderPath];
		}
		NEIGE_ASSERT(shader.type == ShaderType::FRAGMENT, "Fragment shader in pipeline is not a fragment shader.");

		VkPipelineShaderStageCreateInfo fragmentShaderCreateInfo = {};
		fragmentShaderCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		fragmentShaderCreateInfo.pNext = nullptr;
		fragmentShaderCreateInfo.flags = 0;
		fragmentShaderCreateInfo.stage = shader.shaderTypeToVkShaderFlagBits();
		fragmentShaderCreateInfo.module = shader.module;
		fragmentShaderCreateInfo.pName = "main";
		fragmentShaderCreateInfo.pSpecializationInfo = nullptr;
		pipelineStages.push_back(fragmentShaderCreateInfo);

		layoutBindings.insert(layoutBindings.end(), shader.layoutBindings.begin(), shader.layoutBindings.end());
		pushConstantRanges.insert(pushConstantRanges.end(), shader.pushConstantRanges.begin(), shader.pushConstantRanges.end());
		uniqueDescriptorTypes.insert(shader.uniqueDescriptorTypes.begin(), shader.uniqueDescriptorTypes.end());
	}

	if (tesselationControlShaderPath != "") {
		Shader shader;
		std::unordered_map<std::string, Shader>::const_iterator mapSearch = shaders.find(tesselationControlShaderPath);
		if (mapSearch == shaders.end()) {
			shader.init(tesselationControlShaderPath);
			shaders.emplace(tesselationControlShaderPath, shader);
		}
		else {
			shader = shaders[tesselationControlShaderPath];
		}
		NEIGE_ASSERT(shader.type == ShaderType::TESSELATION_CONTROL, "Tesselation control shader in pipeline is not a tesselation control shader.");

		VkPipelineShaderStageCreateInfo tesselationControlShaderCreateInfo = {};
		tesselationControlShaderCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		tesselationControlShaderCreateInfo.pNext = nullptr;
		tesselationControlShaderCreateInfo.flags = 0;
		tesselationControlShaderCreateInfo.stage = shader.shaderTypeToVkShaderFlagBits();
		tesselationControlShaderCreateInfo.module = shader.module;
		tesselationControlShaderCreateInfo.pName = "main";
		tesselationControlShaderCreateInfo.pSpecializationInfo = nullptr;
		pipelineStages.push_back(tesselationControlShaderCreateInfo);

		layoutBindings.insert(layoutBindings.end(), shader.layoutBindings.begin(), shader.layoutBindings.end());
		pushConstantRanges.insert(pushConstantRanges.end(), shader.pushConstantRanges.begin(), shader.pushConstantRanges.end());
		uniqueDescriptorTypes.insert(shader.uniqueDescriptorTypes.begin(), shader.uniqueDescriptorTypes.end());
	}

	if (tesselationEvaluationShaderPath != "") {
		Shader shader;
		std::unordered_map<std::string, Shader>::const_iterator mapSearch = shaders.find(tesselationEvaluationShaderPath);
		if (mapSearch == shaders.end()) {
			shader.init(tesselationEvaluationShaderPath);
			shaders.emplace(tesselationEvaluationShaderPath, shader);
		}
		else {
			shader = shaders[tesselationEvaluationShaderPath];
		}
		NEIGE_ASSERT(shader.type == ShaderType::TESSELATION_EVALUATION, "Tesselation evaluation shader in pipeline is not a tesselation evaluation shader.");

		VkPipelineShaderStageCreateInfo tesselationEvaluationShaderCreateInfo = {};
		tesselationEvaluationShaderCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		tesselationEvaluationShaderCreateInfo.pNext = nullptr;
		tesselationEvaluationShaderCreateInfo.flags = 0;
		tesselationEvaluationShaderCreateInfo.stage = shader.shaderTypeToVkShaderFlagBits();
		tesselationEvaluationShaderCreateInfo.module = shader.module;
		tesselationEvaluationShaderCreateInfo.pName = "main";
		tesselationEvaluationShaderCreateInfo.pSpecializationInfo = nullptr;
		pipelineStages.push_back(tesselationEvaluationShaderCreateInfo);

		layoutBindings.insert(layoutBindings.end(), shader.layoutBindings.begin(), shader.layoutBindings.end());
		pushConstantRanges.insert(pushConstantRanges.end(), shader.pushConstantRanges.begin(), shader.pushConstantRanges.end());
		uniqueDescriptorTypes.insert(shader.uniqueDescriptorTypes.begin(), shader.uniqueDescriptorTypes.end());
	}

	if (geometryShaderPath != "") {
		Shader shader;
		std::unordered_map<std::string, Shader>::const_iterator mapSearch = shaders.find(geometryShaderPath);
		if (mapSearch == shaders.end()) {
			shader.init(geometryShaderPath);
			shaders.emplace(geometryShaderPath, shader);
		}
		else {
			shader = shaders[geometryShaderPath];
		}
		NEIGE_ASSERT(shader.type == ShaderType::GEOMETRY, "Geometry shader in pipeline is not a geometry shader.");

		VkPipelineShaderStageCreateInfo geometryShaderCreateInfo = {};
		geometryShaderCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		geometryShaderCreateInfo.pNext = nullptr;
		geometryShaderCreateInfo.flags = 0;
		geometryShaderCreateInfo.stage = shader.shaderTypeToVkShaderFlagBits();
		geometryShaderCreateInfo.module = shader.module;
		geometryShaderCreateInfo.pName = "main";
		geometryShaderCreateInfo.pSpecializationInfo = nullptr;
		pipelineStages.push_back(geometryShaderCreateInfo);

		layoutBindings.insert(layoutBindings.end(), shader.layoutBindings.begin(), shader.layoutBindings.end());
		pushConstantRanges.insert(pushConstantRanges.end(), shader.pushConstantRanges.begin(), shader.pushConstantRanges.end());
		uniqueDescriptorTypes.insert(shader.uniqueDescriptorTypes.begin(), shader.uniqueDescriptorTypes.end());
	}

	NEIGE_ASSERT(pipelineStages.size() != 0, "Graphics pipeline got no stage (no shader given).");

	// Descriptor set layout
	if (descriptorSetLayout == VK_NULL_HANDLE) {
		VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo = {};
		descriptorSetLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		descriptorSetLayoutCreateInfo.pNext = nullptr;
		descriptorSetLayoutCreateInfo.flags = 0;
		descriptorSetLayoutCreateInfo.bindingCount = static_cast<uint32_t>(layoutBindings.size());
		descriptorSetLayoutCreateInfo.pBindings = layoutBindings.data();
		NEIGE_VK_CHECK(vkCreateDescriptorSetLayout(logicalDevice.device, &descriptorSetLayoutCreateInfo, nullptr, &descriptorSetLayout));
	}

	// Descriptor pool
	if (descriptorPool == VK_NULL_HANDLE) {
		std::vector<VkDescriptorPoolSize> descriptorPoolSizes;
		for (std::set<VkDescriptorType>::iterator it = uniqueDescriptorTypes.begin(); it != uniqueDescriptorTypes.end(); it++) {
			VkDescriptorPoolSize descriptorPoolSize = {};
			descriptorPoolSize.type = *it;
			descriptorPoolSize.descriptorCount = 2048;
			descriptorPoolSizes.push_back(descriptorPoolSize);
		}

		if (descriptorPoolSizes.size() != 0) {
			VkDescriptorPoolCreateInfo descriptorPoolCreateInfo = {};
			descriptorPoolCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
			descriptorPoolCreateInfo.pNext = nullptr;
			descriptorPoolCreateInfo.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
			descriptorPoolCreateInfo.maxSets = 8192;
			descriptorPoolCreateInfo.poolSizeCount = static_cast<uint32_t>(descriptorPoolSizes.size());
			descriptorPoolCreateInfo.pPoolSizes = descriptorPoolSizes.data();
			NEIGE_VK_CHECK(vkCreateDescriptorPool(logicalDevice.device, &descriptorPoolCreateInfo, nullptr, &descriptorPool));
		}
	}

	// Pipeline layout
	VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo = {};
	pipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutCreateInfo.pNext = nullptr;
	pipelineLayoutCreateInfo.flags = 0;
	pipelineLayoutCreateInfo.setLayoutCount = 1;
	pipelineLayoutCreateInfo.pSetLayouts = &descriptorSetLayout;
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
	inputAssemblyCreateInfo.topology = topologyToVkTopology();
	inputAssemblyCreateInfo.primitiveRestartEnable = VK_FALSE;

	// Rasterization
	VkPipelineRasterizationStateCreateInfo rasterizationCreateInfo = {};
	rasterizationCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	rasterizationCreateInfo.pNext = nullptr;
	rasterizationCreateInfo.flags = 0;
	rasterizationCreateInfo.depthClampEnable = VK_FALSE;
	rasterizationCreateInfo.rasterizerDiscardEnable = VK_FALSE;
	rasterizationCreateInfo.polygonMode = topology != Topology::WIREFRAME ? VK_POLYGON_MODE_FILL : VK_POLYGON_MODE_LINE;
	rasterizationCreateInfo.cullMode = VK_CULL_MODE_BACK_BIT;
	rasterizationCreateInfo.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
	rasterizationCreateInfo.depthBiasClamp = VK_FALSE;
	rasterizationCreateInfo.depthBiasConstantFactor = 0.0f;
	rasterizationCreateInfo.depthBiasClamp = 0.0f;
	rasterizationCreateInfo.depthBiasSlopeFactor = 0.0f;
	rasterizationCreateInfo.lineWidth = 1.0f;

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
	colorBlendCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
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

	// Dynamic states
	VkPipelineDynamicStateCreateInfo dynamicCreateInfo = {};
	dynamicCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
	dynamicCreateInfo.pNext = nullptr;
	dynamicCreateInfo.flags = 0;
	std::array<VkDynamicState, 2> dynamicStates = { VK_DYNAMIC_STATE_SCISSOR, VK_DYNAMIC_STATE_VIEWPORT };
	dynamicCreateInfo.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
	dynamicCreateInfo.pDynamicStates = dynamicStates.data();

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
	graphicsPipelineCreateInfo.pViewportState = &viewport->viewportCreateInfo;
	graphicsPipelineCreateInfo.pRasterizationState = &rasterizationCreateInfo;
	graphicsPipelineCreateInfo.pMultisampleState = &multisampleCreateInfo;
	graphicsPipelineCreateInfo.pDepthStencilState = &depthStencilCreateInfo;
	graphicsPipelineCreateInfo.pColorBlendState = colorBlend ? &colorBlendCreateInfo : nullptr;
	graphicsPipelineCreateInfo.pDynamicState = &dynamicCreateInfo;
	graphicsPipelineCreateInfo.layout = pipelineLayout;
	graphicsPipelineCreateInfo.renderPass = renderPass->renderPass;
	graphicsPipelineCreateInfo.subpass = 0;
	graphicsPipelineCreateInfo.basePipelineHandle = VK_NULL_HANDLE;
	graphicsPipelineCreateInfo.basePipelineIndex = -1;
	NEIGE_VK_CHECK(vkCreateGraphicsPipelines(logicalDevice.device, VK_NULL_HANDLE, 1, &graphicsPipelineCreateInfo, nullptr, &pipeline));
}

void GraphicsPipeline::destroy() {
	if (descriptorPool != VK_NULL_HANDLE) {
		vkDestroyDescriptorPool(logicalDevice.device, descriptorPool, nullptr);
		descriptorPool = VK_NULL_HANDLE;
	}
	if (descriptorSetLayout != VK_NULL_HANDLE) {
		vkDestroyDescriptorSetLayout(logicalDevice.device, descriptorSetLayout, nullptr);
		descriptorSetLayout = VK_NULL_HANDLE;
	}
	destroyPipeline();
}

void GraphicsPipeline::bind(CommandBuffer* commandBuffer) {
	vkCmdBindPipeline(commandBuffer->commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);
	viewport->setViewport(commandBuffer);
	viewport->setScissor(commandBuffer);
}

void GraphicsPipeline::destroyPipeline() {
	vkDestroyPipelineLayout(logicalDevice.device, pipelineLayout, nullptr);
	vkDestroyPipeline(logicalDevice.device, pipeline, nullptr);
}

VkPrimitiveTopology GraphicsPipeline::topologyToVkTopology() {
	switch (topology) {
	case Topology::TRIANGLE_LIST:
		return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	case Topology::TRIANGLE_STRIP:
		return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP;
	case Topology::LINE_STRIP:
		return VK_PRIMITIVE_TOPOLOGY_LINE_STRIP;
	case Topology::POINT_LIST:
		return VK_PRIMITIVE_TOPOLOGY_POINT_LIST;
	case Topology::WIREFRAME:
		return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	default:
		NEIGE_ERROR("Unsupported primitive topology");
	}
}
